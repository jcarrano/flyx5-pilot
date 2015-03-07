/**
 * @file
 * @author Juan I Carrano
 * @author Juan I Ubeira
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
 * Attitude and altitude control for multi-rotor aircraft.
 *
 */

#include "multirotor_control.h"
#include "fixed_point/quaternion.h"
#include "fixed_point/vector.h"
#include "fixed_point/fixed_point.h"

void att_ctrl_init(struct att_ctrl_state *state, struct att_ctrl_params *params)
{
	state->parameters = *params;

	state->att_prev = quat_Unit;
	state->error_sat_prev = vec3_Zero;
	state->integral_out_prev = evec3_Zero;
	state->integral_enabled = false;
}

void att_ctrl_integral_enable(struct att_ctrl_state *state, bool en)
{
	state->integral_enabled = en;
}

vec3 att_ctrl_step(struct att_ctrl_state *state, quat setpoint, quat att,
								vec3 angle_rate)
{
	vec3 error = q_error(setpoint, att);
	evec3 ctrl_signal;

	vec3 error_sat = v_clip(error, state->parameters.integral_error_limit);
	evec3 integral_out;

	if (!state->integral_enabled) {
		integral_out = evec3_Zero;
	} else {
		/* Integrate with the trapezium rule */
		integral_out = ev_add(ev_add(
			v_to_ev(state->error_sat_prev), v_to_ev(error_sat)),
						state->integral_out_prev);
	}

	state->integral_out_prev = integral_out;
	state->error_sat_prev = error_sat;

	/* Disable the integral in the z direction as we do not have a magnetometer */
	integral_out.z = EFZero;

	ctrl_signal = 	ev_add(
			    ev_sub(
			        v_mvmul_ev(error, state->parameters.position_gain),
				v_mvmul_ev(angle_rate, state->parameters.speed_gain)
				),
				ev_idiv(integral_out, state->parameters.int_gain_divide)
			);

	state->att_prev = att;

	return ev_to_v(ctrl_signal);
}

/**
 * Calculate a row of Gamma^-1 * T.
 *
 * Gamma is a matrix that maps motor thrusts into torques and net thrust.
 * The inverse of this matrix can be used to map torques and net thrust into
 * motor thust by the formula V = Gamma^-1 * T, where V are the motor thrusts
 * and T is a vector formed by the torques and net thrust.
 * This function calculates one element of V.
 *
 * If the result is negative, it is clipped to zero.
 *
 */
frac gammainv(frac T, frac t1, frac t2, frac t3)
{
	efrac r;
	frac r1;

	r = f_to_ef(T);		/* Mix in the thrust */
	r = ef_f_add(r, t1);	/* Add roll. */
	r = ef_f_add(r, t2);	/* Pitch. */
	r = ef_f_add(r, f_imul(t2, 2));	/* Yaw must be amplified. */

	r1 = ef_to_f(r);
	return f_gt(r1, FZero)? r1 : FZero;
}

void control_mixer4(frac thrust, vec3 torque, frac motor_thrusts[4])
{
	motor_thrusts[0] = gammainv(thrust, FZero,           torque.y,        f_neg(torque.z));
	motor_thrusts[1] = gammainv(thrust, f_neg(torque.x), FZero,           torque.z);
	motor_thrusts[2] = gammainv(thrust, FZero,           f_neg(torque.y), f_neg(torque.z));
	motor_thrusts[3] = gammainv(thrust, torque.x,        FZero,           torque.z);

	return;
}
