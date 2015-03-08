#ifndef ESC_H_
#define ESC_H_

#include <stdint.h>

#define ESC_MIN_VALUE 0x00
#define ESC_MAX_VALUE 0xFFFF

void esc_Init(void);

// chx is read as a percentage, with the maximum being 0xFFFF
void esc_SetValues(uint32_t ch0, uint32_t ch1, uint32_t ch2, uint32_t ch3);

#endif /* ESC_H_ */
