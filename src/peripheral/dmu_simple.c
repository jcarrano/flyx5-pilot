#include "dmu_simple.h"
#include "../xdriver/iic_interface.h"
#include "../debug_tools/debug.h"
#include "../utils/uartstdio.h"

struct dmu_data_T dmu_data = {false, NULL, 0, {false, 0, 0, 0, 0, NULL} };

// Measurements taken individually must respect MPU internal registers' order
struct dmu_measurements_T dmu_measurements;


void dmu_InitFailed(void);
void dmu_StagesInit();
void dmu_CommFailed(void);

void dmu_Init()
{
//	u16 offsetSampleRate;
//	rti_id offsetTask;

	if (dmu_data.init == true)
		return;

	iic_Init(DMU_MODULE_NUMBER);

	dmu_StagesInit();

	while (dmu_data.init == false)
		;

	return;

}

void dmu_StagesInit()
{

	switch (dmu_data.stage)
	{
	case 0:

		iic_commData.data[0] = ADD_PWR_MGMT_1;
		iic_commData.data[1] = PWR_MGMT_1_STOP;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		break;


	case 1:

		iic_commData.data[0] = ADD_PWR_MGMT_1;
		iic_commData.data[1] = 0;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		break;

	case 2:
		// Note: inserting delay / putchars here screws configuration up.

		iic_commData.data[0] = ADD_SAMPLE_RATE_DIVIDER;
		iic_commData.data[1] = SAMPLE_RATE_DIVIDER;	// 25
		iic_commData.data[2] = CONFIG;				// 26
		iic_commData.data[3] = GYRO_CONFIG(GYRO_X_SELFTEST, GYRO_Y_SELFTEST, GYRO_Z_SELFTEST);		// 27
		iic_commData.data[4] = ACCEL_CONFIG(ACCEL_X_SELFTEST, ACCEL_Y_SELFTEST, ACCEL_Z_SELFTEST);	//28
		iic_commData.data[5] = FREE_FALL_THRESHOLD;
		iic_commData.data[6] = FREE_FALL_DURATION;
		iic_commData.data[7] = MOTION_INT_THRESHOLD;
		iic_commData.data[8] = MOTION_INT_DURATION;
		iic_commData.data[9] = ZERO_MOTION_THRESHOLD;
		iic_commData.data[10] = ZERO_MOTION_DURATION;
		iic_commData.data[11] = FIFO_ENABLE;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 12, NULL);

		dmu_data.stage++;

		break;

	case 3:

		iic_commData.data[0] = ADD_INT_PIN_CFG;
		iic_commData.data[1] = INT_PIN_CFG;		// 55
		iic_commData.data[2] = INT_ENABLE;


		dmu_Send(dmu_StagesInit, dmu_InitFailed, 3, NULL);

		dmu_data.stage++;

		break;

	case 4:

		iic_commData.data[0] = ADD_SIGNAL_PATH_RESET;
		iic_commData.data[1] = RESET_SIGNAL(1,1,1);
		iic_commData.data[2] = MOTION_DETECT_CTRL;
		iic_commData.data[3] = USER_CTRL(0,1,1);	// Run means not reset.
		iic_commData.data[4] = PWR_MGMT_1_RUN;
		// PWR_MGMT_2 stays in 0 (reset value).

		dmu_Send(dmu_StagesInit, dmu_InitFailed, 5, NULL);

		dmu_data.stage++;

		break;

	case 5:

		iic_commData.data[0] = ADD_USER_CTRL;
		iic_commData.data[1] = USER_CTRL_INIT;	// Run means not reset.

		dmu_Send(dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		break;


	case 6:

		dmu_FifoReset(dmu_StagesInit);
		dmu_data.stage++;

		break;


	case 7:

		dmu_data.init = true;
		dmu_data.stage = 0;

		break;

	default:
		break;
	}

	return;
}

#include "../debug_tools/led.h"

void dmu_InitFailed()
{
#ifdef DMU_DEBUG_INIT
	Putchar('i');Putchar('f');Putchar('a');Putchar('i');Putchar('l');

#endif

	gled_On();

	while(1);

}

void dmu_GetMeasurements(iic_userAction cb)
{
	dmu_ReceiveFromRegister(ADD_ACCEL_OUT, cb, dmu_CommFailed, sizeof(dmu_measurements), (uint8_t*)&dmu_measurements);
	return;
}

void dmu_PrintFormattedMeasurements(void)
{
	struct dmu_measurements_T* dm = &dmu_measurements;
	UARTprintf("ax: %d, ay: %d, az: %d\ngx: %d, gy: %d, gz: %d\n", dm->accel.x, dm->accel.y, dm->accel.z, dm->gyro.x, dm->gyro.y, dm->gyro.z);
	return;
}

void dmu_CommFailed(void)
{
	UARTprintf("dmu comm fail\n\r");
}
