//*****************************************************************************
//
// hello.c - Simple hello world example.
//
// Copyright (c) 2012-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/i2c.h"

#include "xdriver/iic_interface.h"
#include "debug_tools/led.h"
#include "peripheral/dmu_simple.h"
#include "Misc/error.h"
#include "debug_tools/stdio_simple.h"
#include "peripheral/dmu_definitions.h"
#include "xdriver/gpio_interface.h"


void main_samplesReady(void);

uint32_t TypeGet(uint32_t ui32Port, uint8_t ui8Pin);
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Hello World (hello)</h1>
//!
//! A very simple ``hello world'' example.  It simply displays ``Hello World!''
//! on the UART and is a starting point for more complicated applications.
//!
//! UART0, connected to the Virtual Serial Port and running at
//! 115,200, 8-N-1, is used to display messages from this application.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

#define DMU_INT_PIN GPIO_PIN_2

void main_Init(void);
void ConfigureUART(void);


void readSuccess(void);
void readFail(void);


extern void dmu_PrintFormattedMeasurements(void);
extern void dmu_GetMeasurements(iic_userAction cb);

struct {
	uint8_t readBuffer[1];
	bool samplesReady;
	uint32_t cnt;

}main_data = {{0}, false, 0};


int main(void)
{
    //volatile uint32_t ui32Loop;

	main_Init();

	dmu_Init();
	gpio_Init(GPIO_A, DMU_INT_PIN, GPIO_RISING_EDGE);

	UARTprintf("init done\n\r");
	rled_On();

	//dmu_ReceiveFromRegister(ADD_WHO_AM_I, readSuccess, readFail, 1, readBuffer);

    while(1)
    {

    	if (main_data.samplesReady)
    	{
    		if (main_data.cnt++ > 5)
    		{
    	    	dmu_GetMeasurements(dmu_PrintFormattedMeasurements);
    	    	main_data.cnt = 0;
    		}
    		main_data.samplesReady = false;

    	}
    	/*SysCtlDelay(SysCtlClockGet() / 10 / 3);

    	dmu_GetMeasurements(dmu_PrintFormattedMeasurements);
    	*/
    }
}

void main_Init()
{
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    led_Init();
    led_Off();

    //
    // Initialize the UART.
    //
    ConfigureUART();

    //
    // Hello!
    //
    UARTprintf("Hello, Juan!\n");

	iic_Init(0);
	iic_EnterLoopbackMode();

	err_Init(NULL, puts, NULL);
}



//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

void readSuccess(void)
{
	UARTprintf("Read Success. Value: %x\n\r", (int)main_data.readBuffer[0]);
}

void readFail(void)
{
	UARTprintf("Read Fail.\n\r");
}


void main_samplesReady(void)
{
	uint32_t a = GPIOIntTypeGet(GPIO_PORTA_BASE, 2);
	if (a == GPIO_RISING_EDGE)
	{
		GPIOIntTypeSet(GPIO_PORTA_BASE, DMU_INT_PIN, GPIO_FALLING_EDGE);
		main_data.samplesReady = true;
	}
	else
	{
		GPIOIntTypeSet(GPIO_PORTA_BASE, DMU_INT_PIN, GPIO_RISING_EDGE);
	}
}
