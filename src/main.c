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
#include "peripheral/altimeter_simple.h"
#include "Misc/error.h"
#include "debug_tools/stdio_simple.h"
#include "peripheral/dmu_6500.h"
#include "xdriver/gpio_interface.h"

#include "control/nlcf.h"

void main_samplesReady(void);

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

//#define XTAL_SPEED SYSCTL_XTAL_16MHZ	// 16 for dev board & 20 for real board
#define XTAL_SPEED SYSCTL_XTAL_20MHZ

void main_Init(void);
void ConfigureUART(void);
void main_SetupCalibration(struct nlcf_state* statePtr);

void readSuccess(void);
void readFail(void);


extern void dmu_PrintFormattedMeasurements(void);


struct {
	uint8_t readBuffer[1];
	bool samplesReady;
	uint32_t cnt;

}main_data = {{0}, false, 0};

#include "peripheral/dmu_simple.h"

void PrintMeters(int32_t meters);

void UARTputraw16(uint16_t x)
{
    UARTCharPut(UART0_BASE, x / 256);
    UARTCharPut(UART0_BASE, x & 0xFF);
}

int main_juani(void)
{
    struct dmu_samples_T dmuSamples;
    struct nlcf_state state;

    main_Init();

    dmu_Init();

    nlcf_init(&state);

    main_SetupCalibration(&state);

    _puts("init done\n\r");


    while(1)
    {

    	if(dmu_PumpEvents(&dmuSamples))
    	{
				/*
			UARTprintf("ax: %d, ay: %d, az: %d\ngx: %d, gy: %d, gz: %d\n", dmuSamples.accel.x.v, dmuSamples.accel.y.v,
							dmuSamples.accel.z.v, dmuSamples.gyro.x.v, dmuSamples.gyro.y.v, dmuSamples.gyro.z.v);
			*/
			nlcf_process(&state, dmuSamples.gyro, dmuSamples.accel, NULL);

			quat q_est = dq_to_q(state.q);

			_puts("\x0E\x0C");
			UARTputraw16(q_est.r.v);
			UARTputraw16(q_est.v.x.v);
			UARTputraw16(q_est.v.y.v);
			UARTputraw16(q_est.v.z.v);


			//UARTprintf("%d %d %d %d, ", q_est.r.v, q_est.v.x.v, q_est.v.y.v, q_est.v.z.v);
			//dmu_PrintRawMeasurements(&dmuSamples);
    	}

    }
}

void PrintMeters(int32_t meters)
{
	UARTprintf("Metros: %d\n", meters);
	altimeter_meas_ready = false;
	altimeter_CommenceMeasurement();
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
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | XTAL_SPEED |
                       SYSCTL_OSC_MAIN);

    //
    // Enable the GPIO port that is used for the on-board LED / Button.
    //
    SysCtlPeripheralEnable(SYS_PERIPH(GPIOF));
    SysCtlPeripheralEnable(SYS_PERIPH(GPIOE));

    //led_Init();
    //led_Off();

    //
    // Initialize the UART.
    //
    ConfigureUART();

    CFG_PIN(BUTTON_2);	// For calibration purposes
	R_(GPIOPadConfigSet)(PORT_OF(BUTTON_2),
		BIT(PIN_N(BUTTON_2)), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //
    // Hello!
    //
    _puts("Hugo la concha de tu madre!\n");

	iic_Init(DMU_MODULE_NUMBER);
	//iic_EnterLoopbackMode();

	err_Init(NULL, _puts, NULL);
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

void main_SetupCalibration(struct nlcf_state* statePtr)
{
	struct dmu_samples_T dmuSamples;

	if (!PIN_ACTIVE(BUTTON_2))
	{
		return;
	}

	_puts("Entered calibration mode. Release button to start calibration.\n\r");

	while(PIN_ACTIVE(BUTTON_2))
		;

	_puts("Press BTN2 to trigger measures.");

	uint8_t measurementCount = 0;

	while (measurementCount < 2)
	{
		struct cal_output calibrationOutput;
		quat calibration[2];

    	if(dmu_PumpEvents(&dmuSamples))
    	{
			nlcf_process(statePtr, dmuSamples.gyro, dmuSamples.accel, NULL);
    	}

		if(!PIN_ACTIVE(BUTTON_2))
		{
			continue;
		}
		/*
		if (userInput == 'c')
		{
			quat aux;
			asm sei;
			aux = controlData.QEst;
			asm cli;
			printf("Current quaternion: %d %d %d %d\n", Q_COMPONENTS(aux));
			continue;
		}

		if (userInput != 'm')
			continue;
*/
		if (measurementCount == 0)
		{
			calibration[0] = dq_to_q(statePtr->q);			// state is not written during interrupts, no need of "cli"
			_puts("First measurement done\n\r");
		}

		else if (measurementCount == 1)
		{
			calibration[1] = dq_to_q(statePtr->q);
			_puts("Second measurement done\n\r");
		}

		measurementCount++;

		if (measurementCount == 2)
		{
			calibrationOutput = att_calibrate(calibration[0], calibration[1]);
			UARTprintf("Cal output: %d\n\r", calibrationOutput.quality);
			UARTprintf("Correction: %d %d %d %d\n\r", calibrationOutput.correction.r.v, calibrationOutput.correction.v.x.v,
					calibrationOutput.correction.v.y.v, calibrationOutput.correction.v.z.v);

			if (calibrationOutput.quality == CAL_BAD)
			{
				measurementCount = 1;	// Stay looping second measurement.
				_puts("Calibrate again\n\r");
			}

			nlcf_apply_correction(statePtr, calibrationOutput);
		}

		SysCtlDelay(SysCtlClockGet() / 3 / 3);		// Delay for relasing button.
	}

	_puts("Press BTN2 to continue.");

	while(!PIN_ACTIVE(BUTTON_2))
		;

	return;
}

void readSuccess(void)
{
	UARTprintf("Read Success. Value: %x\n\r", (int)main_data.readBuffer[0]);
}

void readFail(void)
{
	UARTprintf("Read Fail.\n\r");
}

