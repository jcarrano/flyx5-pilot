/**
 * Hardware initialization (pins, clocks, etc)
 *
 * @author Juan I Carrano.
 */

#ifndef HW_INIT_H
#define HW_INIT_H

#include <stdbool.h>

/**
 * Return true if running under debugger control.
 *
 * This function only works in Cortex-M3 and later.
 *
 * @see http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka14536.html
 */
bool running_under_debugger();

/**
 * Initialise the pins that are shared with the jtag interface.
 *
 * After this pins are configured, the chip cannot longer be accesed by jtag
 * (until the pins are reverted to the jtag function).
 *
 * On reset, this function must be called after a small delay, to allow the
 * debugger to access the chip.
 */
void init_jtag_muxed_pins();

/**
 * Initialise all other pins.
 */
void init_pins();

/**
 * Initialise all gpio modules.
 *
 * GPIO Modules must be initialised before the pins can be reconfigured.
 */
void init_all_gpio();

/**
 * Anti-brick routine.
 *
 * Check if the failsafe button is pressed and loop forever.
 * The program should call this routine before doing anything, to provide a way
 * to enter debug mode.
 */
void init_failsafe();

/**
 * Initialise the PLL and system clock.
 */
void init_clock();

#endif /* HW_INIT_H */
