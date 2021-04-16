#ifndef USART1_H
#define USART1_H

#include "stm32f4xx.h"



typedef struct{
	
	char rx_buff[100];
	char tx_buff[100];
	char rx_flag;
	
}Usart1;

extern Usart1 usart1;



void USART1_Init(u32 pck, u32 fck, u8 over8);
void USART1_Send_str(u8 * str);

#endif
