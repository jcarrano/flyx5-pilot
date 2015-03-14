#include "dmu_simple.h"
#include "../xdriver/iic_interface.h"
#include "../debug_tools/debug.h"
#include "../utils/uartstdio.h"
#include "../xdriver/gpio_interface.h"
#include "../Misc/error.h"

struct dmu_data_T dmu_data;

struct dmu_samples_T dmu_offset = {VEC0, VEC0};

iic_commData_T* dmu_commDataPtr = &(iic_commData[DMU_MODULE_NUMBER]);

struct dmu_longSamples_T
{
	vec3_s32 accel;
	vec3_s32 gyro;
};

void dmu_InitFailed(void);
void dmu_StagesInit();
void dmu_CommFailed(void);
void dmu_SamplesReady(void);
void dmu_GetMeasurements(void);
void dmu_AddSamples(struct dmu_longSamples_T * const acc, const struct dmu_samples_T* samples);
void dmu_DivideLongSamples(struct dmu_longSamples_T * const acc, int32_t divider);
void dmu_SubstractOffset(struct dmu_samples_T* samples);

#define dmu_Send(eotCB, commFailedCB, toWrite, sendBuffer)										\
	(iic_Send (DMU_MODULE_NUMBER, DMU_ADDRESS, eotCB, commFailedCB, toWrite, sendBuffer) )

#define dmu_ReceiveFromRegister(regAddress, eotCB, commFailedCB, toRead, receiveBuffer)			\
	(iic_ReceiveFromRegister (DMU_MODULE_NUMBER, regAddress, DMU_ADDRESS, eotCB, 				\
			commFailedCB, toRead, receiveBuffer) )

#define dmu_ReceiveSamples(commFailedCB, toRead, receiveBuffer)												\
	(iic_ReceiveFromRegister (DMU_MODULE_NUMBER, ADD_ACCEL_OUT, DMU_ADDRESS, dmu_SamplesReady, 				\
			commFailedCB, toRead, receiveBuffer) )

void dmu_Init()
{
//	u16 offsetSampleRate;
//	rti_id offsetTask;

	if (dmu_data.init == true)
		return;

	dmu_data.stage = 0;
	dmu_data.status = DMU_IDLE;
	dmu_data.correctOffset = false;

	iic_Init(DMU_MODULE_NUMBER);
	gpio_Init(DMU_INT_PORT_NUM, DMU_INT_PIN, GPIO_RISING_EDGE);

	dmu_StagesInit();

	while (dmu_data.init == false)
		;

	return;

}

static int16_t _parse_sample(const uint8_t sample[2])
{
	//return (int16_t) (((big_endian_16 >> 8) & 0x00FF) | ((big_endian_16 << 8) & 0xFF00));
	return ((uint16_t)(sample[0])) * 256 + sample[1];
}

static struct dmu_samples_T _parse_frame()
{
	struct dmu_samples_T r;

#define _cvt(n) _frac(_parse_sample(dmu_data._frame_buffer[n]))

	r.accel.x = _cvt(DMU_FRAME_ACCEL_X);
	r.accel.y = _cvt(DMU_FRAME_ACCEL_Y);
	r.accel.z = _cvt(DMU_FRAME_ACCEL_Z);

	r.gyro.x = _cvt(DMU_FRAME_GYRO_X);
	r.gyro.y = _cvt(DMU_FRAME_GYRO_Y);
	r.gyro.z = _cvt(DMU_FRAME_GYRO_Z);

#undef _cvt

	return r;
}

bool dmu_PumpEvents(struct dmu_samples_T* samplesPtr)
{
	bool samplesReady = false;

	// To avoid race condition, interrupts shall not share variables with application.
	// Furthermore, set pending state only if idle (NOT if busy, which will cause an error).
	if (dmu_data.samples_ready == true)
	{
		dmu_data.samples_ready = false;
		dmu_data.status = DMU_SAMPLES_READY;
	}

	if (dmu_data.samples_pending == true)
	{
		dmu_data.samples_pending = false;
		if (dmu_data.status == DMU_IDLE)
		{
			dmu_data.status = DMU_SAMPLES_PENDING;
		}
	}

	switch (dmu_data.status)
	{
		case DMU_SAMPLES_PENDING:
			dmu_GetMeasurements();
			break;

		case DMU_IDLE:
		case DMU_BUSY:
			break;

		case DMU_SAMPLES_READY:
			// No sample must be taken during data copy.
			*samplesPtr = _parse_frame();

			if(dmu_data.correctOffset)
			{
				dmu_SubstractOffset(samplesPtr);
			}

			samplesReady = true;
			dmu_data.status = DMU_IDLE;
			break;

		default:
			break;
	}

	return samplesReady;
}

void dmu_StagesInit()
{
	switch (dmu_data.stage)
	{
	case 0:

		dmu_commDataPtr->data[0] = ADD_PWR_MGMT_1;
		dmu_commDataPtr->data[1] = PWR_MGMT_1_STOP;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		break;


	case 1:

		dmu_commDataPtr->data[0] = ADD_PWR_MGMT_1;
		dmu_commDataPtr->data[1] = 0x01;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		//Putchar('1');

		break;

	case 2:
		// Note: inserting delay / putchars here screws configuration up.

		dmu_commDataPtr->data[0] = ADD_SAMPLE_RATE_DIVIDER;
		dmu_commDataPtr->data[1] = SAMPLE_RATE_DIVIDER;	// 25
		dmu_commDataPtr->data[2] = CONFIG;				// 26
		dmu_commDataPtr->data[3] = GYRO_CONFIG(GYRO_X_SELFTEST, GYRO_Y_SELFTEST, GYRO_Z_SELFTEST);		// 27
		dmu_commDataPtr->data[4] = ACCEL_CONFIG(ACCEL_X_SELFTEST, ACCEL_Y_SELFTEST, ACCEL_Z_SELFTEST);	//28
		dmu_commDataPtr->data[5] = ACCEL_CFG_2;
		dmu_commDataPtr->data[6] = ACCEL_LOW_POWER_CFG;
		dmu_commDataPtr->data[7] = MOTION_INT_THRESHOLD;
		dmu_commDataPtr->data[8] = MOTION_INT_DURATION;		// Not used in 6500
		dmu_commDataPtr->data[9] = ZERO_MOTION_THRESHOLD;	// Not used in 6500
		dmu_commDataPtr->data[10] = ZERO_MOTION_DURATION;	// Not used in 6500
		dmu_commDataPtr->data[11] = FIFO_ENABLE;

		dmu_Send (dmu_StagesInit, dmu_InitFailed, 12, NULL);

		dmu_data.stage++;

		//Putchar('2');

		break;

	case 3:

		dmu_commDataPtr->data[0] = ADD_INT_PIN_CFG;
		dmu_commDataPtr->data[1] = INT_PIN_CFG;		// 55
		dmu_commDataPtr->data[2] = INT_ENABLE;


		dmu_Send(dmu_StagesInit, dmu_InitFailed, 3, NULL);

		dmu_data.stage++;

		//Putchar('3');

		break;

	case 4:

		dmu_commDataPtr->data[0] = ADD_SIGNAL_PATH_RESET;
		dmu_commDataPtr->data[1] = RESET_SIGNAL(1,1,1);
		dmu_commDataPtr->data[2] = MOTION_DETECT_CTRL;
		dmu_commDataPtr->data[3] = USER_CTRL(DMP_DISABLE, FIFO_MASTER_DISABLE,FIFO_RESET,SIGNAL_PATH_RESET);	// Run means not reset.
		dmu_commDataPtr->data[4] = PWR_MGMT_1_RUN;
		// PWR_MGMT_2 stays in 0 (reset value).

		dmu_Send(dmu_StagesInit, dmu_InitFailed, 5, NULL);

		dmu_data.stage++;

		//Putchar('4');

		break;

	case 5:

		dmu_commDataPtr->data[0] = ADD_USER_CTRL;
		dmu_commDataPtr->data[1] = USER_CTRL_INIT;	// Run means not reset.

		dmu_Send(dmu_StagesInit, dmu_InitFailed, 2, NULL);

		dmu_data.stage++;

		//Putchar('5');

		break;


	case 6:
		// Fifo reset
		dmu_commDataPtr->data[0] = ADD_USER_CTRL;
		dmu_commDataPtr->data[1] = USER_CTRL(DMP_DISABLE, FIFO_MASTER_DISABLE, FIFO_RESET, SIGNAL_PATH_RESET);
		dmu_Send(dmu_StagesInit, dmu_InitFailed, 2, NULL);
		dmu_data.stage++;

		//Putchar('6');

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

void dmu_GetMeasurements()
{
	dmu_data.status = DMU_BUSY;
	dmu_ReceiveSamples(dmu_CommFailed, sizeof(dmu_data._frame_buffer), (uint8_t*)dmu_data._frame_buffer);
	return;
}

void dmu_SamplesReady(void)
{
	dmu_data.samples_ready = true;
	return;
}

void dmu_SamplesPending(void)
{
	uint32_t a = GPIOIntTypeGet(DMU_INT_PORT, DMU_INT_PIN_NUM);
	if (a == GPIO_RISING_EDGE)
	{
		GPIOIntTypeSet(DMU_INT_PORT, DMU_INT_PIN, GPIO_FALLING_EDGE);
		dmu_data.samples_pending = true;
	}
	else
	{
		GPIOIntTypeSet(DMU_INT_PORT, DMU_INT_PIN, GPIO_RISING_EDGE);
	}
}

void dmu_PrintRawMeasurements(struct dmu_samples_T* dmuSamples)
{
	UARTprintf("%d %d %d %d %d %d, ", dmuSamples->accel.x.v, dmuSamples->accel.y.v,
				dmuSamples->accel.z.v, dmuSamples->gyro.x.v, dmuSamples->gyro.y.v, dmuSamples->gyro.z.v);
	return;
}

void dmu_PrintFormattedMeasurements(struct dmu_samples_T* dmuSamples)
{
	UARTprintf("ax: %d, ay: %d, az: %d\ngx: %d, gy: %d, gz: %d\n", dmuSamples->accel.x.v, dmuSamples->accel.y.v,
				dmuSamples->accel.z.v, dmuSamples->gyro.x.v, dmuSamples->gyro.y.v, dmuSamples->gyro.z.v);
	return;
}

void dmu_CommFailed(void)
{
	UARTprintf("dmu comm fail\n\r");
}

void dmu_CalculateOffset(uint32_t samplesCount)
{
	if (dmu_data.init == false)
	{
		err_Throw("DMU not initialized while trying to calculate offset.");
		return;
	}

	uint32_t i = 0;
	struct dmu_samples_T samples;
	struct dmu_longSamples_T accumulator = {VEC0, VEC0};

	while(i < samplesCount)
	{
		if(dmu_PumpEvents(&samples))
		{
			dmu_AddSamples(&accumulator, &samples);
			i++;
		}
	}

	dmu_DivideLongSamples(&accumulator, samplesCount);

	dmu_offset.accel.x.v = (int16_t)accumulator.accel.x.v;
	dmu_offset.accel.y.v = (int16_t)accumulator.accel.y.v;
	dmu_offset.accel.z.v = (int16_t)accumulator.accel.z.v;

	dmu_offset.gyro.x.v = (int16_t)accumulator.gyro.x.v;
	dmu_offset.gyro.y.v = (int16_t)accumulator.gyro.y.v;
	dmu_offset.gyro.z.v = (int16_t)accumulator.gyro.z.v;
}

void dmu_AddSamples(struct dmu_longSamples_T * const acc, const struct dmu_samples_T* samples)
{
	acc->accel.x.v += samples->accel.x.v;
	acc->accel.y.v += samples->accel.y.v;
	acc->accel.z.v += samples->accel.z.v;

	acc->gyro.x.v += samples->gyro.x.v;
	acc->gyro.y.v += samples->gyro.y.v;
	acc->gyro.z.v += samples->gyro.z.v;
}

void dmu_DivideLongSamples(struct dmu_longSamples_T * const acc, int32_t divider)
{
	acc->accel.x.v /= divider;
	acc->accel.y.v /= divider;
	acc->accel.z.v /= divider;

	acc->gyro.x.v /= divider;
	acc->gyro.y.v /= divider;
	acc->gyro.z.v /= divider;
}

void dmu_EnableOffsetCorrection(bool enable)
{
	dmu_data.correctOffset = enable;
}

void dmu_SubstractOffset(struct dmu_samples_T* samples)
{
	samples->gyro.x.v -= dmu_offset.gyro.x.v;
	samples->gyro.y.v -= dmu_offset.gyro.y.v;
	samples->gyro.z.v -= dmu_offset.gyro.z.v;
}
