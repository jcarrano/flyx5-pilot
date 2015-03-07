#include "joystick.h"

#include <stdbool.h>
#include <stddef.h>

#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "../xdriver/rti.h"

// Calibration
//#define JOY_CALIB // This causes the raw values (which should then be inserted into the calibration defines) to be outputted

#define S_MAX (32767)
#define S_MIN (-32768)
#define U_MAX S_MAX

#define LINEAR_SCALE_U(x,min,max) ((x - min) * (U_MAX) / (max - min))
#define LINEAR_SCALE_S(x,min,max,repos) ((x > repos) ? LINEAR_SCALE_S_UPP(x, repos, max) : LINEAR_SCALE_S_LOW(x, repos, min))
#define LINEAR_SCALE_S_UPP(x,repos,max) ((x - repos) * (S_MAX) / (max - repos))
#define LINEAR_SCALE_S_LOW(x,repos,min) ((x - repos) * (S_MIN) / (min - repos))

#define SATURATE_U(x) ((x > U_MAX) ? U_MAX : ((x < 0) ? 0 : x))
#define SATURATE_S(x) ((x > S_MAX) ? S_MAX : ((x < S_MIN) ? S_MIN : x))
#define INVERT_S(x, inverted) ((inverted != 0) ? (-x) : (x))

#define YAW_MIN (89000)
#define YAW_MAX (152000)
#define YAW_REST (120000)
#define YAW_INVERTED 1

#define PITCH_MIN (88000)
#define PITCH_MAX (152000)
#define PITCH_REST (120000)
#define PITCH_INVERTED 0

#define ROLL_MIN (89000)
#define ROLL_MAX (152000)
#define ROLL_REST (120000)
#define ROLL_INVERTED 1

#define ELEV_MIN (88000)
#define ELEV_MAX (153000)

#define JOYSTICK_GPIO_PORT_BASE GPIO_PORTC_BASE

// A couple of configuration bits are hardcoded, these defines are meant to be used as a reference,
// NOT as a means to modify the pin assignments

#define ROLL_PIN	GPIO_PIN_4
#define PITCH_PIN	GPIO_PIN_5
#define YAW_PIN		GPIO_PIN_6
#define ELEV_PIN	GPIO_PIN_7

#define TIMER_BASE(pin) (((pin == GPIO_PIN_4) || (pin == GPIO_PIN_5)) ? WTIMER0_BASE : WTIMER1_BASE)
#define TIMER_A_OR_B(pin) (((pin == GPIO_PIN_4) || (pin == GPIO_PIN_6)) ? TIMER_A : TIMER_B)
#define TIMER_CAP_EVENT(a_or_b) ((a_or_b == TIMER_A) ? TIMER_CAPA_EVENT : TIMER_CAPB_EVENT)

#define ROLL_IDX 0
#define PITCH_IDX 1
#define YAW_IDX 2
#define ELEV_IDX 3

volatile joy_data_t joy_data;
volatile bool joy_no_signal;

typedef struct
{
	int32_t last_meas;
	bool rising;
	int32_t raw_meas;

	bool measured;
} joy_internal_data_t;

joy_internal_data_t joy_internal_data[4];

uint32_t PinConfigurationForPortPin(uint8_t pin);
void InitTimers(void);
void Joystick_ISR_Handler(uint8_t axis_pin, uint8_t axis_idx);

void Joystick_CheckSignal (void *data, rti_time period, rti_id id);

void joy_Init(void)
{
	joy_no_signal = true;

	joy_data.elev = 0;
	joy_data.roll = 0;
	joy_data.pitch = 0;
	joy_data.yaw = 0;

	rti_Init();

	int i;
	for (i = 0; i < 4; i++)
	{
		joy_internal_data[i].measured = false;
	}

	InitTimers();

	rti_Register(Joystick_CheckSignal, NULL, RTI_MS_TO_TICKS(200), RTI_NOW);
}

void InitTimers(void)
{
	// GPIOC4 & 5 are mapped to Wide Timer 0, 6 & 7 are mapped to Wide Timer 1

	// GPIOC enable
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	// Map GPIO to timers
	GPIOPinConfigure(PinConfigurationForPortPin(ROLL_PIN));
	GPIOPinConfigure(PinConfigurationForPortPin(PITCH_PIN));
	GPIOPinConfigure(PinConfigurationForPortPin(YAW_PIN));
	GPIOPinConfigure(PinConfigurationForPortPin(ELEV_PIN));

	// GPIOC4, 5, 6 & 7 enable as timers
	GPIOPinTypeTimer(JOYSTICK_GPIO_PORT_BASE, ROLL_PIN | PITCH_PIN | YAW_PIN | ELEV_PIN);

	// Enable Wide Timers 0 & 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER1);

	// Configure both timers A and B for both wide timers to be input captures and measure time in upcount mode
	// For both upcount and downcount mode, the prescale will act as a timer extension
	TimerConfigure(WTIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP | TIMER_CFG_B_CAP_TIME_UP);
	TimerConfigure(WTIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP | TIMER_CFG_B_CAP_TIME_UP);

	// Edge detection
	TimerControlEvent(TIMER_BASE(ROLL_PIN), TIMER_A_OR_B(ROLL_PIN), TIMER_EVENT_POS_EDGE);
	joy_internal_data[ROLL_IDX].rising = true;

	TimerControlEvent(TIMER_BASE(PITCH_PIN), TIMER_A_OR_B(PITCH_PIN), TIMER_EVENT_POS_EDGE);
	joy_internal_data[PITCH_IDX].rising = true;

	TimerControlEvent(TIMER_BASE(YAW_PIN), TIMER_A_OR_B(YAW_PIN), TIMER_EVENT_POS_EDGE);
	joy_internal_data[YAW_IDX].rising = true;

	TimerControlEvent(TIMER_BASE(ELEV_PIN), TIMER_A_OR_B(ELEV_PIN), TIMER_EVENT_POS_EDGE);
	joy_internal_data[ELEV_IDX].rising = true;

	// Allow the timers to interrupt
	IntEnable(INT_WTIMER0A);
	IntEnable(INT_WTIMER0B);
	IntEnable(INT_WTIMER1A);
	IntEnable(INT_WTIMER1B);

	// Enable event interrupts on the timers
	TimerIntEnable(TIMER_BASE(ROLL_PIN), TIMER_CAP_EVENT(TIMER_A_OR_B(ROLL_PIN)));
	TimerIntEnable(TIMER_BASE(PITCH_PIN), TIMER_CAP_EVENT(TIMER_A_OR_B(PITCH_PIN)));
	TimerIntEnable(TIMER_BASE(YAW_PIN), TIMER_CAP_EVENT(TIMER_A_OR_B(YAW_PIN)));
	TimerIntEnable(TIMER_BASE(ELEV_PIN), TIMER_CAP_EVENT(TIMER_A_OR_B(ELEV_PIN)));

	// Enable the timers
	TimerEnable(TIMER_BASE(ROLL_PIN), TIMER_A_OR_B(ROLL_PIN));
	TimerEnable(TIMER_BASE(PITCH_PIN), TIMER_A_OR_B(PITCH_PIN));
	TimerEnable(TIMER_BASE(YAW_PIN), TIMER_A_OR_B(YAW_PIN));
	TimerEnable(TIMER_BASE(ELEV_PIN), TIMER_A_OR_B(ELEV_PIN));

	return;
}

uint32_t PinConfigurationForPortPin(uint8_t pin)
{
	switch (pin)
	{
		case GPIO_PIN_4:
			return GPIO_PC4_WT0CCP0; // Wide 0 Timer A

		case GPIO_PIN_5:
			return GPIO_PC5_WT0CCP1; // Wide 0 Timer B

		case GPIO_PIN_6:
			return GPIO_PC6_WT1CCP0; // Wide 1 Timer A

		case GPIO_PIN_7:
			return GPIO_PC7_WT1CCP1; // Wide 1 Timer B

		default: // Error
			while(1)
				;
	}
}

void Joystick_ROLL_ISR(void)
{
	Joystick_ISR_Handler(ROLL_PIN, ROLL_IDX);
}

void Joystick_PITCH_ISR(void)
{
	Joystick_ISR_Handler(PITCH_PIN, PITCH_IDX);
}

void Joystick_YAW_ISR(void)
{
	Joystick_ISR_Handler(YAW_PIN, YAW_IDX);
}

void Joystick_ELEV_ISR(void)
{
	Joystick_ISR_Handler(ELEV_PIN, ELEV_IDX);
}

void Joystick_ISR_Handler(uint8_t axis_pin, uint8_t axis_idx)
{
	TimerIntClear(TIMER_BASE(axis_pin), TIMER_CAP_EVENT(TIMER_A_OR_B(axis_pin)));

	if (joy_internal_data[axis_idx].rising == true)
	{
		joy_internal_data[axis_idx].last_meas = TimerValueGet(TIMER_BASE(axis_pin), TIMER_A_OR_B(axis_pin));

		joy_internal_data[axis_idx].rising = false;
		TimerControlEvent(TIMER_BASE(axis_pin), TIMER_A_OR_B(axis_pin), TIMER_EVENT_NEG_EDGE);
	}
	else
	{
		joy_internal_data[axis_idx].raw_meas = TimerValueGet(TIMER_BASE(axis_pin), TIMER_A_OR_B(axis_pin)) - joy_internal_data[axis_idx].last_meas;
		int32_t raw_meas = joy_internal_data[axis_idx].raw_meas;

		if (axis_idx == ROLL_IDX)
		{
			int32_t scaled_meas = LINEAR_SCALE_S(raw_meas, ROLL_MIN, ROLL_MAX, ROLL_REST);
			scaled_meas = SATURATE_S(scaled_meas);
#ifdef JOY_CALIB
			joy_data.roll = raw_meas;
#else
			joy_data.roll = INVERT_S(scaled_meas, ROLL_INVERTED);
#endif

		}
		else if (axis_idx == PITCH_IDX)
		{

			int32_t scaled_meas = LINEAR_SCALE_S(raw_meas, PITCH_MIN, PITCH_MAX, PITCH_REST);
			scaled_meas = SATURATE_S(scaled_meas);
#ifdef JOY_CALIB
			joy_data.pitch = raw_meas;
#else
			joy_data.pitch = INVERT_S(scaled_meas, PITCH_INVERTED);
#endif
		}
		else if (axis_idx == YAW_IDX)
		{
			int32_t scaled_meas = LINEAR_SCALE_S(raw_meas, YAW_MIN, YAW_MAX, YAW_REST);
			scaled_meas = SATURATE_S(scaled_meas);
#ifdef JOY_CALIB
			joy_data.yaw = raw_meas;
#else
			joy_data.yaw = INVERT_S(scaled_meas, YAW_INVERTED);
#endif
		}
		else
		{
			int32_t scaled_meas = LINEAR_SCALE_U(raw_meas, ELEV_MIN, ELEV_MAX);
#ifdef JOY_CALIB
			joy_data.elev = raw_meas;
#else
			joy_data.elev = SATURATE_U(scaled_meas);
#endif
		}

		joy_internal_data[axis_idx].measured = true;
		joy_internal_data[axis_idx].rising = true;
		TimerControlEvent(TIMER_BASE(axis_pin), TIMER_A_OR_B(axis_pin), TIMER_EVENT_POS_EDGE);
	}

	while ((TimerIntStatus(TIMER_BASE(axis_pin), false) & TIMER_CAP_EVENT(TIMER_A_OR_B(axis_pin))) != 0)
		;
}

void Joystick_CheckSignal (void *data, rti_time period, rti_id id)
{
	bool signal_loss_detected = false;

	int i;
	for (i = 0; i < 4; i++)
	{
		if (joy_internal_data[i].measured == false)
		{
			signal_loss_detected = true;
		}
		else
		{
			joy_internal_data[i].measured = false;
		}
	}

	if (signal_loss_detected == true)
	{
		joy_no_signal = true;
	}
	else
	{
		joy_no_signal = false;
	}
}
