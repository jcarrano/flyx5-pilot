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

#define IIC_GPIO_NUMBER GLUE(GPIO, IIC_PORT_ID)
#define IIC_PORT GLUE(PORT, IIC_PORT_ID)


//! Init target I2C module.
//! SCL & SDA pins are configured as part of initialization.
//! @param moduleNumber iic module defined in enum (IIC_MODULE_X)
void iic_Init(uint8_t moduleNumber);


//! Send message to target device, and execute action afterwards.
//!
//! @param moduleNumber: module number to be used [0 - 3].
//! @param slaveAddress: 7-bit right-aligned direction to send data.
//! @param eotCB: function to call after transmission is completed correctly.
//! @param commFailedCB: Error handler function.
//! @param toSend: quantity of bytes to send.
//! @param sendBuffer: pointer to memory block to be sent.
//
//
void iic_Send(uint8_t moduleNumber, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toSend, uint8_t* sendBuffer);


//! Receive message from target device's register(s), and execute action afterwards.
//!
//! @param moduleNumber: module number to be used [0 - 3].
//!	@param regAddress: register address to start reception.
//! @param slaveAddress: 7-bit right-aligned direction to send data.
//! @param eotCB: function to call after transmission is completed correctly.
//! @param commFailedCB: Error handler function.
//! @param toRead: quantity of bytes to read.
//! @param sendBuffer: pointer to memory block to write the received data.
//
//!	@note \b sendBuffer can be \b NULL; in that case the data received will be stored in iic_commData.data buffer.
//
void iic_ReceiveFromRegister (uint8_t moduleNumber, uint8_t regAddress, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toRead, uint8_t* receiveBuffer);

//! Receive message from target device, and execute action afterwards. The device shall point to target register before this
//! function is called.
//!
//! @param moduleNumber: module number to be used [0 - 3].
//! @param slaveAddress: 7-bit right-aligned direction to send data.
//! @param eotCB: function to call after transmission is completed correctly.
//! @param commFailedCB: Error handler function.
//! @param toRead: quantity of bytes to read.
//! @param sendBuffer: pointer to memory block to write the received data.
//
//!	@note \b sendBuffer can be \b NULL; in that case the data received will be stored in iic_commData.data buffer.

void iic_Receive (uint8_t moduleNumber, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toRead, uint8_t* receiveBuffer);

/*
 * For debugging purposes; slave must be connected for interrupt handler to be called.
 */
void iic_EnterLoopbackMode(void);

#endif /* I2C_INTERFACE_H_ */
