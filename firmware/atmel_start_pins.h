/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMD10 has 8 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7

#define LED7 GPIO(GPIO_PORTA, 2)
#define OUT3 GPIO(GPIO_PORTA, 3)
#define OUT4 GPIO(GPIO_PORTA, 4)
#define OUT2 GPIO(GPIO_PORTA, 5)
#define OUT1 GPIO(GPIO_PORTA, 6)
#define LED11 GPIO(GPIO_PORTA, 7)
#define LED9 GPIO(GPIO_PORTA, 14)
#define LED10 GPIO(GPIO_PORTA, 15)
#define SW_OUT GPIO(GPIO_PORTA, 16)
#define LED8 GPIO(GPIO_PORTA, 22)
#define SW_MODE GPIO(GPIO_PORTA, 23)
#define IN_TRIGGER GPIO(GPIO_PORTA, 24)
#define IN_RESET GPIO(GPIO_PORTA, 25)

#endif // ATMEL_START_PINS_H_INCLUDED
