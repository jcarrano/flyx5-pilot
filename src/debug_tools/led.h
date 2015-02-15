/*
 * led.h
 *
 *  Created on: 07/02/2015
 *      Author: Juan Ignacio
 */

#ifndef LED_H_
#define LED_H_

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

//
// GPIO, Peripheral, and Pin assignments for the colors
//
#define LED_GPIO_PERIPH			SYSCTL_PERIPH_GPIOF
#define RED_GPIO_PERIPH         SYSCTL_PERIPH_GPIOF
#define RED_TIMER_PERIPH        SYSCTL_PERIPH_TIMER0
#define BLUE_GPIO_PERIPH        SYSCTL_PERIPH_GPIOF
#define BLUE_TIMER_PERIPH       SYSCTL_PERIPH_TIMER1
#define GREEN_GPIO_PERIPH       SYSCTL_PERIPH_GPIOF
#define GREEN_TIMER_PERIPH      SYSCTL_PERIPH_TIMER1


#define LED_GPIO_BASE			GPIO_PORTF_BASE
#define RED_GPIO_BASE           GPIO_PORTF_BASE
#define RED_TIMER_BASE          TIMER0_BASE
#define BLUE_GPIO_BASE          GPIO_PORTF_BASE
#define BLUE_TIMER_BASE         TIMER1_BASE
#define GREEN_GPIO_BASE         GPIO_PORTF_BASE
#define GREEN_TIMER_BASE        TIMER1_BASE

#define RED_GPIO_PIN            GPIO_PIN_1
#define BLUE_GPIO_PIN           GPIO_PIN_2
#define GREEN_GPIO_PIN          GPIO_PIN_3

#define LED_GPIO_PIN			(RED_GPIO_PIN | GREEN_GPIO_PIN | BLUE_GPIO_PIN)

void led_Init(void);

void led_On(void);
void led_Off(void);

void rled_On(void);
void gled_On(void);
void bled_On(void);

void rled_Off(void);
void gled_Off(void);
void bled_Off(void);

void rled_Toggle(void);
void gled_Toggle(void);
void bled_Toggle(void);

#endif /* LED_H_ */
