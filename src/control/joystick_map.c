/**
 * @file
 * @author Juan I Carrano
 * @author Nicolás Venturo
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
 * Functions for mapping joystick inputs to attitude and height setpoints.
 */

#include <fixed_point/fixed_point.h>
#include <fixed_point/vector.h>
#include "joystick_map.h"
#include "../Misc/sqrt.h"

/**
 * Non-linear mapping of the throttle stick into altitude.
 */
static frac altitude_mapping(frac t)
{
	return t;
}

/**
 * Take the square root of a number less than or equal to 3.
 */
static efrac norm2_sqrt(efrac f)
{
	/* sqrt(x/4)*2 = sqrt(x) */
	return _efrac(f_sqrt(_frac(f.v / 4), 4).v * 2);
}

multirotor_setpoint joystick_to_setpoint(const joy_data_t data)
{
	vec3 stick;
	frac throttle;
	efrac norm2;
	multirotor_setpoint newSetpoint;

	stick.x = _frac(data.roll >> 2);
	stick.y = _frac(data.pitch >> 2);
	stick.z = _frac(data.yaw >> 2);

	throttle = _frac(data.elev);

	norm2 = f_to_ef(f_mul(stick.x, stick.x));
	norm2 = ef_f_add(norm2, f_mul(stick.y, stick.y));
	norm2 = ef_f_add(norm2, f_mul(stick.z, stick.z));

	if (norm2.v > FRAC_1_V)
	{
		/* Se está dividiendo por un número mayor a 1, stick tiene que dar menor a lo que era. */
		stick = ev_to_v(
			v_efdiv_ev(stick, norm2_sqrt(norm2)));
		newSetpoint.attitude.r = FZero;
	} else {
		newSetpoint.attitude.r = f_sqrt(
				ef_to_f(ef_sub(_efrac(EFRAC_1_V), norm2)), 4);
	}
	newSetpoint.attitude.v = stick;
	newSetpoint.altitude = altitude_mapping(throttle);

	return newSetpoint;
}
