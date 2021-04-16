#ifndef LCD_H
#define LCD_H

#include "stm32f4xx.h"
#define		WHITE      0xFFFF
#define		BLACK      0x0000	  
#define		BLUE       0x001F  
#define		BRED       0XF81F
#define		GRED 	     0XFFE0
#define		GBLUE		   0X07FF
#define		RED        0xF800
#define		MAGENTA    0xF81F
#define		GREEN      0x07E0
#define		CYAN       0x7FFF
#define		YELLOW     0xFFE0
#define		BROWN 		 0XBC40 //×ØÉ«
#define		BRRED 		 0XFC07 //×ØºìÉ«
#define		GRAY  		 0X8430 //»ÒÉ«
void LCD_Draw_Point(u16 x, u16 y, u16 color);
void LCD_Draw_Circle(int x, int y, int r, u16 color);
void LCD_Draw_Pic(u16 sx, u16 ex, u16 sy, u16 ey, u16* color);
void LCD_Display_English(u16 x, u16 y, u16 color, u8 data);
void LCD_Display_English_Str(u16 x, u16 y, u16 color, u8* buf);
void LCD_Display_Size_English(u16 x, u16 y, u16 color, u8 data, u8 size);
void LCD_Display_Chinese(u16 x, u16 y, u16 color, u8 *p);
void LCD_Display_Chinese_Eevr(u16 x, u16 y, u16 color, u8 *p, u8 size);
void LCD_Dis_Ever(u16 x, u16 y, u16 color, u8 *data, u8 size);
void LCD_Loop_DisPlay(u16 x, u16 y, u16 color, u8 *data);
void LCD_Loop_Dis_Ever(u16 x, u16 y, u16 color, u8 *data);
void LCD_Draw_Ten(u16 x, u16 y, u16 color);
void LCD_Draw_Big_Point(u16 x,u16 y,u16 color);

#endif
