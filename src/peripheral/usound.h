#ifndef USOUND_H_
#define USOUND_H_

#include <stdint.h>
#include <stdbool.h>

void usound_Init (void);

extern volatile bool usound_meas_available;
extern volatile int32_t usound_meas;

#endif /* USOUND_H_ */
