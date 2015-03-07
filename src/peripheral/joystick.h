#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>

typedef struct
{
	int32_t roll;
	int32_t pitch;
	int32_t yaw;
	uint32_t elev;
} joy_data_t;

volatile extern joy_data_t joy_data;

void InitJoystick(void);

#endif /* JOYSTICK_H_ */
