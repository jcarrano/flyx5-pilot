#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	int32_t roll;
	int32_t pitch;
	int32_t yaw;
	uint32_t elev;
} joy_data_t;

volatile extern joy_data_t joy_data;

volatile extern bool joy_no_signal;

void joy_Init(void);

#endif /* JOYSTICK_H_ */
