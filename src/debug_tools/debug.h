/*
 * debug.h
 *
 *  Created on: 05/02/2015
 *      Author: Juan Ignacio
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "../common.h"
#include "../macro_magic.h"

#define IO_STD UART0
#define IO_STD_BASE GLUE(IO_STD, _BASE)



// General Key
#define DEBUG

#ifdef DEBUG

/* Print Tools*/
extern void UARTCharPut(uint32_t ui32Base, unsigned char ucData);
#define Putchar(x) UARTCharPut(IO_STD_BASE, x)


// Particular Keys
#define IIC_DEBUG_EOT
#define IIC_DEBUG_ERR
#define IIC_DEBUG

#define DMU_DEBUG_INIT


#endif /* DEBUG	*/

#endif /* DEBUG_H_ */
