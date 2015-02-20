#include "altimeter_simple.h"
#include "altimeter_definitions.h"
#include "../xdriver/gpio_interface.h"
#include <stdbool.h>

iic_commData_T * altimeter_commDataPtr = &(iic_commData[ALTIMETER_MODULE_NUM]);

volatile bool altimeter_meas_ready;
volatile int32_t meas;

volatile bool altimeter_init = false;
int altimeter_init_stage = 0;

uint8_t altimeter_read_buffer[5];

volatile eot_t eot;
volatile iic_userAction comm_failed;

void altimeter_StagesInit(void);
void altimeter_InitFailed(void);
void altimeter_ReportResults(void);
void altimeterINT1ISR(void);

#define ALTIMETER_SEND(eotCB, failedCB, size, buffer) 	(iic_Send (ALTIMETER_MODULE_NUM, ALTIMETER_ADDRESS, eotCB, failedCB, size, buffer))
#define ALTIMETER_RECEIVE_FROM_REG(regAddress, eotCB, failedCB, size, buffer)	(iic_ReceiveFromRegister (ALTIMETER_MODULE_NUM, regAddress, ALTIMETER_ADDRESS, eotCB, failedCB, size, buffer))

void altimeter_Init(void)
{
	if (altimeter_init == true)
		return;

	altimeter_meas_ready = false;

	gpio_Init(GPIO_E, GPIO_PIN_5, GPIO_RISING_EDGE);

	iic_Init(ALTIMETER_MODULE_NUM);

	altimeter_StagesInit();

	while (altimeter_init == false)
		;

	return;
}

void altimeter_StagesInit(void)
{
	switch (altimeter_init_stage)
	{
		case 0:
			altimeter_commDataPtr->data[0] = ADD_CTRL_REG1;
			// Altitude measurements, full oversampling
			altimeter_commDataPtr->data[1] = CTRL_REG1_OS(0);

			ALTIMETER_SEND(altimeter_StagesInit, altimeter_InitFailed, 2, NULL);

			break;

		case 1:
			altimeter_commDataPtr->data[0] = ADD_CTRL_REG3;
			// Both interrupts are set active high, open drain
			altimeter_commDataPtr->data[1] = CTRL_REG3_IPOL1 | CTRL_REG3_IPOL2 | CTRL_REG3_PP_OD2;

			ALTIMETER_SEND(altimeter_StagesInit, altimeter_InitFailed, 2, NULL);

			break;

		case 2:
			altimeter_commDataPtr->data[0] = ADD_PT_DATA_CFG;
			// Only generate events on new Altitude data
			altimeter_commDataPtr->data[1] = PT_DATA_CGF_PDEFE;

			ALTIMETER_SEND(altimeter_StagesInit, altimeter_InitFailed, 2, NULL);

			break;

		case 3:
			altimeter_commDataPtr->data[0] = ADD_CTRL_REG4;
			// Interrupt on data ready
			altimeter_commDataPtr->data[1] = CTRL_REG4_INT_EN_DRDY;

			ALTIMETER_SEND(altimeter_StagesInit, altimeter_InitFailed, 2, NULL);

			break;

		case 4:
			altimeter_commDataPtr->data[0] = ADD_CTRL_REG5;
			// Interrupt on data ready
			altimeter_commDataPtr->data[1] = CTRL_REG5_INT_CFG_DRDY;

			ALTIMETER_SEND(altimeter_StagesInit, altimeter_InitFailed, 2, NULL);

			break;

		case 5:
			altimeter_init = true;

			break;

		default:
			break;
	}

	altimeter_init_stage += 1;

	return;
}

void altimeter_CommenceMeasurement(void)
{
	altimeter_commDataPtr->data[0] = ADD_CTRL_REG1;
	// Altitude measurements, full oversampling, start one shot
	altimeter_commDataPtr->data[1] = CTRL_REG1_OS(0) | CTRL_REG1_OST;

	ALTIMETER_SEND(NULL, NULL, 2, NULL);
}

void altimeter_Measure(eot_t eotCB, iic_userAction commFailedCB) // eot recibe un int32_t con la medicion de altura
{
	eot = eotCB;
	comm_failed = commFailedCB;

	ALTIMETER_RECEIVE_FROM_REG(ADD_OUT_P_MSB, altimeter_ReportResults, commFailedCB, sizeof(altimeter_read_buffer), &(altimeter_read_buffer[0]));
}
#include "../debug_tools/debug.h"

void altimeter_ReportResults(void)
{
	Putchar('f');
	/*
	int32_t altitude_meters = (int32_t) ( ((((uint32_t) altimeter_read_buffer[0]) << 12) & 0xFF000)
										| ((((uint32_t) altimeter_read_buffer[1]) << 4) & 0x00FF0)
										| ((((uint32_t) altimeter_read_buffer[2]) >> 4) & 0x0000F)
										);
										*/

	int32_t altitude_meters = (int32_t) ( ((((uint32_t) altimeter_read_buffer[0]) << 18))
										| ((((uint32_t) altimeter_read_buffer[1]) << 8))
										| ((((uint32_t) altimeter_read_buffer[2]) >> 0))
										);
	Putchar('p');
	if (eot != NULL)
	{
		Putchar('n');
		altimeter_meas_ready = true;
		meas = altitude_meters;
	}
	else
	{
		Putchar('u');
	}
}

void altimeterINT1ISR(void)
{
	altimeter_meas_ready = true;
}

void altimeter_InitFailed(void)
{
#ifdef ALTIMETER_DEBUG_INIT
	Putchar('i');Putchar('f');Putchar('a');Putchar('i');Putchar('l');

#endif

	while(1);

}
