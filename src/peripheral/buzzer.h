/**
 * Simple buzzer interface.
 *
 * @author	Juan I Carrano.
 */

#ifndef BUZZER_H
#define BUZZER_H

#include "stdint.h"

#define BUZZER_DUR_12s 0
#define BUZZER_DUR_8s 1
#define NOTE_DURATION_DOT(x) ((__builtin_ctz(x) + 1)*2)
#define NOTE_DURATION(x) (NOTE_DURATION_DOT(x) + 1)

#define VELOCITY_SILENCE 0
#define DURATION_REPEAT (0xFF)
#define DURATION_STOP (0xFF-1)

struct note_data {
	uint8_t midi_number;
	uint8_t velocity;
	uint8_t duration;
};

#define SCORE_REPEAT {0, 0, DURATION_REPEAT}
/**
 * Pseudo-note to instruct the buzzer to rewind and repeat the sequence.
 */
static const struct note_data score_repeat = SCORE_REPEAT;

#define SCORE_STOP {0, 0, DURATION_STOP}
/**
 * Pseudo-note to instruct the buzzer to stop.
 */
static const struct note_data score_stop = SCORE_STOP;

/**
 * Initialise the buzzer driver.
 *
 * The system clock must be already initialised in order to correctly generate
 * the frequency tables.
 */
void buzzer_init();

/**
 * Load a score.
 *
 * The score MUST end either with score_repeat or score_stop.
 */
void buzzer_load_score(struct note_data *score);

/**
 * Stop sound.
 */
void buzzer_stop();

#endif /* BUZZER_H */
