#ifndef _RTI_H
#define _RTI_H

#include <stdint.h>
#include "driverlib/systick.h"

#define RTI_FREQ (1000) // Hz.
// In order to change this value, RTI_PRESCALER in rti.c must be changed and the file recompiled.
// Note that divider base also affects rti divider (check tables 2-7 and 2-8 of MC9S12(...).pdf)
#define RTI_PER (1.0/RTI_FREQ) // seconds

typedef uint32_t rti_time; // A type for expressing time, each rti_time equals one RTI_PER

#define RTI_MS_TO_TICKS(ms) ((((uint32_t)ms) * RTI_FREQ) / 1000) // Converts miliseconds to rti_time - With RTI_FREQ = 1000, the macro does nothing.

#if RTI_FREQ == (1000)
#define RTI_TICKS_TO_MS(ticks) ((uint32_t)ticks)
#endif

/**
 * Get the current time in ms.
 */
#define rti_GetTimeMs() RTI_TICKS_TO_MS(SysTickValueGet())
																 // If (ms) is set in compile time, this macro consumes no extra processor time.
typedef int32_t rti_id; // An id for a registered callback

typedef void (*rti_ptr) (void *data, rti_time period, rti_id id); // A function callback for registering in the RTI

void rti_Init(void);
// Initializes the RTI module. This requires no other modules to work, and doesn't require interrupts to be enabled.

rti_id rti_Register(rti_ptr callback, void *data, rti_time period, rti_time delay);
// Registers a callback function to be called periodically every period*RTI_PER seconds, after an initial delay of delay*RTI_PER seconds.
// period and delay can be set using RTI_MS_TO_TICKS(timeInMiliseconds).
// If RTI_FREQ is 1000, there is no need of using the macro, as the parameter is already in ms.
// When callback is called, it receives data, period and its rti_id.
// callback is called with interrupts inhibited and MUST NOT disinhibit them.
// Returns the rti_id of the registed callback.

#define RTI_ALWAYS 1 // period for a function that will always be called (its frequency is RTI_FREQ)
#define RTI_ONCE 0 // period for a function that will only be called once after a certain delay
#define RTI_NOW 1 // delay for a function that will be called for the first time as soon as the RTI interrupts the CPU

void rti_SetPeriod(rti_id id, rti_time period);
// Changes the period of a registered rti_id

void rti_Cancel(rti_id n);
// Cancels a registered rti_id

#endif
