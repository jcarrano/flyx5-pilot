/*
 * gpio_interface.h
 *
 *  Created on: 14/02/2015
 *      Author: Juan Ignacio
 */

#ifndef GPIO_INTERFACE_H_
#define GPIO_INTERFACE_H_

#include "inc/hw_memmap.h"
#include <stdbool.h>
#include "driverlib/gpio.h"
#include "../common.h"

enum {
	GPIO_A = 0,
	GPIO_B,
	GPIO_C,
	GPIO_D,
	GPIO_E
};

extern const uint32_t GPIO_BASES[];
#define GPIO_BASE(x) (GPIO_BASES[x])

/*
 * Write tasks to perform in .c file.
 */


//! Initiates GPIO interrupts in given GPIO module and pins, of the given type (Edge rising/ falling/ both - Level high/ low).
//! Individual actions to perform for each pin shall be written in .c file in corresponding module data structure.
//! @param gpioNumber: GPIO module number (use enum above).
//! @param gpioPins: pins that will interrupt in the module; use GPIO_PIN_X(s) ORd.
//! @param gpioIntType: use one of the std types: GPIO_RISING_EDGE / _FALLING_EDGE / _BOTH_EDGES / _HIGH_LEVEL / _LOW_LEVEL.
void gpio_Init(uint8_t gpioNumber, uint32_t gpioPins, uint32_t gpioIntType);


// TI GPIOIntTypeGet corrected.
uint32_t gpio_IntTypeGet(uint32_t ui32Port, uint8_t ui8Pin);


#endif /* GPIO_INTERFACE_H_ */
