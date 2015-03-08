#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>
#include <stdbool.h>
#include "../control/joystick_map.h"

volatile extern joy_data_t joy_data;

volatile extern bool joy_no_signal;

void joy_Init(void);

#endif /* JOYSTICK_H_ */
