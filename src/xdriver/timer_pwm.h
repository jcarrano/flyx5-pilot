/**
 * Functions for using timers as PWM.
 *
 * Only 16 bit mode is currently supported.
 */

#ifndef TIMER_PWM_H
#define TIMER_PWM_H

#include <stdint.h>

/**
 * Set the load and prescaler registers for use in 16 bit timer.
 *
 * This function partitions the value into high and low parts, for loading the
 * IRL and PR.
 */
void TimerPWMLoadSet16(uint32_t ui32Base, uint32_t ui32Timer, uint32_t ui32Value);

/**
 * Set the MATCH and prescaler MATCH registers for use in 16 bit timer.
 *
 * This function partitions the value into high and low parts, for loading the
 * MATCH and PMR.
 */
void TimerPWMMatchSet16(uint32_t ui32Base, uint32_t ui32Timer, uint32_t ui32Value);

#endif /* TIMER_PWM_H */
