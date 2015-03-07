#include "esc.h"

#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#include "inc/hw_memmap.h"

#define ESC_TIMER_0 		WTIMER2_BASE
#define ESC_TIMER_1 		WTIMER3_BASE

#define ESC_TIMER(esc) (((esc == 0) || (esc == 1)) ? ESC_TIMER_0 : ESC_TIMER_1)
#define TIMER_A_OR_B(esc) (((esc == 0) || (esc == 2)) ? TIMER_A : TIMER_B)

#define ESC_TIMER_INT(esc) ((TIMER_A_OR_B(esc) == TIMER_A) ? TIMER_TIMA_MATCH : TIMER_TIMB_MATCH)
#define ESC_TIMER_CFG(esc) ((TIMER_A_OR_B(esc) == TIMER_A) ? TIMER_CFG_A_PWM : TIMER_CFG_B_PWM)

#define ESC_GPIO GPIO_PORTD_BASE
#define ESC_PIN(esc) ((esc == 0) ? GPIO_PIN_0 : ((esc == 1) ? GPIO_PIN_1 : ((esc == 2) ? GPIO_PIN_2 : GPIO_PIN_3)))

#define ESC_PERIOD 200000 // 2.5ms - 400Hz (for a 80MHz clock)
#define ESC_MIN (ESC_PERIOD - 80000) // 1ms
#define ESC_SPAN 80000 // 1ms span

#define ESC_VALUE(percent) (ESC_MIN - ((ESC_SPAN * ((percent) & 0x7FFF)) / 0x7FFF))

#define ESC_LEVEL

bool esc_IsInit = false;

void esc_InitTimers(void);

void esc_Init(void)
{
	if (esc_IsInit == true)
		return;

	esc_InitTimers();
	esc_IsInit = true;
}

void esc_InitTimers(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	GPIOPinConfigure(GPIO_PD0_WT2CCP0);
	GPIOPinConfigure(GPIO_PD1_WT2CCP1);
	GPIOPinConfigure(GPIO_PD2_WT3CCP0);
	GPIOPinConfigure(GPIO_PD3_WT3CCP1);

	GPIOPinTypeTimer(ESC_GPIO, ESC_PIN(0));
	GPIOPinTypeTimer(ESC_GPIO, ESC_PIN(1));
	GPIOPinTypeTimer(ESC_GPIO, ESC_PIN(2));
	GPIOPinTypeTimer(ESC_GPIO, ESC_PIN(3));

	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER3);

	TimerConfigure(WTIMER2_BASE, TIMER_CFG_SPLIT_PAIR | ESC_TIMER_CFG(0) | ESC_TIMER_CFG(1));
	TimerConfigure(WTIMER3_BASE, TIMER_CFG_SPLIT_PAIR | ESC_TIMER_CFG(2) | ESC_TIMER_CFG(3));

	TimerControlLevel(ESC_TIMER(0), TIMER_A_OR_B(0), false);
	TimerControlLevel(ESC_TIMER(1), TIMER_A_OR_B(1), false);
	TimerControlLevel(ESC_TIMER(2), TIMER_A_OR_B(2), false);
	TimerControlLevel(ESC_TIMER(3), TIMER_A_OR_B(3), false);

	TimerLoadSet(ESC_TIMER(0), TIMER_A_OR_B(0), ESC_PERIOD);
	TimerLoadSet(ESC_TIMER(1), TIMER_A_OR_B(1), ESC_PERIOD);
	TimerLoadSet(ESC_TIMER(2), TIMER_A_OR_B(2), ESC_PERIOD);
	TimerLoadSet(ESC_TIMER(3), TIMER_A_OR_B(3), ESC_PERIOD);

	esc_SetValues(0, 0, 0, 0);

	TimerEnable(ESC_TIMER(0), TIMER_A_OR_B(0));
	TimerEnable(ESC_TIMER(1), TIMER_A_OR_B(1));
	TimerEnable(ESC_TIMER(2), TIMER_A_OR_B(2));
	TimerEnable(ESC_TIMER(3), TIMER_A_OR_B(3));
}


void esc_SetValues(uint32_t ch0, uint32_t ch1, uint32_t ch2, uint32_t ch3)
{
	TimerMatchSet(ESC_TIMER(0), TIMER_A_OR_B(0), ESC_VALUE(ch0));
	TimerMatchSet(ESC_TIMER(1), TIMER_A_OR_B(1), ESC_VALUE(ch1));
	TimerMatchSet(ESC_TIMER(2), TIMER_A_OR_B(2), ESC_VALUE(ch2));
	TimerMatchSet(ESC_TIMER(3), TIMER_A_OR_B(3), ESC_VALUE(ch3));
}
