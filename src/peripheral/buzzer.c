/**
 * Simple buzzer interface.
 *
 * @author	Juan I Carrano.
 */

#include "../common.h"
#include "../flyx5_hw.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "../xdriver/timer_pwm.h"

#include "buzzer.h"

#define N_TONES 12

struct buzzer_data {
	struct note_data *score;
	int curr_time;
	int sw_scaler;
};

/**
 * This global variable contains the values needed to generate frequencies and
 * durations.
 */
struct {
	uint32_t periods[N_TONES];
	uint32_t duration8s;
} Buzzer_Tables;

/**
 * Global buzzer data.
 */
struct buzzer_data Buzzer;

/**
 * Sequencer ISR.
 */
static void _sequencer();

void buzzer_init()
{
	uint32_t clk_freq = R_(SysCtlClockGet)();
	uint32_t sequencer_prescaler = 255;
	static unsigned int freqs_midi9[N_TONES] = {
	/*start with 440/32 (midi number 9), multiplied by 16*/
		440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831};
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

	for (i = 0; i < ARSIZE(freqs_midi9); i++) {
		Buzzer_Tables.periods[i] = (clk_freq*32)/freqs_midi9[i];
	}

	Buzzer_Tables.duration8s = clk_freq * 8 / sequencer_prescaler;

	Buzzer.curr_time = 0;
	Buzzer.score = NULL;
	Buzzer.sw_scaler = 0;

	TimerIntRegister(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SEQ), _sequencer);
	TimerIntEnable(BASE_PERIPH(TIMER_BUZZER), GLUE3(TIMER_TIM, TIMER_BUZZER_SEQ, _TIMEOUT));
}

/**
 * Convert from midi number to a PWM period.
 */
static uint32_t midi2period(uint8_t midi_number)
{
	int table_index = (midi_number - 9) % N_TONES;
	int divider = (midi_number - 9) / N_TONES;

	return Buzzer_Tables.periods[table_index] >> divider;
}

/**
 * Convert from midi velocity (intensity) to a duty duration.
 */
static uint32_t velocity2duty(uint8_t velocity, uint32_t period)
{
	return (period >> 8)*velocity;
}

struct period_combi{
	uint16_t hw_counter;
	uint16_t sw_prescaler;
};

/**
 * Convert from duration code (intensity) to a real duration.
 */
static struct period_combi duration2ticks(uint8_t dcode)
{
	struct period_combi r;

	uint32_t period = Buzzer_Tables.duration8s >> (dcode/2);
	if (!(dcode % 2))
		period += period / 2;

	if (period > UINT16_MAX) {
		int divisor = DIV_CEIL(period, UINT16_MAX);
		r.hw_counter = period / divisor;
		r.sw_prescaler = divisor - 1;
	} else {
		r.sw_prescaler = 0;
		r.hw_counter = period;
	}

	return r;
}

/**
 *  Start the synthesis timer.
 */
static void _play_note_now(uint32_t period, uint32_t semiperiod)
{
	TimerPWMLoadSet16(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SYN),
							period);
	TimerPWMMatchSet16(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SYN),
							semiperiod);
	R_(TimerEnable)(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SYN));
}

/**
 *  Stop the synthesis timer.
 */
static void _stop_note_now()
{
	R_(TimerDisable)(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SYN));
}

#define TIMER_ALL_INTERRUPTS (TIMER_TIMB_DMA | TIMER_TIMA_DMA | \
		TIMER_CAPB_EVENT | TIMER_CAPB_MATCH | TIMER_TIMB_TIMEOUT | \
		TIMER_RTC_MATCH | TIMER_CAPA_EVENT | TIMER_CAPA_MATCH | TIMER_TIMA_TIMEOUT)

static void _sequencer()
{
	if (Buzzer.sw_scaler) {
		R_(TimerIntClear)(BASE_PERIPH(TIMER_BUZZER), TIMER_ALL_INTERRUPTS);
		R_(TimerEnable)(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SEQ));
		Buzzer.sw_scaler--;
	} else {
		struct note_data curr_note = Buzzer.score[Buzzer.curr_time];

		if (curr_note.duration == DURATION_STOP) {
			R_(TimerIntClear)(BASE_PERIPH(TIMER_BUZZER), TIMER_ALL_INTERRUPTS);
			_stop_note_now();
		} else if (curr_note.duration == DURATION_REPEAT) {
			Buzzer.curr_time = 0;
		} else {
			uint32_t period = midi2period(curr_note.midi_number);
			uint32_t duty = velocity2duty(curr_note.velocity, period);
			struct period_combi pc = duration2ticks(curr_note.duration);

			R_(TimerIntClear)(BASE_PERIPH(TIMER_BUZZER), TIMER_ALL_INTERRUPTS);
			_stop_note_now();

			if (duty > 0)
				_play_note_now(period, duty);

			Buzzer.sw_scaler = pc.sw_prescaler;
			R_(TimerLoadSet)(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SEQ),
								pc.hw_counter);
			R_(TimerEnable)(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SEQ));

			Buzzer.curr_time++;
		}
	}
}

/**
 * Load a score.
 *
 * The score MUST end either with score_repeat or score_stop.
 */
void buzzer_load_score(struct note_data *score)
{
	buzzer_stop();

	Buzzer.score = score;
	Buzzer.curr_time = 0;
	Buzzer.sw_scaler = 0;

	_sequencer();
}

/**
 * Stop sound.
 */
void buzzer_stop()
{
	bool ints_were_disabled = IntMasterDisable();

	R_(TimerDisable)(BASE_PERIPH(TIMER_BUZZER), sTIMER(TIMER_BUZZER_SEQ));

	_stop_note_now();
	Buzzer.score = NULL;

	if (ints_were_disabled == false)
	{
		IntMasterEnable();
	}
}
