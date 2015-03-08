/**
 * @file
 * @author Juan I. Carrano
 * @author Juan I. Ubeira
 * @copyright	Copyright 2013 by Juan I Carrano, Andrés Calcabrini, Juan I. Ubeira and
 * 		Nicolás Venturo. All rights reserved. \n
 * 		\n
 *		This program is free software: you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation, either version 3 of the License, or
 *		(at your option) any later version. \n
 * 		\n
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details. \n
 * 		\n
 *		You should have received a copy of the GNU General Public License
 *		along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Entry point for the flyx5 firmware.
 */

#include "common.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "xdriver/rti.h"

#include "flyx5_hw.h"
#include "hw_init.h"

#include "debug_tools/stdio_simple.h"
#include "Misc/error.h"

#include "peripheral/buzzer.h"
#include "peripheral/dmu_simple.h"
#include "peripheral/joystick.h"
#include "peripheral/usound.h"

#include "control/nlcf.h"

#include "quad_setup.h"

/**
 * This error routine that is called if the driver library encounters an error.
 */
#ifdef DEBUG
void __error__(char *filename, uint32_t line)
{
    // FIXME: unimplemented
}
#endif


#include "utils/uartstdio.h"
void
ConfigureUART2(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}


void init_peripherals();


int main(void)
{
    struct dmu_samples_T dmuSamples;
    struct nlcf_state state;
    bool esc_calibration_mode = false;

    init_failsafe();

    init_clock();

    //init_pins();
    /* Initialise all ports */
    //init_all_gpio();
/*
    if (!running_under_debugger()) {
            //SysCtlDelay(10000000);
            init_jtag_muxed_pins();
    }
*/


    /* Initialize port */
    /*
    ENABLE_AND_RESET(UART_DEBUG);

    CFG_PIN(DEBUG_RX);
    CFG_PIN(DEBUG_TX);

    R_(UARTConfigSetExpClk)(BASE_PERIPH(UART_DEBUG) , R_(SysCtlClockGet)(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
*/
    ConfigureUART2();


#define HELLO_TXT "This is FLYX5!\r\n"
    UARTStringPut(BASE_PERIPH(UART_DEBUG), HELLO_TXT);
#define CLK_TXT "Clock speed is: "
#define ENDL "\r\n"
    UARTStringPut(BASE_PERIPH(UART_DEBUG), CLK_TXT);
    UARTIntPut(BASE_PERIPH(UART_DEBUG), R_(SysCtlClockGet)());
    UARTStringPut(BASE_PERIPH(UART_DEBUG), ENDL);

    //buzzer_play_note(16667, 16667/2);

    init_peripherals();

    _puts("Peripherals done.\n\r");

    nlcf_init(&state);


    //
    // Loop forever.
    //
    bool calibrationMode = false;

    while(1)
    {

    	if(dmu_PumpEvents(&dmuSamples))
    	{
			nlcf_process(&state, dmuSamples.gyro, dmuSamples.accel, NULL);

			calibrationMode = qset_TryDmuCalibration(calibrationMode, &state);
			//quat q_est = dq_to_q(state.q);
    	}

    }

}

void init_peripherals()
{
    //buzzer_init();
    dmu_Init();
    //joy_Init();
    //usound_Init();

    rti_Init();
	err_Init(NULL, _puts, NULL);
}


