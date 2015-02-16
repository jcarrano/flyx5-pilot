/**
 * Simple buzzer interface.
 *
 * @author	Juan I Carrano.
 */

#include "../common.h"
#include "../flyx5_hw.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "../xdriver/timer_pwm.h"

#define BUZZER_LO_FREQ_CODE ('a')
#define BUZZER_HI_FREQ_CODE ('z')
#define BUZZER_N_FREQS (BUZZER_HI_FREQ_CODE - BUZZER_LO_FREQ_CODE + 1)

#define BUZZER_FREQ_LO 110 /* Hz */

#define BUZZER_LO_DUR_CODE ('A')
#define BUZZER_HI_DUR_CODE ('Z')
#define BUZZER_N_DURS (BUZZER_HI_DUR_CODE - BUZZER_LO_DUR_CODE + 1)

#define BUZZER_DUR_HI 8 /* seconds */

struct buzzer_data {
	char *score;
	int curr_time;
};

/**
 * This global variable contains the values needed to generate frequencies and
 * durations.
 */
struct {
	uint16_t period[BUZZER_N_FREQS];
	uint16_t durations[BUZZER_N_DURS];
} Buzzer_Tables;

/**
 * Global buzzer data.
 */
struct buzzer_data Buzzer;

/**
 * Initialise the buzzer driver.
 *
 * The system clock must be already initialised in order to correctly generate
 * the frequency tables.
 */
void buzzer_init()
{
	uint32_t ckl_freq = R_(SysCtlClockGet)();
	uint32_t sequencer_prescaler = (ckl_freq >> 16) * BUZZER_DUR_HI;

	ENABLE_AND_RESET(TIMER_BUZZER);

	CFG_PIN(BUZZER);

	R_(TimerClockSourceSet)(BASE_PERIPH(TIMER_BUZZER), TIMER_CLOCK_SYSTEM);

	R_(TimerConfigure)(BASE_PERIPH(TIMER_BUZZER),
			  TIMER_CFG_SPLIT_PAIR
			| TIMER_CFG(TIMER_BUZZER_SYN, _PWM)
			| TIMER_CFG(TIMER_BUZZER_SEQ, _ONE_SHOT)
			);

	R_(TimerPrescaleSet)(BASE_PERIPH(TIMER_BUZZER),
				sTIMER(TIMER_BUZZER_SEQ), sequencer_prescaler);
}

void buzzer_play_note(char note, )
{


	TimerPWMLoadSet16(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SYN),
							period);
	TimerPWMMatchSet16(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SYN),
							semiperiod);
	R_(TimerEnable)(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SYN));
}

void buzzer_stop_note()
{
	R_(TimerDisable)(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SYN));
}
