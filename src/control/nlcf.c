/**
 * @file nlcf.c
 * @author Juan I Carrano
 * @copyright	Copyright 2013 by Juan I Carrano, Andrés Calcabrini, Juan I. Ubeira and
 * 		Nicolás Venturo. All rights reserved. \n
 * 		\n
 *		This program is free software: you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation, either version 3 of the License, or
 *		(at your option) any later version. \n
 * 		\n
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details. \n
 * 		\n
 *		You should have received a copy of the GNU General Public License
 *		along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Non linear complementary filter for attitude estimation.
 *
 * References:
 * 	R. Mahony, T. Hamel, and J.-M. Pflimlin, "Non-linear complementary
 * 	filters on the special orthogonal group," IEEE Trans. Automatic Control,
 * 	vol. 53, no. 5, pp. 1203-1218, June 2008.
 *
 */

#if (defined NLCF_DEBUG) || (defined CALIBRATION_DEBUG)
#include <stdio.h>
#include <stdlib.h>
#endif

#include <stddef.h>
#include "nlcf.h"

/* El samplerate es 1000  Hz */
#ifndef NLCF_SAMPLERATE
#define NLCF_SAMPLERATE 1000
#endif

#include "nlcf_constants.h"

/* Non-linear complementary filter on SO(3) */

// Para calibrar, Q_Correction tiene que ser inicialmente UNIT_QUAT.
#ifdef MAIN_CALIBRATE
static quat Q_Correction = UNIT_QUAT;
#else
#define _q_correction {{-762}, {{973}, {32745}, {128}}}
static quat Q_Correction = _q_correction;
struct cal_output cal_correction = {CAL_EXCELLENT, _q_correction};
#endif

static inline vec3 z_dir(quat q)
{
	vec3 zd;
	frac tmp;
	const frac _1 = {FRAC_1_V};

	zd.x = f_imul(f_sub(f_mul(q.v.z, q.v.x), f_mul(q.v.y, q.r)), 2);
	zd.y = f_imul(f_add(f_mul(q.v.z, q.v.y), f_mul(q.v.x, q.r)), 2);
	tmp = f_add(f_mul(q.v.x, q.v.x), f_mul(q.v.y, q.v.y));
	zd.z = f_sub(f_sub(_1, tmp), tmp);

	return zd;
}

static inline vec3 verror(vec3 y, vec3 x)
{
	vec3 zd;

	zd.x = f_sub(f_mul(y.z, x.y), f_mul(x.z, y.y));
	zd.y = f_sub(f_mul(y.x, x.z), f_mul(x.x, y.z));
	zd.z = f_sub(f_mul(y.y, x.x), f_mul(x.y, y.x));

	return zd;
}

void nlcf_init(struct nlcf_state *state)
{
	state->q = dquat_Unit;
	state->bias = vec3_Zero;
	state->correction = quat_Unit;
}

void nlcf_process(struct nlcf_state *state, vec3 gyro, vec3 accel, vec3 *gyro_out)
{
	vec3 z_estim, wmes, d_bias;
	quat p;
	quat q_lowres = dq_to_q(state->q);
	dquat d_q, correction;
	frac err;

	/* Convert measurements to body reference frame */
	gyro = q_rot(state->correction, gyro);
	accel = q_rot(state->correction, accel);

	if (gyro_out != NULL)
		*gyro_out = gyro;

	/* wmes, bias */
	z_estim = z_dir(q_lowres);

	wmes = verror(accel, z_estim);

	d_bias = v_idiv(wmes, D_BIAS_SCALE);

#ifdef NLCF_DEBUG
	printf("%g, %g, %g, ",	F_TO_DOUBLE(z_estim.x),
				F_TO_DOUBLE(z_estim.y),
				F_TO_DOUBLE(z_estim.z));

	printf("%g, %g, %g, ",	F_TO_DOUBLE(wmes.x),
				F_TO_DOUBLE(wmes.y),
				F_TO_DOUBLE(wmes.z));

	printf("%g, %g, %g, ",	F_TO_DOUBLE(state->bias.x),
				F_TO_DOUBLE(state->bias.y),
				F_TO_DOUBLE(state->bias.z));
#endif /* NLCF_DEBUG */

	/* d_q */
	p.r = FZero;
	/* la correccion de bias se estaba portando mal */
	/* p.v = v_{add, sub}(v_add(gyro, v_div(bias, BIAS_SCALE2)),
	 * 					vmul(wmes, WMES_MUL)); */

	#ifdef WMES_DIV
	p.v = v_add(gyro, v_idiv(wmes, WMES_DIV));
	#else
	p.v = v_add(gyro, v_imul(wmes, WMES_MUL));
	#endif

	d_q = q_mul_s_dq(q_lowres, p, D_Q_SCALE);

	/* bias, q */
	state->bias = v_sub(state->bias, d_bias);
	state->q = dq_add(state->q, d_q);

	/* renormalization */
	err = q_xnormerror(dq_to_q(state->q));
	correction = q_scale_dq(q_lowres, err);
	state->q = dq_add(state->q, correction);
}

/* Calibración:

	p es la orientación del IMU con respecto al body:
		X_body = p*X_imu*p'
	q es la orientación del body con respecto la tierra:
		X_earth = q*X_body*q'

	por lo tanto:
		X_earth = q*p*X_imu*p'*q'
		q*p = M (salida del estimador) => X_earth = M*X_imu*M'
	para obtener q:
		M*p' = q*p*p' = q (porque son cuaterniones unitarios)

	La otra manera es convertir las mediciones al marco de referencia
	del vehículo:
		w_body = p*w_imu*p'

*/

#define CAL_ITERATIONS 12

#define Q_COMPONENTS(q) (q).r.v, (q).v.x.v, (q).v.y.v, (q).v.z.v

struct qpair {
	quat p0, p1;
};

struct qpair _calibrate(quat mes0, quat mes1)
{
	const quat pos0 = UNIT_QUAT, pos1 = {{23170}, {{16384}, {16384}, {0}}};
	quat k0 = UNIT_QUAT, k1 = UNIT_QUAT;
	dquat p0, p1;
	struct qpair r;
	int i;

	for (i = 0; i < CAL_ITERATIONS; i++) {
		p0 = q_mul_dq(q_conj(q_mul(k0, pos0)), mes0);
		p1 = q_mul_dq(q_conj(q_mul(k1, pos1)), mes1);

		p0 = dq_xrenorm(p0);
		p1 = dq_xrenorm(p1);
#ifdef CALIBRATION_DEBUG
		{
			printf("%d %d %d %d, ", Q_COMPONENTS(dq_to_q(p0)));
			printf("%d %d %d %d\n", Q_COMPONENTS(dq_to_q(p1)));
		}
#endif /*CALIBRATION_DEBUG*/
		k0 = q_mul(mes0, q_conj(q_mul(pos0, dq_to_q(p1))));
		k1 = q_mul(mes1, q_conj(q_mul(pos1, dq_to_q(p0))));

		k0 = q_udecompose(k0, AXIS_Z);
		k1 = q_udecompose(k1, AXIS_Z);
	}

	r.p0 = dq_to_q(p0);
	r.p1 = dq_to_q(p1);

	return r;
}

const dfrac ERR_1DG = {1073700939};	/*!< 1 degree error threshold. */
const dfrac ERR_2DG = {1073578288};	/*!< 2 degrees error threshold. */
const dfrac ERR_5DG = {1072719860};	/*!< 5 degrees error threshold. */

struct cal_output att_calibrate(quat mes0, quat mes1)
{
	struct qpair p;
	dfrac err;
	struct cal_output r;

	p = _calibrate(mes0, mes1);
	err = q_mul_dq(p.p0, q_conj(p.p1)).r;

	//r.correction = q_conj(p.p0);
	r.correction = p.p0;

	if (df_gt(err, ERR_1DG))
		r.quality = CAL_EXCELLENT;
	else if (df_gt(err, ERR_2DG))
		r.quality = CAL_GOOD;
	else if (df_gt(err, ERR_5DG))
		r.quality = CAL_UGLY;
	else
		r.quality = CAL_BAD;

	return r;
}

void nlcf_apply_correction(struct nlcf_state *state, struct cal_output c)
{
	state->correction = c.correction;
}

void nlcf_reset_correction(struct nlcf_state *state)
{
	state->correction = quat_Unit;
}

/* *********************** Debug support ************************************/

#ifdef NLCF_DEBUG
int main(int argc, char *argv[])
{
	struct nlcf_state state;
	struct cal_output cal = {0, {{318}, {{-23544}, {-22788}, {-285}}}};

	nlcf_init(&state);
	nlcf_apply_correction(&state, cal);

        while (1) {
                int n;
                vec3 gyro;
                vec3 acc;
                quat q;
		vec3 gyro_bff;

                n = scanf("%hd %hd %hd %hd %hd %hd\n", &acc.x.v, &acc.y.v, &acc.z.v,
                                                &gyro.x.v, &gyro.y.v, &gyro.z.v);

                if (n != 6)
                        break;

                nlcf_process(&state, gyro, acc, &gyro_bff);

                printf("%g, %g, %g, %g ",
                                DF_TO_DOUBLE(state.q.r),
                                DF_TO_DOUBLE(state.q.v.x),
                                DF_TO_DOUBLE(state.q.v.y),
                                DF_TO_DOUBLE(state.q.v.z));

                printf("\n");
        }

        return 0;
}
#endif /* NLCF_DEBUG */

#ifdef CALIBRATION_DEBUG
const quat real_mes0 = {{-15530}, {{-137}, {-1013}, {28837}}};
const quat real_mes1 = {{7445}, {{21550}, {-6203}, {22705}}};

int main(int argc, char **argv)
{
	quat mes0, mes1;
	struct cal_output result;

	if (argc >= 9) {
		mes0.r.v = strtol(argv[1], NULL, 0);
		mes0.v.x.v = strtol(argv[2], NULL, 0);
		mes0.v.y.v = strtol(argv[3], NULL, 0);
		mes0.v.z.v = strtol(argv[4], NULL, 0);


		mes1.r.v = strtol(argv[5], NULL, 0);
		mes1.v.x.v = strtol(argv[6], NULL, 0);
		mes1.v.y.v = strtol(argv[7], NULL, 0);
		mes1.v.z.v = strtol(argv[8], NULL, 0);
	} else {
		mes0 = real_mes0;
		mes1 = real_mes1;
	}

	result = att_calibrate(mes0, mes1);

	return 0;
}
#endif /* CALIBRATION_DEBUG */
