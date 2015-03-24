/*
 * quad_setup.c
 *
 *  Created on: 07/03/2015
 *      Author: Juan Ignacio
 */

#include "quad_setup.h"
#include "xdriver/rti.h"
#include "peripheral/esc.h"
#include "peripheral/dmu_simple.h"

#include "debug_tools/stdio_simple.h"
#include "utils/uartstdio.h"

#include "Misc/music.h"

#define ANTI_BOUNCE_MS 400

struct {

	uint8_t measurementCount;
	quat calibration[2];

	bool antiBounce;

}setup_data = {0, {UNIT_QUAT, UNIT_QUAT}, true};

void _setAntiBounce(void* data,  rti_time period, rti_id id);


bool qset_TryDmuCalibration(bool calibrationMode, struct nlcf_state* statePtr)
{

	if (!calibrationMode)
	{
		if (!PIN_ACTIVE(BUTTON_2) || (!setup_data.antiBounce))
		{
			return false;
		}
		else
		{
			calibrationMode = true;
			setup_data.measurementCount = 0;
			setup_data.antiBounce = false;
			rti_Register(_setAntiBounce, &setup_data.antiBounce, RTI_ONCE, RTI_MS_TO_TICKS(ANTI_BOUNCE_MS));
			_puts("Entered calibration mode. Press BTN2 to trigger measures.\n\r");
			return calibrationMode;
		}
	}

	if(!setup_data.antiBounce)
	{
		return calibrationMode;
	}

	struct cal_output calibrationOutput;


	if(!PIN_ACTIVE(BUTTON_2))
	{
		return calibrationMode;
	}

	else
	{
		setup_data.antiBounce = false;
		rti_Register(_setAntiBounce, &setup_data.antiBounce, RTI_ONCE, RTI_MS_TO_TICKS(ANTI_BOUNCE_MS));
	}

	if (setup_data.measurementCount == 0)
	{
		setup_data.calibration[0] = dq_to_q(statePtr->q);			// state is not written during interrupts, no need of "cli"
		setup_data.measurementCount++;
		buzzer_load_score(music_calibration_measure1);
		_puts("First measurement done\n\r");
	}

	else if (setup_data.measurementCount == 1)
	{
		setup_data.calibration[1] = dq_to_q(statePtr->q);
		setup_data.measurementCount++;
		_puts("Second measurement done\n\r");

		calibrationOutput = att_calibrate(setup_data.calibration[0], setup_data.calibration[1]);

		_puts("att calibrate pass\n\r");

		UARTprintf("Cal output: %d\n\r", calibrationOutput.quality);
		UARTprintf("Correction: %d %d %d %d\n\r", calibrationOutput.correction.r.v, calibrationOutput.correction.v.x.v,
				calibrationOutput.correction.v.y.v, calibrationOutput.correction.v.z.v);

		switch (calibrationOutput.quality) {
		case CAL_BAD:
			setup_data.measurementCount = 1;	// Stay looping second measurement.
			buzzer_load_score(music_calibration_bad);
			_puts("Calibrate again.\n\r");
			break;
		case CAL_UGLY:
			buzzer_load_score(music_calibration_ugly);
			setup_data.measurementCount = 1;
			_puts("Calibration ugly. Calibrate again.\n\r");
			break;
		case CAL_GOOD:
			buzzer_load_score(music_calibration_good);
			_puts("Calibration good.\n\r");
			break;
		case CAL_EXCELLENT:
			buzzer_load_score(music_calibration_excellent);
			_puts("Calibration excellent.\n\r");
			break;
		}

		if ((calibrationOutput.quality != CAL_BAD) || (calibrationOutput.quality != CAL_UGLY)) {
			nlcf_apply_correction(statePtr, calibrationOutput);
			_puts("Calibration applied. Press BTN2 to end.\n\r");
		}

	}
	else if (setup_data.measurementCount == 2)
	{
		calibrationMode = false;
		_puts("Calibrated. \n\r");
	}

	return calibrationMode;
}

void _setAntiBounce(void* data,  rti_time period, rti_id id)
{
	*((bool*)data) = true;
}

