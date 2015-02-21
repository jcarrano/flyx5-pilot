#include "stdio_simple.h"
#include "driverlib/uart.h"

void _puts(char* str)
{
	int index = 0;

	while(str[index] != '\0')
	{
		UARTCharPut(UART_MODULE_BASE, str[index++]);
	}
}

