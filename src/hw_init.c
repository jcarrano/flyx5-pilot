/**
 * Hardware initialization (pins, clocks, etc)
 *
 * @author Juan I Carrano.
 */

#include "flyx5_pin.h"
#include <stdbool.h>

/**
 * Return true if running under debugger control.
 *
 * This function only works in Cortex-M3 and later.
 *
 * @see http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka14536.html
 */
static bool running_under_debugger()
{
#define C_DEBUGEN_BIT 0
	return HWREGBITW(NVIC_DBG_CTRL, C_DEBUGEN_BIT);
}

/**
 * Initialise the pins that are shared with the jtag interface.
 *
 * After this pins are configured, the chip cannot longer be accesed by jtag
 * (until the pins are reverted to the jtag function).
 *
 * On reset, this function must be called after a small delay, to allow the
 * debugger to access the chip.
 */
void init_jtag_muxed_pins()
{
	/* Unlock jtag pins */
	GPIO_UNLOCK(BUTTON_1);
	gpio_commit_ctrl(PORT_OF(BUTTON_1), PIN_N(BUTTON_1));
	gpio_commit_ctrl(PORT_OF(LED_A), PIN_N(LED_A));
	gpio_commit_ctrl(PORT_OF(LED_B), PIN_N(LED_B));

	/* Modify pins */
	CFG_PIN(	BUTTON_1	)
	CFG_PIN(	LED_A		)
	CFG_PIN(	LED_B		)

	/* lock them again */
	GPIO_LOCK(BUTTON_1);
}

/**
 * Initialise all other pins.
 */
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

	CFG_PIN(	ESC_PWM(0)	)
	CFG_PIN(	ESC_PWM(1)	)
	CFG_PIN(	ESC_PWM(2)	)
	CFG_PIN(	ESC_PWM(3)	)
	CFG_PIN(	ESC_PWM(4)	)
	CFG_PIN(	ESC_PWM(5)	)
	CFG_PIN(	ESC_PWM(6)	)
	CFG_PIN(	ESC_PWM(7)	)

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
