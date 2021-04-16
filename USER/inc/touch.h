#ifndef TOUCH_H
#define TOUCH_H


#include "stm32f4xx.h"


typedef struct
{
	u16 sx;				//起始X
	u16 sy;				//起始Y
	u16 ex;				//结束X
	u16 ey;				//结束Y
	u16 touch;			//是否按下标志  1表示按下
	u16 loosen;			//是否松手标志  1表示松手
	u8 adjust;			//校准标志位   1表示校准 
	float kx,ky;
	float bx,by;
}TOUCH;


#define 	T_MOSI_H 	GPIOF->BSRRL = 1 << 11
#define 	T_MOSI_L 	GPIOF->BSRRH = 1 << 11
#define 	T_CLK_H 	GPIOB->BSRRL = 1 << 0
#define 	T_CLK_L 	GPIOB->BSRRH = 1 << 0
#define 	T_CS_H 		GPIOC->BSRRL = 1 << 13
#define 	T_CS_L 		GPIOC->BSRRH = 1 << 13
#define 	T_MISO		((GPIOB->IDR & (1 << 2)) && 1)

u16 XPT2046_Send_REV(u8 cmd);
void Touch_Port_Init(void);
void XPT2046_Get_X_Y(u16 *x,u16 *y);
void Touch_Adjust(void);


extern TOUCH touch;




#endif


