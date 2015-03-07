#include "usound.h"
#include <stddef.h>

#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"

#include "../xdriver/rti.h"

#define USOUND_TIMER 		TIMER1_BASE

#define TRIGGER_TIMER 		TIMER_A
#define TRIGGER_TIMER_CFG 	TIMER_CFG_A_ONE_SHOT
#define TRIGGER_TIMER_INT 	TIMER_TIMA_TIMEOUT

#define ECHO_TIMER 			TIMER_B
#define ECHO_TIMER_CFG 		TIMER_CFG_B_CAP_TIME_UP
#define ECHO_TIMER_INT 		TIMER_CAPB_EVENT

#define USOUND_GPIO 		GPIO_PORTF_BASE

#define TRIGGER_PIN 		GPIO_PIN_2
#define ECHO_PIN 			GPIO_PIN_3

#define USOUND_STARTUP_DELAY_MS 200 // Delay to allow the module to properly initialize

#define USOUND_PULSE_TIME_US 10
#define USOUND_SAMPLE_PERIOD_MS 30
#define USOUND_COOLDOWN_MS 3
#define USOUND_TIMEOUT_MS 300

#define USOUND_OUT_OF_RANGE_MS 50
#define USOUND_US_TO_CM(us) ((us)/58)
#define USOUND_CONVERSION(us) (((us) < (USOUND_OUT_OF_RANGE_MS * 1000)) ? USOUND_US_TO_CM(us) : USOUND_INVALID_MEAS)

typedef enum
{
	INITIALIZING,
	READY,
	TRIGGERING,
	WAITING_FOR_ECHO,
	WAITING_FOR_ECHO_TO_END,
	COOLING_DOWN
} USOUND_STAGE;

volatile struct
{
	USOUND_STAGE stage;
	uint32_t measuredValue;
	bool halfReady;
} usound_data;

volatile bool usound_isInit = false;

volatile bool usound_meas_available;
volatile int32_t usound_meas;

void usound_Measure(void);
void usound_InitCallback (void *data, rti_time period, rti_id id);
void usound_SolveTiming (void *data, rti_time period, rti_id id);
void usound_Timeout (void *data, rti_time period, rti_id id);

void usound_Init (void)
{
	if (usound_isInit == true)
		return;

	rti_Init();

	usound_data.stage = INITIALIZING;

	// Two timers are used:
	// GPIO_PF2 & PF3 are mapped to Timer 1 (T1CCP0 & T1CCP1), both are used
	// Both GPIO_PF2 and PF3 are also used

	// GPIOF enable
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// Map GPIO_PF3 (usound Echo) to Timer 1 B - the Trigger GPIO is used manually, so its initialized as a plain output (default behaviour)
	GPIOPinConfigure(GPIO_PF3_T1CCP1);

	// Trigger initialized as output, Echo initialized as timer
	GPIOPinTypeGPIOOutput(USOUND_GPIO, TRIGGER_PIN);
	GPIOPinTypeTimer(USOUND_GPIO, ECHO_PIN);

	// Enable Timer 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

	// Configure Timer A (PF2, trigger) to be an output compare, and Timer B (PF3, echo) to be an input capture that measures time
	TimerConfigure(USOUND_TIMER, TIMER_CFG_SPLIT_PAIR | TRIGGER_TIMER_CFG | ECHO_TIMER_CFG);

	// Enable the echo timer (though it won't interrupt yet)
	TimerPrescaleSet(USOUND_TIMER, ECHO_TIMER, 0xFF);
	TimerEnable(USOUND_TIMER, ECHO_TIMER);

	// Allow the timers to interrupt - the interrupts have not been enabled at the peripheral level yet
	IntEnable(INT_TIMER1A);
	IntEnable(INT_TIMER1B);

	// The trigger interrupt is enabled, though it won't interrupt until the timer starts counting
	TimerIntEnable(USOUND_TIMER, TRIGGER_TIMER_INT);

	// Delay for ~200ms to allow the module to properly initialize
	rti_Register(usound_InitCallback, NULL, RTI_ONCE, RTI_MS_TO_TICKS(USOUND_STARTUP_DELAY_MS));

	while (usound_isInit != true)
		;

	return;
}

void usound_InitCallback (void *data, rti_time period, rti_id id)
{
	if (usound_data.stage == INITIALIZING)
	{
		usound_isInit = true;
		usound_data.stage = READY;

		usound_Measure();
	}
	else
	{
		while (1)
			; // error
	}
}

void usound_Measure(void)
{
	if (usound_data.stage != READY)
	{
		while (1)
			; // error
	}
	else
	{
		usound_data.halfReady = false;
		usound_data.stage = TRIGGERING;

		// Do a 10us pulse on the trigger pin
		GPIOPinWrite(USOUND_GPIO, TRIGGER_PIN, 0x00);		// Inverted logic

		TimerLoadSet(USOUND_TIMER, TRIGGER_TIMER, 800); // 10us for a 80MHz clock
		TimerEnable(USOUND_TIMER, TRIGGER_TIMER);

		// Start the 30ms countdown
		rti_Register (usound_SolveTiming, NULL, RTI_ONCE, RTI_MS_TO_TICKS(USOUND_SAMPLE_PERIOD_MS));
	}
}

void Usound_TRIGGER_ISR(void)
{
	TimerIntClear(USOUND_TIMER, TRIGGER_TIMER_INT);

	if (usound_data.stage != TRIGGERING)
	{
		while (1)
			; // error
	}
	else
	{
		// Trigger Clear
		GPIOPinWrite(USOUND_GPIO, TRIGGER_PIN, 0xFF);		// Inverted logic
		usound_data.stage = WAITING_FOR_ECHO;

		// Echo Rising Edge
		TimerControlEvent(USOUND_TIMER, ECHO_TIMER, TIMER_EVENT_POS_EDGE);
		// Enable Echo Input Capture
		TimerIntEnable(USOUND_TIMER, ECHO_TIMER_INT);
	}

	while ((TimerIntStatus(USOUND_TIMER, false) & TRIGGER_TIMER_INT) != 0)
		;

	return;
}

void Usound_ECHO_ISR(void)
{
	TimerIntClear(USOUND_TIMER, ECHO_TIMER_INT);

	if (usound_data.stage == WAITING_FOR_ECHO)
	{
		TimerControlEvent(USOUND_TIMER, ECHO_TIMER, TIMER_EVENT_NEG_EDGE); // Echo Falling Edge
		usound_data.stage = WAITING_FOR_ECHO_TO_END;

		usound_data.measuredValue = TimerValueGet(USOUND_TIMER, ECHO_TIMER) & 0xFFFFFF;
	}
	else if (usound_data.stage == WAITING_FOR_ECHO_TO_END)
	{
		TimerIntDisable(USOUND_TIMER, ECHO_TIMER_INT);
		usound_data.stage = COOLING_DOWN;

		usound_data.measuredValue = ((TimerValueGet(USOUND_TIMER, ECHO_TIMER) & 0xFFFFFF) - usound_data.measuredValue) & 0xFFFFFF;

		rti_Register (usound_SolveTiming, NULL, RTI_ONCE, RTI_MS_TO_TICKS(USOUND_COOLDOWN_MS));
	}
	else
	{
		while (1)
			;	// error
	}


	while ((TimerIntStatus(USOUND_TIMER, false) & ECHO_TIMER_INT) != 0)
		;
	return;
}


void usound_SolveTiming (void *data, rti_time period, rti_id id)
{
	// Two conditions have to be met for a measurement to end and the device to become ready again:
	//	- ~30ms must have elapsed since the start of the measurement
	//	- ~2ms must have elapsed since the ending of the echo
	// The first of these events (whichever occurs first) will move the driver to a half ready state,
	// and the second one will fully reenable the module and report back the measurement result.

	if (usound_data.halfReady == false)
	{
		usound_data.halfReady = true;
	}
	else
	{
		if (usound_data.stage != COOLING_DOWN)
		{
			while (1)
				; // error
		}
		else
		{
			usound_data.stage = READY;
			usound_meas = usound_data.measuredValue;
			usound_meas_available = true;

			usound_Measure();
		}
	}

	return;
}

void usound_Timeout (void *data, rti_time period, rti_id id)
{
	TimerIntDisable(USOUND_TIMER, ECHO_TIMER);

	usound_data.stage = READY;

	usound_Measure();

	return;
}
