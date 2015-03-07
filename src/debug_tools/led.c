#include "led.h"
#include "inc/hw_memmap.h"
#include <stdint.h>

void led_Init(void)
{
	SysCtlPeripheralEnable(LED_GPIO_PERIPH);
	GPIOPinTypeGPIOOutput(LED_GPIO_BASE, RED_GPIO_PIN | GREEN_GPIO_PIN | BLUE_GPIO_PIN);
}

void led_On(void)
{
	GPIOPinWrite(LED_GPIO_BASE, LED_GPIO_PIN, LED_GPIO_PIN);
}

void led_Off(void)
{
	GPIOPinWrite(LED_GPIO_BASE, LED_GPIO_PIN, 0);
}

void rled_On(void)
{
	GPIOPinWrite(RED_GPIO_BASE, RED_GPIO_PIN, RED_GPIO_PIN);
}

void gled_On(void)
{
	GPIOPinWrite(GREEN_GPIO_BASE, GREEN_GPIO_PIN, GREEN_GPIO_PIN);
}

void bled_On(void)
{
	GPIOPinWrite(BLUE_GPIO_BASE, BLUE_GPIO_PIN, BLUE_GPIO_PIN);
}

void rled_Off(void)
{
	GPIOPinWrite(RED_GPIO_BASE, RED_GPIO_PIN, 0);
}

void gled_Off(void)
{
	GPIOPinWrite(GREEN_GPIO_BASE, GREEN_GPIO_PIN, 0);
}

void bled_Off(void)
{
	GPIOPinWrite(BLUE_GPIO_BASE, BLUE_GPIO_PIN, 0);
}

void rled_Toggle(void)
{
	GPIOPinWrite(RED_GPIO_BASE, RED_GPIO_PIN, ~GPIOPinRead(RED_GPIO_BASE, RED_GPIO_PIN));
}

void gled_Toggle(void)
{
	GPIOPinWrite(GREEN_GPIO_BASE, GREEN_GPIO_PIN, ~GPIOPinRead(GREEN_GPIO_BASE, GREEN_GPIO_PIN));
}

void bled_Toggle(void)
{
	GPIOPinWrite(BLUE_GPIO_BASE, BLUE_GPIO_PIN, ~GPIOPinRead(BLUE_GPIO_BASE, BLUE_GPIO_PIN));
}
