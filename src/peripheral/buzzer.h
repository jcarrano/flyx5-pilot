/**
 * Simple buzzer interface.
 *
 * @author	Juan I Carrano.
 */

#ifndef BUZZER_H
#define BUZZER_H

void buzzer_init();

void buzzer_play_note(unsigned int period, unsigned int semiperiod);

#endif /* BUZZER_H */
