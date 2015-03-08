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

#define N_TONES 12

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
	uint16_t periods[N_TONES];
	uint16_t durations[N_TONES];
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
	static int freqs16[N_TONES] = {
	/*start with 220 x 16*/	3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274,
			5588, 5920, 6272, 6645 /* end with 415.3 x 16*/};
	int i;

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

	for (i = 0; i < ARSIZE(freqs16); i++) {
		Buzzer_Tables[i] = (clk_freq*16)/freqs16[i];
	}
}

void buzzer_play_note(int period, int semiperiod)
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
