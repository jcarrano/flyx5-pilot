#include "iic_interface.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "../Misc/error.h"
#include "../macro_magic.h"

#include "../debug_tools/led.h"	// Debug purposes
#include "../utils/uartstdio.h"
#include "../debug_tools/debug.h"

/*
 * Driver for single module;
 * TO DO: make compatible for many modules
 */

#define INT_IIC_MODULE GLUE(INT_, IIC_MODULE)
#define IIC_GPIO GLUE(SYSCTL_PERIPH_, IIC_GPIO_NUMBER)
#define IIC_PERIPHERAL GLUE(SYSCTL_PERIPH_, IIC_MODULE)
#define IIC_GPIO_PORT GLUE3(GPIO_, IIC_PORT, _BASE)
#define IIC_GPIO_SDA_PIN GLUE(GPIO_PIN_, IIC_SDA_PIN)
#define IIC_GPIO_SCL_PIN GLUE(GPIO_PIN_, IIC_SCL_PIN)

#define I2C_RW_BIT(x) (HWREG(x + I2C_O_MSA) & 0x01)
#define I2C_READ_OPERATION 1
#define I2C_WRITE_OPERATION 0

typedef struct {
    iic_ptr currCB;
    iic_userAction eotCB;
    iic_userAction commFailedCB;
    uint8_t dataIdx;
    bool init;
    bool stoppingBus;
    uint32_t activeModule;

}iic_data_T;

iic_data_T iic_data = {NULL,NULL,NULL,0,false,false, I2C0_BASE};

iic_commData_T iic_commData;

// Internal Symbols
void iic_Write(uint8_t moduleNumber);
void iic_Read (uint8_t moduleNumber);

void iic_FullStagesReceive (uint8_t moduleBase);


// Internal Symbols for individual modules

void iic0_InterruptHandler(void);
void iic1_InterruptHandler(void);
void iic2_InterruptHandler(void);
void iic3_InterruptHandler(void);

void iic0_FullStagesReceive(void);
void iic1_FullStagesReceive(void);
void iic2_FullStagesReceive(void);
void iic3_FullStagesReceive(void);


// Indexers for modules

const uint32_t IIC_MODULE_BASES[] = {I2C0_BASE, I2C1_BASE, I2C2_BASE, I2C3_BASE};
#define IIC_MODULE_BASE(x) (IIC_MODULE_BASES[x])

const iic_userAction IIC_STAGES_RECEIVE_PTRS[] = {iic0_FullStagesReceive, iic1_FullStagesReceive, iic2_FullStagesReceive, iic3_FullStagesReceive};
#define IIC_STAGES_RECEIVE_PTR(x) (IIC_STAGES_RECEIVE_PTRS[x])



// TODO: use tables to make it multi-module usable
void iic_Init(uint8_t moduleNumber)
{
    // Enable the GPIO Peripheral used by the I2C.
	SysCtlPeripheralReset(IIC_GPIO);
    SysCtlPeripheralEnable(IIC_GPIO);

    // Enable I2C0
    SysCtlPeripheralReset(IIC_PERIPHERAL);	// see TODO
    SysCtlPeripheralEnable(IIC_PERIPHERAL);

    // Configure GPIO Pins for I2C mode. ; see TODO
    GPIOPinConfigure(IIC_SDA);
    GPIOPinTypeI2C(IIC_GPIO_PORT, IIC_GPIO_SDA_PIN);
    GPIOPinConfigure(IIC_SCL);
    GPIOPinTypeI2CSCL(IIC_GPIO_PORT, IIC_GPIO_SCL_PIN);


    //Start Module
	I2CMasterInitExpClk(IIC_MODULE_BASE(moduleNumber), SysCtlClockGet(), true);

	IntEnable(INT_IIC_MODULE);	// See TODO

	I2CMasterIntEnableEx(IIC_MODULE_BASE(moduleNumber), I2C_MASTER_INT_DATA);
	//I2CMasterIntEnable(IIC_MODULE_BASE);

	IntMasterEnable();
}

void iic0_InterruptHandler(void)
{
	I2CMasterIntClear(IIC_SINGLE_MODULE_BASE);		// Clear interrupt source early as stated in Driver Library User Manual
	iic_data.activeModule = IIC_SINGLE_MODULE_BASE;	// Warning: this field shall not be overwritten by other I2C transfer
										// until iic_Write is finished (currCB in the middle of a transfer)

	Putchar('z');

	// End of Transmission detection
	if (iic_data.currCB == NULL)
	{

	#ifdef IIC_DEBUG_EOT
		Putchar('e');Putchar('o');Putchar('t');Putchar('\n');Putchar('\r');
	#endif
		iic_data.stoppingBus = true;
		if(I2C_RW_BIT(IIC_SINGLE_MODULE_BASE) == I2C_READ_OPERATION)
		{
			iic_commData.dataPtr[iic_data.dataIdx] = (uint8_t)I2CMasterDataGet(IIC_SINGLE_MODULE_BASE);		// Fetch last byte
		}
	}

	// Error detection
	if (I2CMasterErr(IIC_SINGLE_MODULE_BASE) != I2C_MASTER_ERR_NONE)
	{
		IntDisable(INT_IIC_MODULE);

		iic_data.eotCB = iic_data.commFailedCB;
		iic_data.currCB = NULL;
	}

	if (iic_data.currCB != NULL)
	{
		iic_data.currCB(0);
	}
	else
	{
		iic_data.eotCB();
	}

	while(I2CMasterIntStatus(IIC_SINGLE_MODULE_BASE, false) == true)	// IntClear is not immediate, make sure int flag is clear.
		;

	return;
}

void iic_Send(uint8_t moduleNumber, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toSend, uint8_t* sendBuffer)
{

	if (I2CMasterBusy(IIC_MODULE_BASE(moduleNumber)))
	{
		if (iic_data.stoppingBus)	// If the bus is busy but not because it is being stopped, it's a logical error.
	    	while (I2CMasterBusy(IIC_MODULE_BASE(moduleNumber))); 						// Wait until bus stop is complete
	    else
	    	err_Throw("iic: attempt to send message while bus is busy.\n");
	}

	iic_data.stoppingBus = false;
    iic_data.eotCB = eotCB;
    iic_data.commFailedCB = commFailedCB;
    iic_data.dataIdx = 0;

    iic_commData.dataSize = toSend - 1;	// 	dataSize received as parameter for compatibility with receive.
        								//	As before, toSend is the size (to send 1 byte, toSend has to be 1), so the (-1) is for compatibility.
 	if (sendBuffer != NULL)
	 	iic_commData.dataPtr = sendBuffer;
    else
    	iic_commData.dataPtr = iic_commData.data;

    I2CMasterSlaveAddrSet(IIC_MODULE_BASE(moduleNumber), slaveAddress, false);	// False = write

    I2CMasterDataPut(IIC_MODULE_BASE(moduleNumber), iic_commData.dataPtr[iic_data.dataIdx++]);	// dataIdx always points to the data to send

    if (toSend == 1)
    {
    	I2CMasterControl(IIC_MODULE_BASE(moduleNumber), I2C_MASTER_CMD_SINGLE_SEND);
        iic_data.currCB = NULL;
    }
    else
    {
    	I2CMasterControl(IIC_MODULE_BASE(moduleNumber), I2C_MASTER_CMD_BURST_SEND_START);
        iic_data.currCB = iic_Write;
    }
}

void iic_Write(uint8_t moduleNumber)
{
	I2CMasterDataPut(IIC_MODULE_BASE(moduleNumber), iic_commData.dataPtr[iic_data.dataIdx]);	// dataIdx always points to the data to send

    if (iic_data.dataIdx++ == iic_commData.dataSize)	// e.g: toWrite = 2, dataSize = 1 --> first comp = true --> send second and finish
    {
        //iic_data.currCB = iic_data.eotCB;
        I2CMasterControl(iic_data.activeModule, I2C_MASTER_CMD_BURST_SEND_FINISH);//I2C_MASTER_CMD_BURST_SEND_FINISH);
        iic_data.currCB = NULL;
    }
    else
    {
        I2CMasterControl(iic_data.activeModule, I2C_MASTER_CMD_BURST_SEND_CONT);
    }
    return;
}

void iic_Receive (uint8_t moduleNumber, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toRead, uint8_t* receiveBuffer)
{

	if (I2CMasterBusy(IIC_MODULE_BASE(moduleNumber)))
	{
		if (iic_data.stoppingBus)	// If the bus is busy but not because it is being stopped, it's a logical error.
	    	while (I2CMasterBusy(IIC_MODULE_BASE(moduleNumber))); 						// Wait until bus stop is complete
	    else
	    	err_Throw("iic: attempt to receive message while bus is busy.\n");
	}

	iic_data.stoppingBus = false;
    iic_data.eotCB = eotCB;
    iic_data.commFailedCB = commFailedCB;
//    iic_data.currCB = iic_read_start;
    iic_data.dataIdx = 0;

    iic_commData.dataSize = toRead-1;		// toRead es lo que quiero leer, el -1 es necesario para eso.

 	if (receiveBuffer != NULL)
	 	iic_commData.dataPtr = receiveBuffer;
    else
    	iic_commData.dataPtr = iic_commData.data;

    I2CMasterSlaveAddrSet(IIC_MODULE_BASE(moduleNumber), slaveAddress, true);	// False = read

    if (toRead == 1)
    {
    	I2CMasterControl(IIC_MODULE_BASE(moduleNumber), I2C_MASTER_CMD_SINGLE_RECEIVE);
        iic_data.currCB = NULL;
    }
    else
    {
    	I2CMasterControl(IIC_MODULE_BASE(moduleNumber), I2C_MASTER_CMD_BURST_RECEIVE_START);
        iic_data.currCB = iic_Read;
    }

 	/*
    IIC_SET_AS_TX();

    IIC_START();
    IIC_SEND((slvAddress << 1) + READ);		//precedence
    */
}

#define receive_dataCopy(receiveData, _regAddress, _slaveAddress, _eotCB, _commFailedCB, _toRead, _receiveBuffer, _stage) do \
{	receiveData.regAddress = _regAddress;		\
	receiveData.slaveAddress = _slaveAddress;	\
	receiveData.eotCB = _eotCB; 				\
	receiveData.commFailedCB = _commFailedCB; 	\
	receiveData.toRead = _toRead; 				\
	receiveData.receiveBuffer = _receiveBuffer; \
	receiveData.stage = _stage; } while(0)


void iic_ReceiveFromRegister (uint8_t moduleNumber, uint8_t regAddress, uint8_t slaveAddress, iic_userAction eotCB, iic_userAction commFailedCB, uint8_t toRead, uint8_t* receiveBuffer)
{
	if (I2CMasterBusy(IIC_MODULE_BASE(moduleNumber)))
	{
		if (iic_data.stoppingBus)	// If the bus is busy but not because it is being stopped, it's a logical error.
	    	while (I2CMasterBusy(IIC_MODULE_BASE(moduleNumber))); 	// Wait until bus stop is complete; stoppingBus is cleared in iic_Send.
	    else
	    	err_Throw("iic: attempt to receive message (from register) while bus is busy.\n");
	}

	receive_dataCopy(iic_commData.transferParameters, regAddress, slaveAddress, eotCB, commFailedCB, toRead, receiveBuffer, 0);

	iic_FullStagesReceive(moduleNumber);
}


void iic0_FullStagesReceive(void)
{
	iic_FullStagesReceive(0);
}

void iic1_FullStagesReceive(void)
{
	iic_FullStagesReceive(1);
}

void iic2_FullStagesReceive(void)
{
	iic_FullStagesReceive(2);
}

void iic3_FullStagesReceive(void)
{
	iic_FullStagesReceive(3);
}

void iic_FullStagesReceive (uint8_t moduleNumber)
{
	iic_receiveData_T* rData = &iic_commData.transferParameters;
	switch (rData->stage)
	{
	case 0:		// Prepare to read: send read address to slave device.

		iic_Send(moduleNumber, rData->slaveAddress, IIC_STAGES_RECEIVE_PTR(moduleNumber), rData->commFailedCB, 1, &(rData->regAddress));	// Write start read address to slave device
		rData->stage++;

		break;

	case 1:		// Start reception itself. Data needed to call iic_Receive can be overwritten in global array if receiveBuffer is NULL, but its OK.
		iic_Receive(moduleNumber, rData->slaveAddress, rData->eotCB, rData->commFailedCB,
											rData->toRead, rData->receiveBuffer);
		break;

	default:
		break;

	}

	return;
}
/*
void iic_read_start (void)
{
	if (iic_commData.dataSize == 0)		// Acá pregunto por 0, a read le mando 1 porque quiero leer 1...
	{
		IIC_NOT_ACKNOWLEDGE_DATA();
		iic_data.currCB = iic_data.eotCB;
	}
	else
	{
		IIC_ACKNOWLEDGE_DATA();
		iic_data.currCB = iic_read;
	}

	IIC_SET_AS_RX();
	iic_commData.data[0] = IIC_RECEIVE(); //Dummy read - faster if the index is fixed in compile time.
}
*/

void iic_Read (uint8_t moduleNumber)
{
	iic_commData.dataPtr[iic_data.dataIdx] = (uint8_t)I2CMasterDataGet(IIC_MODULE_BASE(moduleNumber));

	if (iic_data.dataIdx++ == (iic_commData.dataSize-1))	// receive last byte
	{
		//IIC_NOT_ACKNOWLEDGE_DATA();
		I2CMasterControl(IIC_MODULE_BASE(moduleNumber), I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        //iic_data.currCB = iic_data.eotCB;
		iic_data.currCB = NULL;
    }
	else
	{
		I2CMasterControl(IIC_MODULE_BASE(moduleNumber), I2C_MASTER_CMD_BURST_RECEIVE_CONT);
	}

    //iic_commData.dataPtr[iic_data.dataIdx] = IIC_RECEIVE();

    return;
}



/*
 * For debugging purposes; slave must be connected for interrupt handler to be called.
 *
 */
void iic_EnterLoopbackMode(void)
{
	HWREG(IIC_SINGLE_MODULE_BASE + I2C_O_MCR) |= 0x01;
}
