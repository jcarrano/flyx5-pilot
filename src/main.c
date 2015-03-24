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
#include "driverlib/interrupt.h"

#include "xdriver/iic_interface.h"
#include "debug_tools/led.h"
#include "peripheral/dmu_simple.h"
#include "peripheral/altimeter_simple.h"
#include "peripheral/joystick.h"
#include "Misc/error.h"
#include "debug_tools/stdio_simple.h"
#include "peripheral/dmu_6500.h"
#include "xdriver/gpio_interface.h"
#include "peripheral/esc.h"
#include "control/nlcf.h"

#define MAIN_PUMP_SAMLES

#define _MAIN_ESC_PROGRAM_
//#define _MAIN_JOYSTICK_SETPOINT_PRINT_
//#define _MAIN_OFFSET_CORRECTION_PRINT_
//#define _MAIN_STATE_PRINT_


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
extern void init_failsafe();

extern void dmu_PrintFormattedMeasurements(void);
extern struct cal_output cal_correction;

struct {
	uint8_t readBuffer[1];
	bool samplesReady;
	uint32_t cnt;

}main_data = {{0}, false, 0};

#include "peripheral/dmu_simple.h"

void PrintMeters(int32_t meters);

static bool joystick_check_arm(joy_data_t joy)
{
    const int32_t low_thres = (INT16_MIN / 20) * 19;
    const uint32_t low_thres_u = (UINT16_MAX / 20);

    return joy.roll < low_thres && joy.pitch < low_thres &&
            joy.yaw > -low_thres && joy.elev < low_thres_u;
}

int main_(void)
{
    struct nlcf_state state;

    init_failsafe();

    main_Init();

    dmu_Init();

    nlcf_init(&state);
    nlcf_apply_correction(&state, cal_correction);

    _puts("init done\n\r");

    _puts("starting offset calculation...\n\r");

    dmu_CalculateOffset(DMU_OFFSET_SAMPLES);

    _puts("Done\n\r");

	UARTprintf("ax: %d, ay: %d, az: %d\ngx: %d, gy: %d, gz: %d\n", dmu_offset.accel.x.v, dmu_offset.accel.y.v,
			dmu_offset.accel.z.v, dmu_offset.gyro.x.v, dmu_offset.gyro.y.v, dmu_offset.gyro.z.v);

	//UARTCharGet(BASE_PERIPH(UART_DEBUG));
	bool offsetCorrection = true;
	dmu_EnableOffsetCorrection(offsetCorrection);

	int i = 0;

#ifdef 	_MAIN_JOYSTICK_SETPOINT_PRINT_
	multirotor_setpoint setpoint;
	joy_data_t joyData;
	joy_Init();
#endif

#ifdef _MAIN_ESC_PROGRAM_
	joy_Init();
	esc_Init();
#endif

    while(1)
    {
#ifdef MAIN_PUMP_SAMLES
        struct dmu_samples_T dmuSamples;
    	if(dmu_PumpEvents(&dmuSamples))
    	{
#ifdef _MAIN_ESC_PROGRAM_
    	esc_SetThrust(_frac(joy_data.elev), _frac(joy_data.elev), _frac(joy_data.elev), _frac(joy_data.elev));
    	UARTprintf("%d\n\r", (int32_t) _frac(joy_data.elev).v);
#endif
#ifdef _MAIN_OFFSET_CORRECTION_PRINT_
    		if(i++ > SAMPLE_RATE)
    		{
    			UARTprintf("ax: %d, ay: %d, az: %d\ngx: %d, gy: %d, gz: %d\n", dmuSamples.accel.x.v, dmuSamples.accel.y.v,
							dmuSamples.accel.z.v, dmuSamples.gyro.x.v, dmuSamples.gyro.y.v, dmuSamples.gyro.z.v);
    			i = 0;
    		}

    		if(UARTCharGetNonBlocking(BASE_PERIPH(UART_DEBUG)) == 't')
    		{
    			offsetCorrection = !offsetCorrection;
    			dmu_EnableOffsetCorrection(offsetCorrection);
    		}
#endif

#ifdef _MAIN_STATE_PRINT_
    		nlcf_process(&state, dmuSamples.gyro, dmuSamples.accel, NULL);

			quat q_est = dq_to_q(state.q);

			_puts("\x0E\x0C");
			UARTputraw16(q_est.r.v);
			UARTputraw16(q_est.v.x.v);
			UARTputraw16(q_est.v.y.v);
			UARTputraw16(q_est.v.z.v);
#endif

#ifdef _MAIN_JOYSTICK_SETPOINT_PRINT_


			IntMasterDisable();
			joyData = joy_data;
			IntMasterEnable();

			setpoint = joystick_to_setpoint(joyData);

			_puts("\x0E\x0C");
			UARTputraw16(setpoint.attitude.r.v);
			UARTputraw16(setpoint.attitude.v.x.v);
			UARTputraw16(setpoint.attitude.v.y.v);
			UARTputraw16(setpoint.attitude.v.z.v);

/*
			if (i++ > 50)
			{
				//UARTprintf("e: %d p: %d r: %d y: %d \n\r", joyData.elev, joyData.pitch, joyData.roll, joyData.yaw);
				//UARTprintf("%d", (int)joystick_check_arm(joyData));
				UARTprintf("r: %d p: %d y: %d e: %d\n\r", joyData.roll, joyData.pitch, joyData.yaw, joyData.elev);
				i = 0;
			}
*/
#endif
			//UARTprintf("%d %d %d %d, ", q_est.r.v, q_est.v.x.v, q_est.v.y.v, q_est.v.z.v);
			//dmu_PrintRawMeasurements(&dmuSamples);
    	}
#endif // Pump samples

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
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | XTAL_SPEED |
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

void readSuccess(void)
{
	UARTprintf("Read Success. Value: %x\n\r", (int)main_data.readBuffer[0]);
}

void readFail(void)
{
	UARTprintf("Read Fail.\n\r");
}
