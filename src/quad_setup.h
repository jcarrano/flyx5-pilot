/*
 * quad_setup.h
 *
 *  Created on: 07/03/2015
 *      Author: Juan Ignacio
 */

#ifndef QUAD_SETUP_H_
#define QUAD_SETUP_H_

#include "flyx5_hw.h"
#include "control/nlcf.h"


#define ESC_SETUP_BUTTON BUTTON_1
#define DMU_SETUP_BUTTON BUTTON_2

//! Handles IMU calibration. Use inside a loop with updated statePtr from nlcf filter.
//!
//! @param calibrationMode Set true if user is already calibrating the device when the function is called, or false if not.
//! @param statePtr Reference to current filter status.
//! @return Returns true if the device's calibration process is still going on, false otherwise.
//! @note Usage: press BTN2 to enter calibration mode, then to trigger measurement 1 & 2, and once more to accept and finish.
//! @note Sounds will be triggered after entering calibration mode, and after second measurement to tell the user how precise the process was.

bool qset_TryDmuCalibration(bool calibrationMode, struct nlcf_state* statePtr);

#endif /* QUAD_SETUP_H_ */
