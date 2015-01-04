/**
 * Hardware initialization (pins, clocks, etc)
 *
 * @author Juan I Carrano.
 */

#include "common.h"
#include <inc/hw_types.h>
#include <inc/hw_nvic.h>
#include <driverlib/sysctl.h>
#include "hw_init.h"
#include "flyx5_hw.h"

bool running_under_debugger()
{
	return (HWREGB(NVIC_DBG_CTRL)&NVIC_DBG_CTRL_C_DEBUGEN) ? true : false;
}

void init_jtag_muxed_pins()
{
	/* Unlock jtag pins */
	GPIO_UNLOCK(BUTTON_1);
	gpio_commit_ctrl(PORT_OF(BUTTON_1), PIN_N(BUTTON_1), Unlock);
	gpio_commit_ctrl(PORT_OF(LED_A), PIN_N(LED_A), Unlock);
	gpio_commit_ctrl(PORT_OF(LED_B), PIN_N(LED_B), Unlock);

	/* Modify pins */
	CFG_PIN(	BUTTON_1	)
	CFG_PIN(	LED_A		)
	CFG_PIN(	LED_B		)

	/* lock them again */
	GPIO_LOCK(BUTTON_1);
}

void init_pins()
{
	CFG_PIN(	BUTTON_2	)

	CFG_PIN(	DEBUG_RX	)
	CFG_PIN(	DEBUG_TX	)

	CFG_PIN(	BATT_SENSE0	)
	CFG_PIN(	BATT_SENSE1	)

	CFG_PIN(	SENSORS_SCL	)
	CFG_PIN(	SENSORS_SDA	)

	CFG_PIN(	DMU_SCL		)
	CFG_PIN(	DMU_SDA		)

	CFG_PIN(	DMU_INT		)
	CFG_PIN(	MAG_INT		)
	CFG_PIN(	ALT_INT12	)

	CFG_PIN(	ULTRA_PROBE	)
	CFG_PIN(	ULTRA_ECHO	)

	CFG_PIN(	ESC_SCK		)
	CFG_PIN(	ESC_MISO	)
	CFG_PIN(	ESC_MOSI	)

	CFG_PIN(	ESC_PWM0	)
	CFG_PIN(	ESC_PWM1	)
	CFG_PIN(	ESC_PWM2	)
	CFG_PIN(	ESC_PWM3	)
	CFG_PIN(	ESC_PWM4	)
	CFG_PIN(	ESC_PWM5	)
	CFG_PIN(	ESC_PWM6	)
	CFG_PIN(	ESC_PWM7	)

	CFG_PIN(	AUX_SCK		)
	CFG_PIN(	SDCARD_SS	)
	CFG_PIN(	AUX_MISO	)
	CFG_PIN(	AUX_MOSI	)
	CFG_PIN(	NRF_CE		)
	CFG_PIN(	NRF_SS		)
	CFG_PIN(	NRF_INT		)

	CFG_PIN(	BUZZER		)

	CFG_PIN(	RC_ROLL		)
	CFG_PIN(	RC_PITCH        )
	CFG_PIN(	RC_YAW          )
	CFG_PIN(	RC_ALTITUDE     )
	CFG_PIN(	RC_AUX1         )
	CFG_PIN(	RC_AUX2         )
}

void init_all_gpio()
{
	R_(SysCtlPeripheralEnable)(SYS_PERIPH(GPIOA));
	R_(SysCtlPeripheralEnable)(SYS_PERIPH(GPIOB));
	R_(SysCtlPeripheralEnable)(SYS_PERIPH(GPIOC));
	R_(SysCtlPeripheralEnable)(SYS_PERIPH(GPIOD));
	R_(SysCtlPeripheralEnable)(SYS_PERIPH(GPIOE));
	R_(SysCtlPeripheralEnable)(SYS_PERIPH(GPIOF));
}

void init_clock()
{
	/* The following parameters should provide us a 80MHz clock
	 * Note that abobe 40MHz, the flash controller operates at half the
	 * system clock, so random-access throughput suffers and is only
	 * recovered at 80MHz.
	 * Linear-access is not affected due to pre-fetching. */

	R_(SysCtlClockSet)(
		  SYSCTL_USE_PLL		/* Use the PLL so we can get a
							higher frequency */
		| SYSCTL_OSC_MAIN		/* Use the main oscillator */
		| SYSCTL_SYSDIV_2_5		/* 400MHz / 2.5 / 2 = 80MHz */
		| GLUE3(SYSCTL_XTAL_,XTAL_MHZ,MHZ)	/* The board xtal */
		);

	/* The ADC **MUST** be clocked with 16 MHz
	 * We will use the PLL divided by 25. */

	/* TODO: setup ADC clock */
}
