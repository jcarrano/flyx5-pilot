/**
 * @file nlcf.h
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
 * Non-linear complementary filter.
 */

#ifndef NLCF_H
#define NLCF_H

#include <fixed_point/quaternion.h>
#include <fixed_point/vector.h>

/**
 * Calibration quality.
 */
typedef enum CAL_QUALITY {
	CAL_EXCELLENT,	/*!< Solution is accurate by less than 1 degree. */
	CAL_GOOD,	/*!< Solution is accurate by less than 2 degrees. */
	CAL_UGLY,	/*!< Solution is accurate by less than 5 degrees. */
	CAL_BAD		/*!< Solution is accurate by more than 5 degrees. */
} CAL_QUALITY;

/**
 * Non linear complementary filter state.
 */
struct nlcf_state {
	dquat q;		/*!< Current attitude estimate. */
	vec3 bias;		/*!< Current gyro bias estimate. */
	quat correction;	/*!< Current IMU rotation correction. */
};

/**
 * Result of the calibration procedure.
 */
struct cal_output {
	CAL_QUALITY quality; /*!< Accuracy of the calculated parameters. Depends
			Upon the quality of the measurements used to calibrate. */
	quat correction; /*!< Orientation of the IMU relative to the body. */
};

/**
 * Initialize the non linear complementary filter.
 *
 * @param	state	Pointer to the filter state data.
 *
 * @note This function resets the correction data.
 */
void nlcf_init(struct nlcf_state *state);

/**
 * Process measurements and update attitude estimation.
 *
 * This function converts the sensor measurements to the corrected reference
 * frame before executing the algorithm. The corrected measurements are made
 * available via output parameters.
 *
 * @param	state	Pointer to the filter state data.
 * @param	gyro	Gyroscope measurement.
 * @param	accel	Accelerometer measurement.
 * @param	gyro_out	Optional. If not NULL, the corrected gyro measurement
 * 				is stored here.
 */
void nlcf_process(struct nlcf_state *state, vec3 gyro, vec3 accel, vec3 *gyro_out);

/**
 * Load the IMU rotation correction parameters into the filter.
 */
void nlcf_apply_correction(struct nlcf_state *state, struct cal_output c);

/**
 * Reset the correction parameters for IMU rotation.
 */
void nlcf_reset_correction(struct nlcf_state *state);

/**
 * Calculate the IMU rotation correction parameters.
 *
 * This is used to compensate for the orientation of the IMU relative to the
 * body.
 * The correction must be reset by a call to @ref nlcf_reset_correction, and the
 * filter must have time to stabilize before taking measurements.
 *
 * @param	mes0	Attitude measurement taken when the body is not tilted.
 * @param	mes0	Attitude measurement taken when the body is tilted 90
 * 			degrees along the x axis.
 *
 * @return	Calibration output. Consist of a calibration quality, and a
 * 		correction parameter. The calibration quality is dependent of
 * 		the quality of the measurements. If the measurements are not
 * 		90 degrees from each other, the quality will be worse.
 */
struct cal_output att_calibrate(quat mes0, quat mes1);

#endif /*NLCF_H*/
