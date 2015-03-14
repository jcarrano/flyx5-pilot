#ifndef _DMUSIMPLE_H_INCLUDED_
#define _DMUSIMPLE_H_INCLUDED_

#include <fixed_point/vector_types.h>
#include "../common.h"
#include "../xdriver/iic_interface.h"
#include "dmu_6500.h"
#include "../flyx5_hw.h"

#define DMU_OFFSET_SAMPLES 1000

#define DMU_LOW_BIT 0x01

#define DMU_ADDRESS (0b01101000 | (DMU_LOW_BIT))
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

//!  Main dmu data struct. Contains all the data needed for the dmu to work.
struct dmu_data_T
{
	bool init;
	bool correctOffset;

	iic_ptr userCb;
	uint8_t stage;

	volatile DMU_STATUS status;
	volatile bool samples_ready;
	volatile bool samples_pending;

	uint8_t _frame_buffer[DMU_FRAME_N_ELEM][DMU_SAMPLE_BYTES];
};

//! Data type to store processed samples from dmu.
struct dmu_samples_T
{
	vec3_s16 accel;
	vec3_s16 gyro;
};

extern struct dmu_data_T dmu_data;
extern struct dmu_samples_T dmu_offset;

//! Init Digital Motion Unit.
//! Default configuration set in dmu_6500.h:
//! * 1 ms sample time
//! * Data ready interrupt enabled - pulse output
//! * Maximum scale for Gyro and Accel

void dmu_Init(void);

//! Trigger dmu events by polling in user application.
//! This function starts communication with DMU if samples are available in sensor, or stores a copy of received samples
//! in samplesPtr. The function does nothing if there are no samples available in sensor/ memory, or if a communication is going on.
//! @param samplesPtr pointer to struct to copy samples if available.
//! @return Returns \b true if there were samples just received from sensor.

bool dmu_PumpEvents(struct dmu_samples_T* samplesPtr);

//! Print measurements as text, each one spaced and the whole set terminated by a comma.
//! @param samplesPtr pointer to the samples to print.
//! @note samples shall not be edited during interrupts to use this function safely.

void dmu_PrintRawMeasurements(struct dmu_samples_T* dmuSamples);

//! Enables or disables offset correction in dmu_PumpEvents.
//! @param enable Set to \b true to enable offset correction.
//! @note dmu_CalculateOffset shall be called before applying correction; offset is set to 0 by default.

void dmu_EnableOffsetCorrection(bool enable);

//! Averages n samples to calculate gyro offset, and stores it into internal global variable.
//! @param samplesCount number of samples to average.
//! @note dmu has to be initialized before calculating offset.

void dmu_CalculateOffset(uint32_t samplesCount);

#endif 	// _DMUSIMPLE_H_INCLUDED_
