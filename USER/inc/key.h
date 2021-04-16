#ifndef KEY_H
#define KEY_H

#include "stm32f4xx.h"


#define KEY1 ( GPIOA->IDR & (1<<0) ) //����1����KEY1=1
#define KEY2 !( GPIOE->IDR & (1<<2) )//����2����KEY2=1
#define KEY3 !( GPIOE->IDR & (1<<3) )//����3����KEY3=1
#define KEY4 !( GPIOE->IDR & (1<<4) )//����4����KEY4=1

void Delay_ms(u32 ms);
void KYE_Init(void);
u8 key_scan(void);


#endif
