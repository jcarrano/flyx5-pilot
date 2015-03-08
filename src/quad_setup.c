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


struct {

	uint8_t measurementCount;
	quat calibration[2];

	bool antiBounce;

}setup_data;

void _setAntiBounce(void* data,  rti_time period, rti_id id);


void qset_TryEscCalibration(void)
{
    if(PIN_ACTIVE(ESC_SETUP_BUTTON))
    {
    	esc_SetValues(ESC_MAX_VALUE, ESC_MAX_VALUE, ESC_MAX_VALUE, ESC_MAX_VALUE);

    	// Wait for ESCs to buzz, then release button 1.
    	while(PIN_ACTIVE(ESC_SETUP_BUTTON))
    		;

    	esc_SetValues(ESC_MIN_VALUE, ESC_MIN_VALUE, ESC_MIN_VALUE, ESC_MIN_VALUE);
    }

    return;
}

bool qset_TryDmuCalibration(bool calibrationMode, struct nlcf_state* statePtr)
{
	if (!calibrationMode)
	{
		if (!PIN_ACTIVE(BUTTON_2))
		{
			return false;
		}
		else
		{
			calibrationMode = true;
			setup_data.measurementCount = 0;
			setup_data.antiBounce = false;
			rti_Register(_setAntiBounce, &setup_data.antiBounce, RTI_ONCE, RTI_MS_TO_TICKS(120));
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
		rti_Register(_setAntiBounce, &setup_data.antiBounce, RTI_ONCE, RTI_MS_TO_TICKS(120));
	}

	if (setup_data.measurementCount == 0)
	{
		setup_data.calibration[0] = dq_to_q(statePtr->q);			// state is not written during interrupts, no need of "cli"
		setup_data.measurementCount++;
		_puts("First measurement done\n\r");
	}

	else if (setup_data.measurementCount == 1)
	{
		setup_data.calibration[1] = dq_to_q(statePtr->q);
		setup_data.measurementCount++;
		_puts("Second measurement done\n\r");

		calibrationOutput = att_calibrate(setup_data.calibration[0], setup_data.calibration[1]);
		UARTprintf("Cal output: %d\n\r", calibrationOutput.quality);
		UARTprintf("Correction: %d %d %d %d\n\r", calibrationOutput.correction.r.v, calibrationOutput.correction.v.x.v,
				calibrationOutput.correction.v.y.v, calibrationOutput.correction.v.z.v);

		if (calibrationOutput.quality == CAL_BAD)
		{
			setup_data.measurementCount = 1;	// Stay looping second measurement.
			_puts("Calibrate again\n\r");
		}
		nlcf_apply_correction(statePtr, calibrationOutput);

		_puts("Press BTN2 to exit calibration. \n\r");

	}
	else if (setup_data.measurementCount == 2)
	{
		calibrationMode = false;
	}


	return calibrationMode;
}

void _setAntiBounce(void* data,  rti_time period, rti_id id)
{
	*((bool*)data) = true;
}

