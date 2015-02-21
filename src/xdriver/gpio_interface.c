#include "gpio_interface.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include <inc/hw_ints.h>
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"


typedef void (*gpio_ptr)(void);

typedef struct
{
	int32_t GPIO_INT_PIN;
	gpio_ptr action;

}gpio_PinAction;


/*
 * Task Definitions: write Pin - Action pairs here by priority.
 * Write function real name in define.
 */

//#define gpioA_pin0Action main_samplesReady

#define gpioE_pin3Action dmu_SamplesPending
#define gpioE_pin5Action altimeterINT1ISR

extern void gpioA_pin0Action(void);
extern void gpioE_pin3Action(void);
extern void gpioE_pin5Action(void);

const gpio_PinAction GPIOA_PIN_ACTIONS[0];


const gpio_PinAction GPIOE_PIN_ACTIONS[] = {
	{
		GPIO_PIN_3,
		gpioE_pin3Action
	},
	{
		GPIO_PIN_5,
		gpioE_pin5Action
	}
};

// Constants and Macros
const uint32_t GPIO_BASES[] = {GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTC_BASE, GPIO_PORTD_BASE, GPIO_PORTE_BASE};
//#define GPIO_BASE(x) (GPIO_BASES[x])

const uint32_t GPIO_INTS[] = {INT_GPIOA, INT_GPIOB, INT_GPIOC, INT_GPIOD, INT_GPIOE};
#define GPIO_INT(x) (GPIO_INTS[x])

// Internal Symbols
void gpioA_InterruptHandler(void);
void gpio_ExecuteActions(uint32_t gpioBase, const gpio_PinAction pinActions[], uint8_t activeActions);

void gpioE_InterruptHandler(void);


void gpio_Init(uint8_t gpioNumber, uint32_t gpioPins, uint32_t gpioIntType)
{
	uint32_t gpioBase = GPIO_BASE(gpioNumber);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA + gpioNumber);
	GPIOPinTypeGPIOInput(gpioBase, gpioPins);

	GPIOIntTypeSet(gpioBase, gpioPins, gpioIntType);

	GPIOIntEnable(gpioBase, gpioPins);
	IntEnable(GPIO_INT(gpioNumber));
	IntMasterEnable();

	return;
}

void gpioA_InterruptHandler(void)
{
	gpio_ExecuteActions(GPIO_PORTA_BASE, GPIOA_PIN_ACTIONS, sizeof(GPIOA_PIN_ACTIONS) / (sizeof(uint32_t) + sizeof(gpio_ptr)));
	return;
}

void gpioE_InterruptHandler(void)
{
	gpio_ExecuteActions(GPIO_PORTE_BASE, GPIOE_PIN_ACTIONS, sizeof(GPIOE_PIN_ACTIONS) / (sizeof(uint32_t) + sizeof(gpio_ptr)));
	return;
}


/*
void gpio_dExecuteActions(int8_t gpioNumber)
{
	uint32_t gpioBase = GPIO_BASE(gpioNumber);

	for(int8_t i = 0; i < GPIO_PINS; i++)
	{
		uint32_t gpioPin = (GPIO_PIN_0 << i)
		if(GPIOIntStatus(gpioBase, false) & gpioPin)
		{
			GPIOIntClear(gpioBase, gpioPin);
			gpioActions[gpioNumber].pinActions[gpioPin]();
		}
	}
}
*/

void gpio_ExecuteActions(uint32_t gpioBase, const gpio_PinAction pinActions[], uint8_t activeActions)
{
	int8_t i;
	for(i = 0; i < activeActions; i++)
	{
		if(GPIOIntStatus(gpioBase, false) & pinActions[i].GPIO_INT_PIN)
		{
			if(pinActions[i].action != NULL)
			{
				GPIOIntClear(gpioBase, pinActions[i].GPIO_INT_PIN);
				pinActions[i].action();
/*
				while(GPIOIntStatus(gpioBase, false) & pinActions[i].GPIO_INT_PIN)
				{
					GPIOIntClear(gpioBase, pinActions[i].GPIO_INT_PIN);
				}*/

			}
		}
	}

	return;
}

uint32_t gpio_IntTypeGet(uint32_t ui32Port, uint8_t ui8Pin)
{
    uint32_t ui32IBE, ui32IS, ui32IEV, ui32SI;

    //
    // Check the arguments.
    //
    ASSERT(_GPIOBaseValid(ui32Port));
    ASSERT(ui8Pin < 8);

    //
    // Convert from a pin number to a bit position.
    //
    ui8Pin = 1 << ui8Pin;

    //
    // Return the pin interrupt type.
    //
    ui32IBE = HWREG(ui32Port + GPIO_O_IBE);
    ui32IS = HWREG(ui32Port + GPIO_O_IS);
    ui32IEV = HWREG(ui32Port + GPIO_O_IEV);
    ui32SI = HWREG(ui32Port + GPIO_O_SI);
    return(((ui32IBE & ui8Pin) ? 0x01 : 0) | ((ui32IS & ui8Pin) ? 0x02 : 0) |
           ((ui32IEV & ui8Pin) ? 0x04 : 0) | ((ui32SI & 0x01) ? 0x10000 : 0));
}
