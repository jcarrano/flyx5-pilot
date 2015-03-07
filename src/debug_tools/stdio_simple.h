/*
 * stdio_simple.h
 *
 *  Created on: 14/02/2015
 *      Author: Juan Ignacio
 */

#ifndef STDIO_SIMPLE_H_
#define STDIO_SIMPLE_H_

#include <stdint.h>


void UARTStringPut(uint32_t ui32Base, const char *s);

//! Wrapper for UARTStringPut, directing output to UART_DEBUG defined in flyx5_hw.
void _puts(const char *s);

void UARTIntPut(uint32_t ui32Base, int x);

#endif /* STDIO_SIMPLE_H_ */
