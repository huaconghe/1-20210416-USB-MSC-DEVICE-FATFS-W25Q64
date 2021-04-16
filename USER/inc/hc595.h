#ifndef HC595_H
#define HC595_H
#include "stm32f4xx.h"

#define SHCP_H	( GPIOE->ODR |=1<<14 )
#define SHCP_L	( GPIOE->ODR &=~(1<<14) )//LK_SCK,595移位时钟

#define STCP_H  ( GPIOE->ODR |= (1<<8) ) 
#define STCP_L  ( GPIOE->ODR &=~(1<<8) )//LK_CS,595锁存时钟

#define DATE_H  	( GPIOE->ODR |=1<<12) 
#define DATE_L  	( GPIOE->ODR &=~(1<<12) )//LK_SI,595数据输入

#define OE_H    ( GPIOE->ODR |=1<<10 )
#define OE_L    ( GPIOE->ODR &=~(1<<10) )//LK_OE，595使能
typedef unsigned char uchar;

//extern u8 r_buff[100][33];
void GPIO_INIT(void);
void out595_datdu(uchar du0, uchar du1, uchar HBIT, uchar LBIT ) ;
void show_Chinese(void);
void show_many_Chinese(u8*buff);
void show_string(u8* buff);
void show_time(void);

#endif
