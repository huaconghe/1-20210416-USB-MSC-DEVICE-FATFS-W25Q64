#include "lcd.h"
#include "ili9486.h"
#include <math.h>
#include "fon.h"
#include "systick.h"


/******************��ӯ��*****************************
�������ܣ�LCD���㺯��
�����βΣ�x:���x����  y:���y����   color�������ɫ
��������ֵ����
������ע: 
******************************************************/
void LCD_Draw_Point(u16 x, u16 y, u16 color)
{
	LCD_Send_CMD(0x2A);
	LCD_Send_Data((x & 0xff00) >> 8);
	LCD_Send_Data((x & 0x00ff) >> 0);
	LCD_Send_Data((x & 0xff00) >> 8);
	LCD_Send_Data((x & 0x00ff) >> 0);
	
	LCD_Send_CMD(0x2B);
	LCD_Send_Data((y & 0xff00) >> 8);
	LCD_Send_Data((y & 0x00ff) >> 0);
	LCD_Send_Data((y & 0xff00) >> 8);
	LCD_Send_Data((y & 0x00ff) >> 0);
	
	LCD_Send_CMD(0x2C);
	LCD_Send_Data(color);
}

/******************��ӯ��*****************************
�������ܣ�LCD��Բ����
�����βΣ�x:Բ�ĵ�x����  y:Բ�ĵ�y����   color�������ɫ  r:Բ�İ뾶
��������ֵ����
������ע: 
******************************************************/
void LCD_Draw_Circle(int x, int y, int r, u16 color)
{
	int i,j;
	for(i = x - r; i <= x + r; i++)
	{
		for(j = y - r; j <= y + r; j++)
		{
			if(((i - x) * (i - x) + (j - y) * (j - y) - r * r <= r) &&  (((i - x) * (i - x) + (j - y) * (j - y) - r * r)  >= -r))
			{
				if(i <= 319 && i >= 0 && j <= 479 && j >= 0)
				{
					LCD_Draw_Point(i,j,color);
				}
			}
		}
	}
}

/******************��ӯ��*****************************
�������ܣ�LCD��ӡͼƬ����
�����βΣ�sx:��ʼ�е�ַ  ex:�����е�ַ  sy:��ʼ�е�ַ ey:�����е�ַ  
��������ֵ����
������ע: 
******************************************************/
void LCD_Draw_Pic(u16 sx, u16 ex, u16 sy, u16 ey, u16* color)
{
	u32 i;
	LCD_Send_CMD(0x2A);
	LCD_Send_Data((sx & 0xff00) >> 8);
	LCD_Send_Data((sx & 0x00ff) >> 0);
	LCD_Send_Data((ex & 0xff00) >> 8);
	LCD_Send_Data((ex & 0x00ff) >> 0);
	
	LCD_Send_CMD(0x2B);
	LCD_Send_Data((sy & 0xff00) >> 8);
	LCD_Send_Data((sy & 0x00ff) >> 0);
	LCD_Send_Data((ey & 0xff00) >> 8);
	LCD_Send_Data((ey & 0x00ff) >> 0);
	
	LCD_Send_CMD(0x2C);
	for(i = 0; i < (ex - sx + 1) * (ey - sy + 1); i++)
	{
		LCD_Send_Data(*(color + i));  
	}
}


/******************��ӯ��*****************************
�������ܣ�LCD��ӡ��ĸ
�����βΣ�x: x����  y:y����  color: �ֵ���ɫ 
��������ֵ����
������ע: ��
******************************************************/
void LCD_Display_English(u16 x, u16 y, u16 color, u8 data)
{
	int i,j;
	u16 X;
	for(i = 0; i < 16; i++)
	{
		for(j = 0; j < 8; j++)
		{
			X = x + j;
			if( X > 319)
			{
				X %= 319;
			}
			if(font[data - 32][i] & (0x80 >> j))
			{
				LCD_Draw_Point(X,y + i,color);
			}
			else
			{
				LCD_Draw_Point(X,y + i,0xffff);
			}
		}
	}
}

/******************��ӯ��*****************************
�������ܣ�LCD��ӡӢ���ַ���
�����βΣ�x: x����  y:y����  color: �ֵ���ɫ 
��������ֵ����
������ע: ��
******************************************************/
void LCD_Display_English_Str(u16 x, u16 y, u16 color, u8* buf)
{
	while(*buf)
	{
		LCD_Display_English(x,y,color,*buf);
		buf++;
		x = x + 8;
	}
}


/******************��ӯ��*****************************
�������ܣ�LCD��ӡ��ĸ,��������ѡ����ĸ��С
�����βΣ�x: x����  y:y����  color: �ֵ���ɫ 
��������ֵ����
������ע: ��
******************************************************/
void LCD_Display_Size_English(u16 x, u16 y, u16 color, u8 data, u8 size)
{
	int i,j;
	for(i = 0; i < 16 * size; i++)
	{
		for(j = 0; j < 8 * size; j++)
		{
			if(font[data - 32][i / size] & (0x80 >> j / size))
			{
				LCD_Draw_Point(x + j,y + i,color);
			}
		
		}
	}
}

/******************��ӯ��*****************************
�������ܣ�LCD��ӡ����
�����βΣ�x: x����  y:y����  color: �ֵ���ɫ 
��������ֵ����
������ע: ��
******************************************************/
void LCD_Display_Chinese(u16 x, u16 y, u16 color, u8 *p)
{
	
	int size;
	int i,j;
	u16 X;
	size = ((*p - 0XA1) * 94 + (*(p + 1) - 0xA1)) * 32;
	for(i = 0; i < 16; i ++)
	{
		for(j = 0; j < 16; j++)
		{
			X = x + j;
			if(X > 319)
			{
				X = X % 319;
			}
			if(hanzi[size + (i * 2) + (j / 8)] & (0x80 >> (j % 8)))
			{
				
				LCD_Draw_Point(X,y + i,color);
			}
			else
			{
				LCD_Draw_Point(X,y + i,0xffff);
			}
		}
	}
}

/******************��ӯ��*****************************
�������ܣ�LCD��ӡ���֣������С
�����βΣ�x: x����  y:y����  color: �ֵ���ɫ  size :�ֵĴ�С 
��������ֵ����
������ע: ��
******************************************************/
void LCD_Display_Chinese_Eevr(u16 x, u16 y, u16 color, u8 *p, u8 size)
{
	
	int z_size;
	int i,j;
	z_size = ((*p - 0XA1) * 94 + (*(p + 1) - 0xA1)) * 32;
	for(i = 0; i < 16 * size; i ++)
	{
		for(j = 0; j < 16 * size; j++)
		{
			if(hanzi[z_size + (i / size * 2) + (j / (8 * size))] & (0x80 >> (j / size % 8 )))
			{
				LCD_Draw_Point(x + j,y + i,0x0000);
			}
		}
	}
}

/******************��ӯ��*****************************
�������ܣ�����Ӣ�ĺ��ִ�ӡ�ַ���
�����βΣ�x: x����  y:y����  color: �ֵ���ɫ  size :�ֵĴ�С 
��������ֵ����
������ע: ��
******************************************************/
void LCD_Dis_Ever(u16 x, u16 y, u16 color, u8 *data, u8 size)
{
	while(*data != '\0')
	{
		if(*data > 127)
		{
			LCD_Display_Chinese_Eevr(x,y,color,data,size);
			x += 16 * size;
			data += 2;
		}
		else
		{
			LCD_Display_Size_English(x,y,color,*data,size);
			x += 8 * size;
			data++;
		}
		if(*data > 127)
		{
			if((319 - x) < (16 * size))
			{
				x = 0;
				y += 16 * size;
			}
		}
		else
		{
			if((319 - x) < (8 * size))
			{
				x = 0;
				y += 16 * size;
			}
		}
	
	}
}



/******************��ӯ��*****************************
�������ܣ�LCDѭ����ʾ����Ӣ�ĺ��ִ�ӡ�ַ���
�����βΣ�x: x����  y:y����  color: �ֵ���ɫ  size :�ֵĴ�С 
��������ֵ����
������ע: ��
******************************************************/
void LCD_Loop_Dis_Ever(u16 x, u16 y, u16 color, u8 *data)
{
	while(*data != '\0')
	{
		if(*data > 127)
		{
			LCD_Display_Chinese(x,y,color,data);
			x += 16;
			data += 2;
		}
		else
		{
			LCD_Display_English(x,y,color,*data);
			x += 8;
			data++;
		}
	}
}

/******************��ӯ��*****************************
�������ܣ�LCD���Ƶ�
�����βΣ�x: x����  y:y����  color: �ֵ���ɫ  size :�ֵĴ�С 
��������ֵ����
������ע: ��
******************************************************/
void LCD_Loop_DisPlay(u16 x, u16 y, u16 color, u8 *data)
{
	u16 X;
	while(1)
	{
		LCD_Loop_Dis_Ever(x + X,y,color,data);
		X += 4;
		if(X >= 320)
		{
			X = X % 320;
		}
		SysTick_delay_ms(10);
	}
}

/******************��ӯ��*****************************
�������ܣ�LCD��ʮ�ּ�
�����βΣ�x,y:ʮ�ּ����ĵ�  color: ʮ�ּ���ɫ
��������ֵ����
������ע: ��
******************************************************/
void LCD_Draw_Ten(u16 x, u16 y, u16 color)
{
	u16 i;

	for(i = x - 10; i <= (x + 10); i++)
	{
		LCD_Draw_Point(i,y,color);
	}
	
	for(i = y - 10; i <= (y + 10); i++)
	{
		LCD_Draw_Point(x,i,color);
	}


}

void LCD_Draw_Big_Point(u16 x,u16 y,u16 color)
{
	 u16 i,j;
	for(i = x - 2; i <= x + 2; i++)
	{
		for(j = y - 2; j <= y + 2; j++)
		{
			if(i <= 319 && j <= 479)
			{
				LCD_Draw_Point(i,j,color);
			}
		}
	}
}











