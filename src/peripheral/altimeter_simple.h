#ifndef ALTIMETER_SIMPLE_H_
#define ALTIMETER_SIMPLE_H_

#include <stdint.h>
#include <stdbool.h>

#include "../xdriver/iic_interface.h"

#define ALTIMETER_MODULE_NUM 1
#define ALTIMETER_ADDRESS 0x60

typedef void (*eot_t)(int32_t meters);
extern volatile bool altimeter_meas_ready;
extern volatile int32_t meas;

void altimeter_Init(void);

void altimeter_CommenceMeasurement(void);
void altimeter_Measure(eot_t eot, iic_userAction commFailedCB); // eot recibe un int32_t con la medicion de altura

#endif /* ALTIMETER_SIMPLE_H_ */
