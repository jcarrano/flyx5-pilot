/**
 * Hardware definitions for flyx5 board.
 *
 * @author  Juan I Carrano
 *
 * @copyright Copyright 2014 Carrano, Calcabrini, Ubeira, Venturo.
 */

#ifndef FLYX5_HW_H
#define FLYX5_HW_H

#include "common.h"
#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include "xdriver/gpio_extra.h"
#include "macro_magic.h"

// FIXME: create TimerIn & TimerOut

/* ************************ Macros to work with pins ************************/

/* Configure the pin */

#define CFG_PINs(port, pins, type, activity) { \
	R_(GPIOPinType##type)(GPIO_PORT##port##_BASE, pins); \
	}

#define CFG_PIN5(port, pin, function, type, activity) { \
	R_(GPIOPinConfigure)(GPIO_P##port##pin##_##function); \
	R_(GPIOPinType##type)(GPIO_PORT##port##_BASE, GLUE(GPIO_PIN_, pin)); \
	}

#define CFG_PIN4(port, pin, type, activity, _0) { \
	R_(GPIOPinType##type)(GPIO_PORT##port##_BASE, GLUE(GPIO_PIN_, pin)); \
	}

#define CFG_PIN(...) _POLYARGS5(__VA_ARGS__, CFG_PIN5, CFG_PIN4, _0)

/* Get the correspondig port base */

#define PORT_OF5(port, pin, function, type, activity) GLUE3(GPIO_PORT,port,_BASE)

#define PORT_OF4(port, pins, type, activity, _0) GLUE3(GPIO_PORT,port,_BASE)

#define PORT_OF(...) _POLYARGS5(__VA_ARGS__, PORT_OF5, PORT_OF4, _0)

/* Get the pin number */

#define PIN_N_(port, pin, ...) (pin) MDUMMY((__VA_ARGS__))
#define PIN_N(...) PIN_N_(__VA_ARGS__)

/* Interrupt enabling and disabling for gpios */

// TODO: implement
#define PIN_INT_EN(port, pins, type, activity, _0) R_(GPIOIntEnable)(port, GPIO_INT_PIN_##pin);

/* Lock and unlock special function pins */

#define GPIO_UNLOCK(p) gpio_lock(PORT_OF(p), Unlock)
#define GPIO_LOCK(p) gpio_lock(PORT_OF(p), Lock)

/* Get the pin state */

#define _XFORM_ACT_HI(value, mask) ((value) & 0xFFF0)
#define _XFORM_ACT_RISE(value, mask) ((value) & 0xFFF0)
#define _XFORM_ACT_LO(value, mask) ((~value) & mask)
#define _XFORM_ACT_FALL(value, mask) ((~value) & mask)

#define _PIN_ACTIVE(port, pin, type, activity) \
	(GLUE(_XFORM_, activity)(GPIOPinRead(PORT_OF(port, pin, type, activity), \
	 BIT(PIN_N(port, pin))), BIT(PIN_N(port, pin))))

#define PIN_ACTIVE(...) _PIN_ACTIVE(__VA_ARGS__)

/* Get the gpio peripheral for pin */

#define GPIO_PERIPH_(port, ...) GLUE(SYSCTL_PERIPH_GPIO, port) MDUMMY((__VA_ARGS__))
#define GPIO_PERIPH(...) GPIO_PERIPH_(__VA_ARGS__)

/* ************************* Pin map table ********************************** */

/*	Name		Port	Pin	Function	Type		Active*/

/* These three pins are shared with JTAG. Initialization must be delayed by some
 * ms to allow debugging */
#define BUTTON_1	C,	3,			GPIOInput,	ACT_FALL
#define LED_A		C,	2,			GPIOOutput,	ACT_HI
#define LED_B		C,	1,			GPIOOutput,	ACT_HI

#define BUTTON_2	E,	2,			GPIOInput,	ACT_FALL

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
#define	DMU_INT         E,	3,			GPIOInput,	ACT_RISE
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

#define ESC_PWM0	D,	0,	WT2CCP0,	Timer,		ACT_HI
#define ESC_PWM1	D,	1,	WT2CCP1,	Timer,		ACT_HI
#define ESC_PWM2	D,	2,	WT3CCP0,	Timer,		ACT_HI
#define ESC_PWM3	D,	3,	WT3CCP1,	Timer,		ACT_HI
#define ESC_PWM4	D,	4,	WT4CCP0,	Timer,		ACT_HI
#define ESC_PWM5	D,	5,	WT4CCP1,	Timer,		ACT_HI
#define ESC_PWM6	D,	6,	WT5CCP0,	Timer,		ACT_HI
#define ESC_PWM7	D,	7,	WT5CCP1,	Timer,		ACT_HI

/* Aux spi bus */
#define AUX_SCK         A,	2,	SSI0CLK,	SSI,
#define SDCARD_SS	A,	3,			GPIOOutput,	ACT_LO
#define AUX_MISO        A,	4,	SSI0RX,		SSI,
#define AUX_MOSI        A,	5,	SSI0TX,		SSI,
#define NRF_CE          B,	0,			GPIOOutput,
#define NRF_SS          B,	1,			GPIOOutput,
#define NRF_INT         B,	5,			GPIOInput,

#define BUZZER          F,	4,	T2CCP0,		Timer,		ACT_HI

#define RC_ROLL         C,	4,	WT0CCP0,	Timer,		ACT_HI
#define RC_PITCH        C,	5,	WT0CCP1,	Timer,		ACT_HI
#define RC_YAW          C,	6,	WT1CCP0,	Timer,		ACT_HI
#define RC_ALTITUDE     C,	7,	WT1CCP1,	Timer,		ACT_HI
#define RC_AUX1         F,	0,	T0CCP0,		Timer,		ACT_HI
#define RC_AUX2         F,	1,	T0CCP1,		Timer,		ACT_HI

/* *********************** Peripherals used **************************** */

#define UART_DEBUG_NUM 0
#define BUS_SENSORS_NUM	1
#define BUS_DMU_NUM 	0

#define UART_DEBUG	GLUE(UART, UART_DEBUG_NUM)
#define BUS_SENSORS	GLUE(I2C, BUS_SENSORS_NUM)
#define BUS_DMU		GLUE(I2C, BUS_DMU_NUM)

/* Timers for ultrasound */
#define TIMER_ULTRA	TIMER1
/* SPI bus for SPI-controlled ESCS */
#define BUS_ESC		SSI2
/* Timers used for PWM-controlled ESCS */
#define TIMER_ESC01	WTIMER2
#define TIMER_ESC23	WTIMER3
#define TIMER_ESC45	WTIMER4
#define TIMER_ESC67	WTIMER5
/* Spi bus for NRF and SD card */
#define BUS_AUX		SSI0

#define TIMER_BUZZER	TIMER2
	/* Half-Timer for the frequency synthesis */
#define TIMER_BUZZER_SYN	A
	/* Half-Timer for the note sequencing */
#define TIMER_BUZZER_SEQ	B

/* Timer for the roll & yaw channels. */
#define TIMER_RC_RY	WTIMER0
/* Timer for the pitch & altitude channels. */
#define TIMER_RC_PA	WTIMER1
/* Timer for the auxiliary channels. */
#define TIMER_RC_AUX	TIMER0


/* ******************* Macros to work with timers ********************* */
#define sTIMER(t) GLUE(TIMER_, t)
#define TIMER_CFG(t, cfg) GLUE3(TIMER_CFG_, t, cfg)

/* ******************* Macros to work with peripherals ***************** */

#define ENABLE_AND_RESET(periph) { \
	R_(SysCtlPeripheralEnable)(SYS_PERIPH(periph)); \
	R_(SysCtlPeripheralReset)(SYS_PERIPH(periph)); \
	}

#define SYS_PERIPH(periph) GLUE(SYSCTL_PERIPH_, periph)
#define BASE_PERIPH(periph) GLUE(periph, _BASE)

/* *********************** Clock   ************************************ */

#define XTAL_MHZ  20

#endif /* FLYX5_HW_H */
