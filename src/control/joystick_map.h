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

#ifndef JOYSTICK_MAP_H
#define JOYSTICK_MAP_H

#include <stdint.h>
#include <fixed_point/types.h>
#include <fixed_point/quaternion_types.h>

/**
 * Set-point for a quadrotor.
 */
typedef struct{
	quat attitude;	/*!< Desired orientation. */
	frac altitude;  /*!< Desired altitude. */
} quad_setpoint;

/**
 * Joystick / Controller data.
 */
typedef struct
{
	int32_t roll;
	int32_t pitch;
	int32_t yaw;
	uint32_t elev;
} joy_data_t;

/**
 * Convert the joystick commmands into a quaternion setpoint.
 */
quad_setpoint joystick_to_setpoint(const joy_data_t data);

#endif /* JOYSTICK_MAP_H */
