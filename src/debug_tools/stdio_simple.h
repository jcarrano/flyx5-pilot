/*
 * stdio_simple.h
 *
 *  Created on: 14/02/2015
 *      Author: Juan Ignacio
 */

#ifndef STDIO_SIMPLE_H_
#define STDIO_SIMPLE_H_

#include "inc/hw_memmap.h"

#define UART_MODULE_BASE UART0_BASE

void puts(char* str);


#endif /* STDIO_SIMPLE_H_ */
