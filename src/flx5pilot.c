/**
 * Entry point for the flyx5 firmware.
 *
 * @author Juan I. Carrano
 *
 * @copyright Copyright 2014 Juan I Carrano, Andrés Calcabrini, Juan I Ubeira,
 *              Nicolás venturo.
 */

#include "common.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
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
    	joy_data_t joyData;
    	quat q_setp;

    	if(dmu_PumpEvents(&dmuSamples))
    	{
			nlcf_process(&state, dmuSamples.gyro, dmuSamples.accel, NULL);

			calibrationMode = qset_TryDmuCalibration(calibrationMode, &state);

			quat q_est = dq_to_q(state.q);
/*
			_puts("\x0E\x0C");
			UARTputraw16(q_est.r.v);
			UARTputraw16(q_est.v.x.v);
			UARTputraw16(q_est.v.y.v);
			UARTputraw16(q_est.v.z.v);
			*/
    	}

    	SysCtlDelay(8000000 / 3);

    	IntMasterDisable();

    	joyData = joy_data;

    	IntMasterEnable();

    	q_setp = joystick_to_setpoint(joyData).attitude;

		_puts("\x0E\x0C");
		UARTputraw16(q_setp.r.v);
		UARTputraw16(q_setp.v.x.v);
		UARTputraw16(q_setp.v.y.v);
		UARTputraw16(q_setp.v.z.v);

    	//UARTprintf("%u %d %d %d \n\r", joyData.elev, joyData.pitch, joyData.roll, joyData.yaw);
    }

}

void init_peripherals()
{
	err_Init(NULL, _puts, NULL);
    //buzzer_init();
    //dmu_Init();
    joy_Init();
    //usound_Init();

    rti_Init();

}


