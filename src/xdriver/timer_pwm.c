/**
 * Functions for using timers as PWM.
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"

#include "timer_pwm.h"

void TimerPWMLoadSet16(uint32_t ui32Base, uint32_t ui32Timer, uint32_t ui32Value)
{
    //
    // Set the timer A load value if requested.
    //
    if(ui32Timer & TIMER_A)
    {
        HWREG(ui32Base + TIMER_O_TAILR) = ui32Value & 0xFFFF;
        HWREG(ui32Base + TIMER_O_TAPR) = ui32Value >> 16;
    }

    //
    // Set the timer B load value if requested.
    //
    if(ui32Timer & TIMER_B)
    {
        HWREG(ui32Base + TIMER_O_TBILR) = ui32Value & 0xFFFF;
        HWREG(ui32Base + TIMER_O_TBPR) = ui32Value >> 16;
    }
}

void TimerPWMMatchSet16(uint32_t ui32Base, uint32_t ui32Timer, uint32_t ui32Value)
{
    if(ui32Timer & TIMER_A)
    {
        HWREG(ui32Base + TIMER_O_TAMATCHR) = ui32Value & 0xFFFF;
        HWREG(ui32Base + TIMER_O_TAPMR) = ui32Value >> 16;
    }

    //
    // Set the timer B match value if requested.
    //
    if(ui32Timer & TIMER_B)
    {
        HWREG(ui32Base + TIMER_O_TBMATCHR) = ui32Value & 0xFFFF;
        HWREG(ui32Base + TIMER_O_TBPMR) = ui32Value >> 16;
    }
}

