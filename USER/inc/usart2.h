#ifndef USART2_H
#define USART2_H

#include "stm32f4xx.h"



typedef struct{
	
	char rx_buff[100];
	char tx_buff[100];
	char rx_flag;
	
}Usart2;

extern Usart2 usart2;



void USART2_Init(u32 pck, u32 fck, u8 over8);
void USART2_Send_str(u8 * str);

#endif


