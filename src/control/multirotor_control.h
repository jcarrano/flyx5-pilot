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
 */

#ifndef MULTIROTOR_CONTROL_H
#define MULTIROTOR_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include <fixed_point/types.h>
#include <fixed_point/vector_types.h>
#include <fixed_point/quaternion_types.h>

/**
 * Attitude controller parameters.
 */
struct att_ctrl_params {
	mvec3 position_gain;
	mvec3 speed_gain;
	int16_t int_gain_divide;
	frac integral_error_limit;
};

/**
 * Internal state for the attitude controller.
 */
struct att_ctrl_state {
	struct att_ctrl_params parameters; /*!< Controller parameters. */

	quat att_prev;		/*!< Previous attitude value.	*/
	vec3 error_sat_prev;	/*!< Previous saturated error.	*/
	evec3 integral_out_prev; /*!< Integral accumulator.	*/
	bool integral_enabled;	/*!< True if integral control is enabled.*/
};

extern struct att_ctrl_params ctrl_default_params;

/* ********************* Attitude control ********************************* */

/**
 * Initialize the attitude controller state.
 *
 * The integral control is initially disabled.
 *
 * @param	att_ctrl_state	The controller state.
 */
void att_ctrl_init(struct att_ctrl_state *state, struct att_ctrl_params *params);

/**
 * Enable or disable integral control.
 *
 * @param	att_ctrl_state	The controller state.
 * @param	en		If true, enable the integral control; if false,
 * 				disable it.
 */
void att_ctrl_integral_enable(struct att_ctrl_state *state, bool en);

/**
 * Run a step the attitude controller.
 *
 * @param	att_ctrl_state	The controller state.
 * @param	setpoint	Desired attitude.
 * @param	att		Current attitude estimation.
 * @param	angle_rate	Current angular velocity.
 *
 * @return		Required torques in X, Y and Z
 */
vec3 att_ctrl_step(struct att_ctrl_state *state, quat setpoint, quat att,
								vec3 angle_rate);


/**
 * Transform torques and thust into motor thrust - 4 motor version.
 *
 * Motors are numbered from 0 to 3.
 * In local NED (north-east-down) coordinates, the Z axis points down from the
 * aircraft. Motor 0 is placed along the positive X axis, and the rest are
 * numbered in positive direction around the Z axis.
 *
 * Motors 0 and 2 rotate clockwise.
 * Motors 1 and 3 rotate counter-clockwise.
 *
 * @param	thrust	Desired overall thrust.
 * @param	torque	Desired torque en each axis
 * @param	motor_speeds	Array where the resulting motor thrusts are to
 * 				be placed.
 */
void control_mixer4(frac thrust, vec3 torque, frac motor_thrusts[4]);

#endif /* MULTIROTOR_CONTROL_H */
