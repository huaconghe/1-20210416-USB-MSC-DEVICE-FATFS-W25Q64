#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f4xx.h"

//#define delay_ms(x) SysTick_delay_ms(x)
//#define delay_us(x) SysTick_delay_us(x)

void SysTick_delay_ms(u16 ms);
void SysTick_delay_us(u16 us);


#endif
