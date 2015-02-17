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

iic_commData_T* dmu_commDataPtr = &(iic_commData[DMU_MODULE_NUMBER]);

void dmu_Init()
{
//	u16 offsetSampleRate;
//	rti_id offsetTask;

	if (dmu_data.init == true)
		return;

	iic_Init(DMU_MODULE_NUMBER);
	gpio_Init(DMU_INT_PORT_NUM, DMU_INT_PIN, GPIO_RISING_EDGE);

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

		dmu_commDataPtr->data[0] = ADD_PWR_MGMT_1;
		dmu_commDataPtr->data[1] = 0;//PWR_MGMT_1_STOP;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		break;


	case 1:

		dmu_commDataPtr->data[0] = ADD_PWR_MGMT_1;
		dmu_commDataPtr->data[1] = 0;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		break;

	case 2:
		// Note: inserting delay / putchars here screws configuration up.

		dmu_commDataPtr->data[0] = ADD_SAMPLE_RATE_DIVIDER;
		dmu_commDataPtr->data[1] = SAMPLE_RATE_DIVIDER;	// 25
		dmu_commDataPtr->data[2] = CONFIG;				// 26
		dmu_commDataPtr->data[3] = GYRO_CONFIG(GYRO_X_SELFTEST, GYRO_Y_SELFTEST, GYRO_Z_SELFTEST);		// 27
		dmu_commDataPtr->data[4] = ACCEL_CONFIG(ACCEL_X_SELFTEST, ACCEL_Y_SELFTEST, ACCEL_Z_SELFTEST);	//28
		dmu_commDataPtr->data[5] = FREE_FALL_THRESHOLD;
		dmu_commDataPtr->data[6] = FREE_FALL_DURATION;
		dmu_commDataPtr->data[7] = MOTION_INT_THRESHOLD;
		dmu_commDataPtr->data[8] = MOTION_INT_DURATION;
		dmu_commDataPtr->data[9] = ZERO_MOTION_THRESHOLD;
		dmu_commDataPtr->data[10] = ZERO_MOTION_DURATION;
		dmu_commDataPtr->data[11] = FIFO_ENABLE;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 12, NULL);

		dmu_data.stage++;

		break;

	case 3:

		dmu_commDataPtr->data[0] = ADD_INT_PIN_CFG;
		dmu_commDataPtr->data[1] = INT_PIN_CFG;		// 55
		dmu_commDataPtr->data[2] = INT_ENABLE;


		dmu_Send(dmu_StagesInit, dmu_InitFailed, 3, NULL);

		dmu_data.stage++;

		break;

	case 4:

		dmu_commDataPtr->data[0] = ADD_SIGNAL_PATH_RESET;
		dmu_commDataPtr->data[1] = RESET_SIGNAL(1,1,1);
		dmu_commDataPtr->data[2] = MOTION_DETECT_CTRL;
		dmu_commDataPtr->data[3] = USER_CTRL(0,1,1);	// Run means not reset.
		dmu_commDataPtr->data[4] = PWR_MGMT_1_RUN;
		// PWR_MGMT_2 stays in 0 (reset value).

		dmu_Send(dmu_StagesInit, dmu_InitFailed, 5, NULL);

		dmu_data.stage++;

		break;

	case 5:

		dmu_commDataPtr->data[0] = ADD_USER_CTRL;
		dmu_commDataPtr->data[1] = USER_CTRL_INIT;	// Run means not reset.

		dmu_Send(dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		break;


	case 6:
		// Fifo reset
		dmu_commDataPtr->data[0] = ADD_USER_CTRL;
		dmu_commDataPtr->data[1] = USER_CTRL(FIFO_MASTER_DISABLE, FIFO_RESET, 1);
		dmu_Send(dmu_StagesInit, dmu_InitFailed, 2, NULL);
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

	while(1);

}

void dmu_GetMeasurements(iic_userAction cb)
{
	dmu_ReceiveFromRegister(ADD_ACCEL_OUT, cb, dmu_CommFailed, sizeof(dmu_measurements), (uint8_t*)&dmu_measurements);
	return;
}

int16_t e16toh(const uint16_t big_endian_16bits);

void dmu_PrintFormattedMeasurements(void)
{
	struct dmu_measurements_T* dm = &dmu_measurements;
	UARTprintf("ax: %d, ay: %d, az: %d\ngx: %d, gy: %d, gz: %d\n", (int)e16toh(dm->accel.x), (int)e16toh(dm->accel.y), (int)e16toh(dm->accel.z), (int)e16toh(dm->gyro.x), (int)e16toh(dm->gyro.y), (int)e16toh(dm->gyro.z));
	return;
}

int16_t e16toh(const uint16_t big_endian_16)
{
	return (int16_t) (((big_endian_16 >> 8) & 0x00FF) | ((big_endian_16 << 8) & 0xFF00));
}

void dmu_CommFailed(void)
{
	UARTprintf("dmu comm fail\n\r");
}
