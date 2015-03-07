#ifndef _DMUSIMPLE_H_INCLUDED_
#define _DMUSIMPLE_H_INCLUDED_

#include <fixed_point/vector_types.h>
#include "../common.h"
#include "../xdriver/iic_interface.h"
#include "dmu_6500.h"
#include "../flyx5_hw.h"

#define MPU_LOW_BIT 0x01

#define MPU_ADDRESS (0b01101000 | (MPU_LOW_BIT))
#define DMU_MODULE_NUMBER BUS_DMU_NUM

#define DMU_INT_PORT_NUM GPIO_E
#define DMU_INT_PORT GPIO_BASE(DMU_INT_PORT_NUM)
#define DMU_INT_PIN_NUM 3
#define DMU_INT_PIN GLUE(GPIO_PIN_, DMU_INT_PIN_NUM)

typedef enum {DMU_IDLE = 0, DMU_SAMPLES_PENDING, DMU_BUSY, DMU_SAMPLES_READY} DMU_STATUS;

#define DMU_SAMPLE_BYTES 2

// Measurements taken individually must respect MPU internal registers' order
enum DMU_DATA_FRAME {
	DMU_FRAME_ACCEL_X,
	DMU_FRAME_ACCEL_Y,
	DMU_FRAME_ACCEL_Z,
	DMU_FRAME_TEMP,
	DMU_FRAME_GYRO_X,
	DMU_FRAME_GYRO_Y,
	DMU_FRAME_GYRO_Z,
	DMU_FRAME_N_ELEM
};

struct dmu_data_T
{
	bool init;

	iic_ptr userCb;
	uint8_t stage;

	volatile DMU_STATUS status;

	uint8_t _frame_buffer[DMU_FRAME_N_ELEM][DMU_SAMPLE_BYTES];
};

struct dmu_samples_T
{
	vec3_s16 accel;
	vec3_s16 gyro;
};

extern struct dmu_data_T dmu_data;

void dmu_Init(void);

bool dmu_PumpEvents(struct dmu_samples_T* samplesPtr);

void dmu_GetMeasurements(void);

void dmu_PrintRawMeasurements(struct dmu_samples_T* dmuSamples);

#endif 	// _DMUSIMPLE_H_INCLUDED_
