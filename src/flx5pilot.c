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

#include <stdbool.h>

#include "common.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "xdriver/rti.h"
#include "xdriver/sbutton.h"

#include "flyx5_hw.h"
#include "hw_init.h"

#include "debug_tools/stdio_simple.h"
#include "Misc/error.h"
#include "Misc/music.h"

#include "peripheral/buzzer.h"
#include "peripheral/dmu_simple.h"
#include "peripheral/joystick.h"
#include "peripheral/usound.h"
#include "peripheral/esc.h"

#include "control/nlcf.h"
#include "control/multirotor_control.h"

#include "quad_setup.h"

#include "utils/uartstdio.h"

#include "debug_tools/debug.h"

/**
 * This error routine that is called if the driver library encounters an error.
 */
#ifdef DEBUG
void __error__(char *filename, uint32_t line)
{
    // FIXME: unimplemented
}
#endif

/* Messages **/
#define ENDL "\r\n"
#define HELLO_TXT "This is FLYX5!"ENDL
#define CLK_TXT "Clock speed is: "

#define ARM_HOLD_TIME_MS 3000

typedef enum GOTO_MODE {GOTO_IDLE, GOTO_IMU_CAL, GOTO_FLY} program_mode;

void uart_init();
void init_peripherals();

program_mode flight_control();
program_mode imu_calibration();
program_mode idle_process();
void esc_calibration();

struct nlcf_state Estimator_State;

int main(void)
{
    program_mode pmode = GOTO_IDLE;

    init_failsafe();

    /* **************************** initialization ************************** */

    init_clock();

    /* Initialise all ports
     * For now, each peripheral is initialising its own gpio.
     */
    init_all_gpio();


    if (!running_under_debugger()) {
            SysCtlDelay(1000000);
            init_jtag_muxed_pins();
    }

    uart_init();
    buzzer_init();

    buzzer_load_score(music_startup);

    _puts(HELLO_TXT);
    _puts(CLK_TXT);
    _puti(R_(SysCtlClockGet)());
    _puts(ENDL);

    init_peripherals();

    _puts("Peripherals done.\n\r");

    /* ************************** end initialization ************************ */

    esc_calibration();

    nlcf_init(&Estimator_State);

    while (1) {
        switch (pmode) {
        case GOTO_IDLE:
            pmode = idle_process();
            break;
        case GOTO_IMU_CAL:
            pmode = imu_calibration();
            break;
        case GOTO_FLY:
            pmode = flight_control();
            break;
        default:
            err_Throw("Unknown program mode");
            break;
        }
    }

    return 0;
}

void esc_calibration()
{
    if (!running_under_debugger() && PIN_ACTIVE(BUTTON_1)) {
        esc_SetValues(ESC_MAX_VALUE,ESC_MAX_VALUE,ESC_MAX_VALUE,ESC_MAX_VALUE);
        esc_EnableOutput();

        buzzer_load_score(music_calibration_escs_enter);

        while (PIN_ACTIVE(BUTTON_1))
            ;

        buzzer_load_score(music_calibration_escs_step);
        esc_SetValues(ESC_MIN_VALUE,ESC_MIN_VALUE,ESC_MIN_VALUE,ESC_MIN_VALUE);
    }
}

static bool joystick_check_arm(joy_data_t joy)
{
    const int32_t low_thres = (INT16_MIN / 20) * 19;
    const uint32_t low_thres_u = (UINT16_MAX / 20);

    return joy.roll < low_thres && joy.pitch < low_thres &&
            joy.yaw > -low_thres && joy.elev < low_thres_u;
}

program_mode idle_process()
{
    program_mode destination;
    sbutton arm_button;

    esc_SetValues(ESC_MIN_VALUE,ESC_MIN_VALUE,ESC_MIN_VALUE,ESC_MIN_VALUE);
    esc_EnableOutput();

    sbutton_init(&arm_button);

    while(1)
    {
        struct dmu_samples_T imu_samples;

    	if(dmu_PumpEvents(&imu_samples))
    	{
            joy_data_t joystick_snapshot;

			nlcf_process(&Estimator_State, imu_samples.gyro, imu_samples.accel, NULL);

            IntMasterDisable();
            joystick_snapshot = joy_data;
            IntMasterEnable();

            if (s_hold(&arm_button, ARM_HOLD_TIME_MS, joystick_check_arm(joystick_snapshot))) {
                destination = GOTO_FLY;
                break;
            }
    	}

        if (PIN_ACTIVE(BUTTON_2)) {
            destination = GOTO_IMU_CAL;
            break;
        }
    }

    return destination;
}

program_mode imu_calibration()
{
    struct dmu_samples_T imu_samples;

    esc_SetValues(ESC_MIN_VALUE,ESC_MIN_VALUE,ESC_MIN_VALUE,ESC_MIN_VALUE);

    buzzer_load_score(music_enter_calibration);

    nlcf_init(&Estimator_State);
    bool calibrationMode = qset_TryDmuCalibration(false, &Estimator_State);

    while(calibrationMode)
    {
    	if(dmu_PumpEvents(&imu_samples))
    	{
			nlcf_process(&Estimator_State, imu_samples.gyro, imu_samples.accel, NULL);

			calibrationMode = qset_TryDmuCalibration(calibrationMode, &Estimator_State);
    	}
    }

    return GOTO_IDLE;
}

static void _esc_set(frac motor_thrusts[4])
{
#define _t2t(i) (((uint32_t)(motor_thrusts[i].v))*2)
	esc_SetValues(_t2t(0), _t2t(1),_t2t(2),_t2t(3));
#undef _t2t
}

program_mode flight_control()
{
	sbutton arm_button;
    struct att_ctrl_state controller_state;

    buzzer_load_score(music_armed);

    sbutton_init(&arm_button);
    att_ctrl_init(&controller_state, &ctrl_default_params);

    while(1)
    {
        struct dmu_samples_T imu_samples;
    	joy_data_t joystick_snapshot;

    	if(dmu_PumpEvents(&imu_samples))
    	{
            multirotor_setpoint setpoint;
            vec3 angle_rate;
            vec3 torques;
            frac motor_thrusts[4];

			nlcf_process(&Estimator_State, imu_samples.gyro, imu_samples.accel, &angle_rate);

            IntMasterDisable();
            joystick_snapshot = joy_data;
            IntMasterEnable();
            setpoint = joystick_to_setpoint(joystick_snapshot);

            torques = att_ctrl_step(&controller_state, setpoint.attitude, dq_to_q(Estimator_State.q), angle_rate);

            control_mixer4(setpoint.altitude, torques, motor_thrusts);

            _esc_set(motor_thrusts);

            if (s_hold(&arm_button, ARM_HOLD_TIME_MS, joystick_check_arm(joystick_snapshot))) {
				break;
            }
    	}
    }

    esc_SetValues(ESC_MIN_VALUE,ESC_MIN_VALUE,ESC_MIN_VALUE,ESC_MIN_VALUE);
    buzzer_load_score(music_unarmed);

    return GOTO_IDLE;
}

void uart_init(void)
{
    R_(SysCtlPeripheralEnable)(GPIO_PERIPH(DEBUG_RX));
    ENABLE_AND_RESET(UART_DEBUG);

    CFG_PIN(DEBUG_RX);
    CFG_PIN(DEBUG_TX);

    R_(UARTClockSourceSet)(BASE_PERIPH(UART_DEBUG), UART_CLOCK_SYSTEM);

    R_(UARTConfigSetExpClk)(BASE_PERIPH(UART_DEBUG) , R_(SysCtlClockGet)(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, R_(SysCtlClockGet)());
}

void init_peripherals()
{
	err_Init(NULL, _puts, NULL);
	rti_Init();
    dmu_Init();
    joy_Init();
    esc_Init();
    //usound_Init();
}


