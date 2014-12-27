/**
 * Entry point for the flyx5 firmware.
 *
 * @author Juan I. Carrano
 *
 * @copyright Copyright 2014 Juan I Carrano, Andrés Calcabrini, Juan I Ubeira,
 *              Nicolás venturo.
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"

/**
 * This error routine that is called if the driver library encounters an error.
 */
#ifdef DEBUG
void __error__(char *filename, uint32_t line)
{
    // FIXME: unimplemented
}
#endif


int main(void)
{
    //
    // Enable the GPIO module.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(1);

    //
    // Configure PA1 as an output.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_1);

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Set the GPIO high.
        //
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_1, GPIO_PIN_1);

        //
        // Delay for a while.
        //
        SysCtlDelay(1000000);

        //
        // Set the GPIO low.
        //
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_1, 0);

        //
        // Delay for a while.
        //
        SysCtlDelay(1000000);
    }
}
