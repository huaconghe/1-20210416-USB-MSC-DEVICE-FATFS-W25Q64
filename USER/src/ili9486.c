#include "ili9486.h"
#include "systick.h"

/******************信盈达*****************************
函数功能：ili9486与MCU的连接管脚配置
函数形参：无
函数返回值：无
函数备注: 
CS:PD7  WR:PD5  RS:PD6  RD:PD4	//推挽输出 默认状态为高电平
BL:PB15  D0:PD14  D1:PD15  D2:PD0
D3:PD1   D4:PE7   D5:PE8   D6:PE9
D7:PE10  D8:PE11  D9:PE12  D10:PE13
D11:PE14 D12:PE15  D13:PD8 D14:PD9
D15:PD10						//配置成推挽输出  默认状态为低电平
******************************************************/
void ILI9486_Port_Init(void)
{
	RCC->AHB1ENR |= 1 << 6 | 1 << 3 | 1 << 5 | 1 << 1 | 1 << 4;	//打开GDFBE五组IO口时钟
	
	/*CS*/
	GPIOD->MODER &= ~(3ul << (2 * 7));				//清零
	GPIOD->MODER |= 1ul << (2 * 7);				//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 7));				//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 7);				//配置成高速
	LCD_CS_H;										//CS拉高
	
	/*WR*/
	GPIOD->MODER &= ~(3ul << (2 * 5));				//清零
	GPIOD->MODER |= 1ul << (2 * 5);					//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 5));				//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 5);					//配置成高速
	LCD_WR_H;										//WR拉高
	
	/*RS*/
	GPIOD->MODER &= ~(3ul << (2 * 6));				//清零
	GPIOD->MODER |= 1ul << (2 * 6);					//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 6));				//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 6);					//配置成高速
	LCD_RS_H;	

	/*RD*/
	GPIOD->MODER &= ~(3ul << (2 * 4));				//清零
	GPIOD->MODER |= 1ul << (2 * 4);					//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 4));				//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 4);				//配置成高速
	LCD_RD_H;										//WR拉高

	
	/*BL*/
	GPIOB->MODER &= ~(3ul << (2 * 15));				//清零
	GPIOB->MODER |= 1ul << (2 * 15);				//配置成输出模式
	GPIOB->OTYPER &= ~(1ul << (1 * 15));			//配置成推挽
	GPIOB->OSPEEDR |= 3ul << (2 * 15);				//配置成高速

	/*D0~D15*/
	GPIOD->MODER &= ~(3ul << (2 * 14));				//清零
	GPIOD->MODER |= 1ul << (2 * 14);				//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 14));			//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 14);				//配置成高速
	
	GPIOD->MODER &= ~(3ul << (2 * 15));				//清零
	GPIOD->MODER |= 1ul << (2 * 15);				//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 15));			//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 15);				//配置成高速
	
	GPIOD->MODER &= ~(3ul << (2 * 0));				//清零
	GPIOD->MODER |= 1ul << (2 * 0);					//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 0));				//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 0);				//配置成高速
	
	GPIOD->MODER &= ~(3ul << (2 * 1));				//清零
	GPIOD->MODER |= 1ul << (2 * 1);					//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 1));				//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 1);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 7));				//清零
	GPIOE->MODER |= 1ul << (2 * 7);					//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 7));				//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 7);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 8));				//清零
	GPIOE->MODER |= 1ul << (2 * 8);					//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 8));				//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 8);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 9));				//清零
	GPIOE->MODER |= 1ul << (2 * 9);					//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 9));				//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 9);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 10));				//清零
	GPIOE->MODER |= 1ul << (2 * 10);				//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 10));			//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 10);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 11));				//清零
	GPIOE->MODER |= 1ul << (2 * 11);				//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 11));			//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 11);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 12));				//清零
	GPIOE->MODER |= 1ul << (2 * 12);				//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 12));			//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 12);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 13));				//清零
	GPIOE->MODER |= 1ul << (2 * 13);				//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 13));			//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 13);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 14));				//清零
	GPIOE->MODER |= 1ul << (2 * 14);				//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 14));			//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 14);				//配置成高速
	
	GPIOE->MODER &= ~(3ul << (2 * 15));				//清零
	GPIOE->MODER |= 1ul << (2 * 15);				//配置成输出模式
	GPIOE->OTYPER &= ~(1ul << (1 * 15));			//配置成推挽
	GPIOE->OSPEEDR |= 3ul << (2 * 15);				//配置成高速
	
	GPIOD->MODER &= ~(3ul << (2 * 8));				//清零
	GPIOD->MODER |= 1ul << (2 * 8);					//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 8));				//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 8);				//配置成高速
	
	GPIOD->MODER &= ~(3ul << (2 * 9));				//清零
	GPIOD->MODER |= 1ul << (2 * 9);					//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 9));				//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 9);				//配置成高速
	
	GPIOD->MODER &= ~(3ul << (2 * 10));				//清零
	GPIOD->MODER |= 1ul << (2 * 10);				//配置成输出模式
	GPIOD->OTYPER &= ~(1ul << (1 * 10));			//配置成推挽
	GPIOD->OSPEEDR |= 3ul << (2 * 10);				//配置成高速
}

/******************信盈达*****************************
函数功能：把16位数据转变成16个数据线输出
函数形参：data:要输出的16位数据
函数返回值：无
函数备注: 
CS:PG12  WR:PD5  RS:PD6  RD:PD4	//推挽输出 默认状态为高电平
BL:PB15  D0:PD14  D1:PD15  D2:PD0
D3:PD1   D4:PE7   D5:PE8   D6:PE9
D7:PE10  D8:PE11  D9:PE12  D10:PE13
D11:PE14 D12:PE15  D13:PD8 D14:PD9
D15:PD10						//配置成推挽输出  默认状态为低电平
******************************************************/
void ILI9486_Set(u16 data)
{
	if(data & (1 << 0))
	{
		GPIOD->BSRRL = 1 << 14;
	}
	else
	{
		GPIOD->BSRRH = 1 << 14;
	}
	
	if(data & (1 << 1))
	{
		GPIOD->BSRRL = 1 << 15;
	}
	else
	{
		GPIOD->BSRRH = 1 << 15;
	}
	
	if(data & (1 << 2))
	{
		GPIOD->BSRRL = 1 << 0;
	}
	else
	{
		GPIOD->BSRRH = 1 << 0;
	}
	
	if(data & (1 << 3))
	{
		GPIOD->BSRRL = 1 << 1;
	}
	else
	{
		GPIOD->BSRRH = 1 << 1;
	}
	
	if(data & (1 << 4))
	{
		GPIOE->BSRRL = 1 << 7;
	}
	else
	{
		GPIOE->BSRRH = 1 << 7;
	}
	
	if(data & (1 << 5))
	{
		GPIOE->BSRRL = 1 << 8;
	}
	else
	{
		GPIOE->BSRRH = 1 << 8;
	}
	
	if(data & (1 << 6))
	{
		GPIOE->BSRRL = 1 << 9;
	}
	else
	{
		GPIOE->BSRRH = 1 << 9;
	}
	
	if(data & (1 << 7))
	{
		GPIOE->BSRRL = 1 << 10;
	}
	else
	{
		GPIOE->BSRRH = 1 << 10;
	}

	if(data & (1 << 8))
	{
		GPIOE->BSRRL = 1 << 11;
	}
	else
	{
		GPIOE->BSRRH = 1 << 11;
	}

	if(data & (1 << 9))
	{
		GPIOE->BSRRL = 1 << 12;
	}
	else
	{
		GPIOE->BSRRH = 1 << 12;
	}
	
	if(data & (1 << 10))
	{
		GPIOE->BSRRL = 1 << 13;
	}
	else
	{
		GPIOE->BSRRH = 1 << 13;
	}

	if(data & (1 << 11))
	{
		GPIOE->BSRRL = 1 << 14;
	}
	else
	{
		GPIOE->BSRRH = 1 << 14;
	}
	
	if(data & (1 << 12))
	{
		GPIOE->BSRRL = 1 << 15;
	}
	else
	{
		GPIOE->BSRRH = 1 << 15;
	}
	
	if(data & (1 << 13))
	{
		GPIOD->BSRRL = 1 << 8;
	}
	else
	{
		GPIOD->BSRRH = 1 << 8;
	}
	
	if(data & (1 << 14))
	{
		GPIOD->BSRRL = 1 << 9;
	}
	else
	{
		GPIOD->BSRRH = 1 << 9;
	}
	
	if(data & (1 << 15))
	{
		GPIOD->BSRRL = 1 << 10;
	}
	else
	{
		GPIOD->BSRRH = 1 << 10;
	}
}

/******************信盈达*****************************
函数功能：发送命令
函数形参：cmd:命令
函数返回值：无
函数备注: 
******************************************************/
void LCD_Send_CMD(u16 cmd)
{
	LCD_CS_L;
	LCD_RS_L;
	LCD_WR_L;
	ILI9486_Set(cmd);
	LCD_WR_H;
	LCD_CS_H;
}

/******************信盈达*****************************
函数功能：发送数据
函数形参：data:数据
函数返回值：无
函数备注: 
******************************************************/
void LCD_Send_Data(u16 data)
{
	LCD_CS_L;
	LCD_RS_H;
	LCD_WR_L;
	ILI9486_Set(data);
	LCD_WR_H;
	LCD_CS_H;
}

/******************信盈达*****************************
函数功能：LCD清屏函数
函数形参：sx:起始列地址  ex:结束列地址  sy:起始行地址 ey:结束行地址  color:背景颜色
函数返回值：无
函数备注: 
******************************************************/
void LCD_Clear(u16 sx, u16 ex, u16 sy, u16 ey, u16 color)
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
		LCD_Send_Data(color);
	}
}

/******************信盈达*****************************
函数功能：LCD初始化代码
函数形参：无
函数返回值：无
函数备注: 这个代码由厂家提供
******************************************************/
void ILI9486_Init(void)
{
	ILI9486_Port_Init();				//初始化GPIO口
	//************* Reset LCD Driver ****************//
	SysTick_delay_ms(120); // Delay 120 ms
	//************* Start Initial Sequence **********//
	LCD_Send_CMD(0XF2);
	LCD_Send_Data(0x18);
	LCD_Send_Data(0xA3);
	LCD_Send_Data(0x12);
	LCD_Send_Data(0x02);
	LCD_Send_Data(0XB2);
	LCD_Send_Data(0x12);
	LCD_Send_Data(0xFF);
	LCD_Send_Data(0x10);
	LCD_Send_Data(0x00);
	LCD_Send_CMD(0XF8);
	LCD_Send_Data(0x21);
	LCD_Send_Data(0x04);
	LCD_Send_CMD(0XF9);
	LCD_Send_Data(0x00);
	LCD_Send_Data(0x08);
	LCD_Send_CMD(0x36);
	LCD_Send_Data(0x08);   //设置RGB,含排线的屏
//	LCD_Send_Data(0x00);   //设置RGB，不含排线的屏BGR
	LCD_Send_CMD(0x3A);
	LCD_Send_Data(0x05);   //设置16位BPP
	LCD_Send_CMD(0xB4);
	LCD_Send_Data(0x01);//0x00
	LCD_Send_CMD(0xB6);
	LCD_Send_Data(0x02);
	LCD_Send_Data(0x22);
	LCD_Send_CMD(0xC1);
	LCD_Send_Data(0x41);
	LCD_Send_CMD(0xC5);
	LCD_Send_Data(0x00);
	LCD_Send_Data(0x07);//0X18
	LCD_Send_CMD(0xE0);
	LCD_Send_Data(0x0F);
	LCD_Send_Data(0x1F);
	LCD_Send_Data(0x1C);
	LCD_Send_Data(0x0C);
	LCD_Send_Data(0x0F);
	LCD_Send_Data(0x08);
	LCD_Send_Data(0x48);
	LCD_Send_Data(0x98);
	LCD_Send_Data(0x37);
	LCD_Send_Data(0x0A);
	LCD_Send_Data(0x13);
	LCD_Send_Data(0x04);
	LCD_Send_Data(0x11);
	LCD_Send_Data(0x0D);
	LCD_Send_Data(0x00);
	LCD_Send_CMD(0xE1);
	LCD_Send_Data(0x0F);
	LCD_Send_Data(0x32);
	LCD_Send_Data(0x2E);
	LCD_Send_Data(0x0B);
	LCD_Send_Data(0x0D);
	LCD_Send_Data(0x05);
	LCD_Send_Data(0x47);
	LCD_Send_Data(0x75);
	LCD_Send_Data(0x37);
	LCD_Send_Data(0x06);
	LCD_Send_Data(0x10);
	LCD_Send_Data(0x03);
	LCD_Send_Data(0x24);
	LCD_Send_Data(0x20);
	LCD_Send_Data(0x00);
	LCD_Send_CMD(0x11);   //退出睡眠
	SysTick_delay_ms(120);
	LCD_Send_CMD(0x29);   //开启显示

	LCD_Send_CMD(0x36); //设置扫描方向
	LCD_Send_Data(0x08);//正常扫描从左到右从上到下,选择RGB格式
	LCD_Clear(0,319,0,479,0xffff);//0001000100010001   32 64 136
	GPIOB->BSRRL = 1 << 15;//打开背光灯
}


