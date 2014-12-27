/**
 * Pin definitions for flyx5 board.
 *
 * @author  Juan I Carrano
 *
 * @copyright Copyright 2014 Carrano, Calcabrini, Ubeira, Venturo.
 */

#ifndef FLYX5_PIN_H
#define FLYX5_PIN_H

#include <inc/hw_gpio.h>
#include <driverlib/gpio.h>
#include "xdriver/gpio_extra.h"
#include "macro_magic.h"

// FIXME: create TimerIn & TimerOut

/* ************************ Macros to work with pins ************************/

/* Configure the pin */

#define CFG_PIN5(port, pin, function, type, activity) { \
	R(GPIOPinType##type)(GPIO_PORT##port##_BASE, GPIO_PIN_##pin); \
	R(GPIOPinConfigure)(GPIO_P##port##pin##_##function); \
	}

#define CFG_PINs(port, pins, type, activity) { \
	R(GPIOPinType##type)(GPIO_PORT##port##_BASE, pins); \
	}

#define DEF_PIN4(port, pins, type, activity, _0) { \
	R(GPIOPinType##type)(GPIO_PORT##port##_BASE, GPIO_PIN_##pin); \
	}

#define CFG_PIN(...) _POLYARGS5(__VA_ARGS__, CFG_PIN5, DEF_PIN4)

/* Get the correspondig port base */

#define PORT_OF5(port, pin, function, type, activity) (GPIO_PORT##port##_BASE)

#define PORT_OF4(port, pins, type, activity, _0) (GPIO_PORT##port##_BASE)

#define PORT_OF(...) _POLYARGS5(__VA_ARGS__, PORT_OF5, PORT_OF4)

/* Get the pin number */

#define PIN_N(port, pin, ...) (pin)

/* Interrupt enabling and disabling for gpios */

// TODO: implement
#define PIN_INT_EN(port, pins, type, activity, _0) GPIOIntEnable(, GPIO_INT_PIN_##pin);

/* Lock and unlock special function pins */

#define GPIO_UNLOCK(p) gpio_lock(PORT_OF(p), Unlock)
#define GPIO_LOCK(p) gpio_lock(PORT_OF(p), Lock)

/* ************************* Pin map table ********************************** */

/*	Name		Port	Pin	Function	Type		Active*/

/* These three pins are shared with JTAG. Initialization must be delayed by some
 * ms to allow debugging */
#define BUTTON_1	C,	3,			GPIOInput,	ACT_FALL
#define LED_A		C,	2,			GPIOOutput,	ACT_HI
#define LED_B		C,	1,			GPIOOutput,	ACT_HI

#define BUTTON_2	E,	2			GPIOInput,	ACT_FALL

/* Unusable gpio due to an errata
JTAG
Unusable	PC0
*/

#define DEBUG_RX        A,	0,	U0RX,		UART,
#define	DEBUG_TX        A,	1,	U0TX,		UART,

#define BATT_SENSE0     E,	0,			ADC,
#define BATT_SENSE1     E,	1,			ADC,

/* i2c buses */
#define SENSORS_SCL     A,	6,	I2C1SCL,	I2C,
#define SENSORS_SDA     A,	7,	I2C1SDA,	I2C,

#define DMU_SCL         B,	2,	I2C0SCL,	I2C,
#define DMU_SDA         B,	3,	I2C0SDA, 	I2C,

/* sensor interrupts */
#define	DMU_INT         E,	3,			GPIOInput,	ACT_??
#define	MAG_INT         E,	4,			GPIOInput,	ACT_??
#define	ALT_INT12       E,	5,			GPIOInput,	ACT_??

/* ultrasound */
#define ULTRA_PROBE     F,	2,	T1CCP0, 	Timer,		ACT_HI
#define ULTRA_ECHO      F,	3,	T1CCP1,		Timer,		ACT_HI

/* Use this definitions for SPI motor controllers */
#define ESC_SCK         B,	4,	SSI2CLK,	SSI,
#define ESC_MISO        B,	6,	SSI2RX,		SSI,
#define ESC_MOSI        B,	7,	SSI2TX,		SSI,

#define ESC_SS(n)	D,	n,			GPIOOutput,	ACT_LO
#define	ESC_SSs		D,	0xFF,			GPIOOutput,	ACT_LO

/* Use this definitions for traditional PWM-controlled motor controllers */

#define ESC_PWM(n)	D,	n,	WT2CCP##n,	Timer,		ACT_HI

/* Aux spi bus */
#define AUX_SCK         A,	2,	SSI0CLK,	SSI,
#define SDCARD_SS	A,	3,			GPIOOutput,	ACT_LO
#define AUX_MISO        A,	4,	SSI0RX,		SSI,
#define AUX_MOSI        A,	5,	SSI0TX,		SSI,
#define NRF_CE          B,	0,			GPIOOutput,
#define NRF_SS          B,	1,			GPIOOutput,
#define NRF_INT         B,	5,			GPIOInput,

#define BUZZER          F,	4,	T2CCP0,		Timer,

#define RC_ROLL         C,	4,	WT0CCP0,	Timer,		ACT_HI
#define RC_PITCH        C,	5,	WT0CCP1,	Timer,		ACT_HI
#define RC_YAW          C,	6,	WT1CCP0,	Timer,		ACT_HI
#define RC_ALTITUDE     C,	7	WT1CCP1,	Timer,		ACT_HI
#define RC_AUX1         F,	0,	T0CCP0,		Timer,		ACT_HI
#define RC_AUX2         F,	1,	T0CCP1,		Timer,		ACT_HI

/* *********************** Peripherals used **************************** */

#define UART_DEBUG	UART0
#define BUS_SENSORS	I2C1
#define BUS_DMU		I2C0
/* Timers for ultrasound */
#define TIMER_ULTRA	TIMER1
/* SPI bus for SPI-controlled ESCS */
#define BUS_ESC		SSI2
/* Timer used for PWM-controlled ESCS */
#define TIMER_ESC	WTIMER2
/* Spi bus for NRF and SD card */
#define BUS_AUX		SSI0

#define TIMER_BUZZER	TIMER2
/* Timer for the roll & yaw channels. */
#define TIMER_RC_RY	WTIMER0
/* Timer for the pitch & altitude channels. */
#define TIMER_RC_PA	WTIMER1
/* Timer for the auxiliary channels. */
#define TIMER_RC_AUX	TIMER0

#endif /* FLYX5_PIN_H */
