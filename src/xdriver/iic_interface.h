/*
 * I2c_Interface.h
 *
 *  Created on: 26/12/2014
 *      Author: Juan Ignacio
 */

#ifndef I2C_INTERFACE_H_
#define I2C_INTERFACE_H_

#include "../common.h"
#include "../macro_magic.h"
#include "inc/hw_memmap.h"

/*
 * Registers/ pins used:
 * GPIO: SYSCTL_PERIPH_I2C_GPIOB
 * I2C Peripheral: SYSCTL_PERIPH_I2C0
 * SDA: GPIO_PB3_I2C0SDA	PORTB Pin 3
 * SCL: GPIO_PB2_I2C0SCL 	PORTB Pin 2
 */

#define IIC_MODULE I2C0
#define IIC_SINGLE_MODULE_BASE GLUE(IIC_MODULE, _BASE)

enum{
	IIC_MODULE_0,
	IIC_MODULE_1,
	IIC_MODULE_2,
	IIC_MODULE_3
};

#define I2C_MODULE_QTY 2	// I2C0 & 1 available

#define IIC_MEM_SIZE (50)

typedef void (*iic_ptr)(uint8_t moduleNumber);
typedef void (*iic_userAction)(void);

typedef struct
{
	uint8_t regAddress;
	uint8_t slaveAddress;
	iic_userAction eotCB;
	iic_userAction commFailedCB;
	uint8_t toRead;
	uint8_t* receiveBuffer;
	uint8_t stage;

}iic_receiveData_T;

typedef struct {
    uint8_t data[IIC_MEM_SIZE];
    uint8_t dataSize;
    uint8_t* dataPtr;

    iic_receiveData_T transferParameters;

} iic_commData_T;


extern iic_commData_T iic_commData[I2C_MODULE_QTY];

#if (IIC_MODULE == I2C0)

#define IIC_PORT_ID B

#define IIC_SDA GPIO_PB3_I2C0SDA
#define IIC_SDA_PIN 3

#define IIC_SCL GPIO_PB2_I2C0SCL
#define IIC_SCL_PIN 2

#endif

#define IIC_GPIO_NUMBER GLUE(GPIO, IIC_PORT_ID)
#define IIC_PORT GLUE(PORT, IIC_PORT_ID)




void iic_Init(uint8_t moduleNumber);

/*
 * @param module: module number to be used [0 - 3].
 * @param slaveAddress: 7-bit right-aligned direction to send data.
 * @param eotCB: function to call after transmission is completed.
 * @param commFailedCB: Error handler function.
 * @param toSend: quantity of bytes to send.
 * @param sendBuffer: pointer to memory block to be sent.
 */

void iic_Send(uint8_t moduleNumber, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toSend, uint8_t* sendBuffer);

void iic_ReceiveFromRegister (uint8_t moduleNumber, uint8_t regAddress, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toRead, uint8_t* receiveBuffer);

void iic_Receive (uint8_t moduleNumber, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toRead, uint8_t* receiveBuffer);

void iic_EnterLoopbackMode(void);

#endif /* I2C_INTERFACE_H_ */
