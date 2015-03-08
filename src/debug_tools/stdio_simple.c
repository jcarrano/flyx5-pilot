/**
 * @file
 * @author Juan Ignacio Ubeira
 * @copyright	Copyright 2013 by Juan I Carrano, Andrés Calcabrini, Juan I. Ubeira and
 * 		Nicolás Venturo. All rights reserved. \n
 * 		\n
 *		This program is free software: you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation, either version 3 of the License, or
 *		(at your option) any later version. \n
 * 		\n
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details. \n
 * 		\n
 *		You should have received a copy of the GNU General Public License
 *		along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Simple routines for sending data to the uart.
 */

#include "stdio_simple.h"
#include "../flyx5_hw.h"
#include "../common.h"

void _puts(const char* str)
{
	UARTStringPut(BASE_PERIPH(UART_DEBUG), str);
}

void _puti(int x)
{
	UARTIntPut(BASE_PERIPH(UART_DEBUG), x);
}

void _putu(unsigned int x)
{
	UARTUIntPut(BASE_PERIPH(UART_DEBUG), x);
}


void UARTStringPut(uint32_t ui32Base, const char *s)
{
    char c;

    while((c = *s) != '\0') {
        R_(UARTCharPut)(ui32Base, c);
        s++;
    }
}

void UARTIntPut(uint32_t ui32Base, int x)
{
#define INT_MAX_S_LEN 32
    char s[INT_MAX_S_LEN];
    int i = INT_MAX_S_LEN-2;

    s[INT_MAX_S_LEN-1] = 0;

    while (x) {
        s[i--] = x%10 + '0';
        x = x/10;
    }

    UARTStringPut(ui32Base, s + i + 1);
}

void UARTUIntPut(uint32_t ui32Base, unsigned int x)
{
    char s[INT_MAX_S_LEN];
    int i = INT_MAX_S_LEN-2;

    s[INT_MAX_S_LEN-1] = 0;

    while (x) {
        s[i--] = x%10 + '0';
        x = x/10;
    }

    UARTStringPut(ui32Base, s + i + 1);
}

void UARTputraw16(uint16_t x)
{
    UARTCharPut(UART0_BASE, x / 256);
    UARTCharPut(UART0_BASE, x & 0xFF);
}
