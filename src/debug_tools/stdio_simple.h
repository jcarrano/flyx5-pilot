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

#ifndef STDIO_SIMPLE_H_
#define STDIO_SIMPLE_H_

#include <stdint.h>

/**
 * Wrapper for UARTStringPut, directing output to UART_DEBUG defined in flyx5_hw.
 */
void _puts(const char *s);

/**
 * Wrapper for UARTIntPut, directing output to UART_DEBUG defined in flyx5_hw.
 */
void _puti(int x);

void _putu(unsigned int x);

/**
 * Send a string to the UART, blocks execution until done.
 *
 * String must be null terminated.
 */
void UARTStringPut(uint32_t ui32Base, const char *s);

/**
 * Convert int to string and send to UART, blocks execution until done.
 */
void UARTIntPut(uint32_t ui32Base, int x);

/**
 * Send a raw 16 bit value over the uart.
 *
 * Data is sent in network byte-order (big-endian).
 */
void UARTputraw16(uint16_t x);

#endif /* STDIO_SIMPLE_H_ */
