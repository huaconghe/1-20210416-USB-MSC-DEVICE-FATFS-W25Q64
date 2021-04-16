#ifndef ILI9486_H
#define ILI9486_H

#include "stm32f4xx.h"

#define 	LCD_CS_L 	GPIOD->BSRRH = 1 << 7
#define 	LCD_CS_H 	GPIOD->BSRRL = 1 << 7
#define 	LCD_WR_L 	GPIOD->BSRRH = 1 << 5
#define 	LCD_WR_H 	GPIOD->BSRRL = 1 << 5
#define 	LCD_RS_L 	GPIOD->BSRRH = 1 << 6
#define 	LCD_RS_H 	GPIOD->BSRRL = 1 << 6
#define 	LCD_RD_L 	GPIOD->BSRRH = 1 << 4
#define 	LCD_RD_H 	GPIOD->BSRRL = 1 << 4

void ILI9486_Init(void);
void LCD_Clear(u16 sx, u16 ex, u16 sy, u16 ey, u16 color);
void LCD_Send_Data(u16 data);
void LCD_Send_CMD(u16 cmd);

#endif



