/*
 * error.h
 *
 *  Created on: 12/02/2015
 *      Author: Juan Ignacio
 */

#ifndef ERROR_H_
#define ERROR_H_


typedef void (*err_action)(void);
typedef void (*err_print)(const char* errMsg);

void err_Init(err_action emergency, err_print printMsg, err_action steady);

void err_Throw(char* errMsg);

#endif /* ERROR_H_ */
