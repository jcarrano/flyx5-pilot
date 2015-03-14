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

bool qset_TryDmuCalibration(bool calibrationMode, struct nlcf_state* statePtr);

#endif /* QUAD_SETUP_H_ */
