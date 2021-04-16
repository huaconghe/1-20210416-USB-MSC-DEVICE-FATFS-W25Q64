#ifndef LED_H
#define LED_H

#include "stm32f4xx.h"

#define LED1_ON GPIOA->BSRRH |=1<<4 //LED1 ��
#define LED2_ON GPIOA->BSRRH |=1<<5	//LED2 ��
#define LED3_ON GPIOA->BSRRH |=1<<6 //LED3 ��
#define LED4_ON GPIOA->BSRRH |=1<<7 //LED4 ��

#define LED1_OFF GPIOA->BSRRL |=1<<4 //LED1 ��
#define LED2_OFF GPIOA->BSRRL |=1<<5 //LED2 ��
#define LED3_OFF GPIOA->BSRRL |=1<<6 //LED3 ��
#define LED4_OFF GPIOA->BSRRL |=1<<7 //LED4 ��

#define LED1_XOR GPIOA->ODR ^=1<<4 
#define LED2_XOR GPIOA->ODR ^=1<<5 
#define LED3_XOR GPIOA->ODR ^=1<<6 
#define LED4_XOR GPIOA->ODR ^=1<<7 

void LED_Init(void);


#endif
