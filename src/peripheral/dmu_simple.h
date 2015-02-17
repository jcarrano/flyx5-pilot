#ifndef _DMUSIMPLE_H_INCLUDED_
#define _DMUSIMPLE_H_INCLUDED_

#include "../common.h"
#include "../xdriver/iic_interface.h"
#include "dmu_6500.h"
#include "../Misc/arith.h"
#include "../flyx5_hw.h"

#define MPU_LOW_BIT 0x01

#define MPU_ADDRESS (0b01101000 | (MPU_LOW_BIT))
#define DMU_MODULE_NUMBER BUS_DMU_NUM

#define DMU_INT_PORT_NUM GPIO_E
#define DMU_INT_PORT GPIO_BASE(DMU_INT_PORT_NUM)
#define DMU_INT_PIN_NUM 3
#define DMU_INT_PIN GLUE(GPIO_PIN_, DMU_INT_PIN_NUM)

struct dmu_data_T
{
	bool init;

	iic_ptr userCb;
	uint8_t stage;

	struct
	{
		bool enable;
		int8_t fetchTimes;
		uint8_t remainingBytes;
		uint16_t count;
		uint8_t avgDiscard;
		iic_ptr stageCb;
	}fifo;
};

struct dmu_measurements_T
{
	s16Vec3 accel;

	int16_t temp;

	s16Vec3 gyro;
}__attribute__((__packed__));

extern struct dmu_data_T dmu_data;

void dmu_Init(void);

extern void dmu_InitFailed(void);

#define dmu_Send(eotCB, commFailedCB, toWrite, sendBuffer)	\
	(iic_Send (DMU_MODULE_NUMBER, MPU_ADDRESS, eotCB, commFailedCB, toWrite, sendBuffer) )

#define dmu_ReceiveFromRegister(regAddress, eotCB, commFailedCB, toRead, receiveBuffer)			\
	(iic_ReceiveFromRegister (DMU_MODULE_NUMBER, regAddress, MPU_ADDRESS, eotCB, 				\
			commFailedCB, toRead, receiveBuffer) )

#endif 	// _DMUSIMPLE_H_INCLUDED_
