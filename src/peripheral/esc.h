#ifndef ESC_H_
#define ESC_H_

#include <stdint.h>
#include <fixed_point/types.h>

static const frac ESC_MIN_VALUE = {0};
static const frac ESC_MAX_VALUE = {FRAC_MAX_V};

/**
 * Initialize the ESC control subsystem.
 *
 * After initialization, the output is disabled.
 */
void esc_Init(void);

/**
 * Set the motor thrusts.
 *
 * DO NOT USE NEGATIVE VALUES!!!.
 */
void esc_SetThrust(frac ch0, frac ch1, frac ch2, frac ch3);

/**
 * Enable the PWM outputs.
 */
void esc_EnableOutput(void);

/**
 * Set all output to the minumum.
 */
#define	esc_ToMinimum() esc_SetThrust(ESC_MIN_VALUE, ESC_MIN_VALUE, ESC_MIN_VALUE, ESC_MIN_VALUE)

/**
 * Set all output to the maximum.
 */
#define	esc_ToMaximum() esc_SetThrust(ESC_MAX_VALUE, ESC_MAX_VALUE, ESC_MAX_VALUE, ESC_MAX_VALUE)

#endif /* ESC_H_ */
