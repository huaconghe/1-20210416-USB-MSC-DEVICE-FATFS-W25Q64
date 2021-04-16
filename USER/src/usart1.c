#include "usart1.h"
#include <stdio.h>
#include "dma.h"
Usart1 usart1={0,0,0};

/*****************************************************************************************
*函数功能：USART1初始化
*函数形参:无
*函数返回值：无
*函数备注:USART1_TX/PA9	USART1_RX/PA10
*
*****************************************************************************************/

void USART1_Init(u32 pck, u32 fck, u8 over8)
{
	u32 Priority;
	float USARTDIV;
	u32  Mantissa;  //整数
	u32  Fraction;  //小数部分
	
	RCC->AHB1ENR |= 1 << 0;  //使能GPIOA时钟
	
	/* PA9 -- TX*/
	GPIOA->MODER &= ~(3 << (2 * 9));
	GPIOA->MODER |= 2 << (2 * 9);  //复用模式
	GPIOA->AFR[1] &= ~(15 << (4 *(9 - 8)));
	GPIOA->AFR[1] |= 7 << (4 *(9 - 8));  //复用为串口1
	
	/* PA10 -- RX*/
	GPIOA->MODER &= ~(3 << (2 * 10));
	GPIOA->MODER |= 2 << (2 * 10);  //复用模式
	GPIOA->AFR[1] &= ~(15 << (4 *(10 - 8)));
	GPIOA->AFR[1] |= 7 << (4 *(10 - 8));  //复用为串口1
	
	/* 配置串口1 */
	RCC->APB2ENR |= 1 << 4;     //使能串口1时钟
	USART1->CR1 &= ~(1 << 12);  //8位数据位
	USART1->CR2 &= ~(3 << 12);  //1位停止位
	USART1->CR1 |= 1 << 3;      //发送使能
	USART1->CR1 |= 1 << 2;      //接收使能
	
	/* 波特率配置 */
	if(over8 == 0)
	{
		USART1->CR1 &= ~(1 << 15);   //16倍过采样
	}
	else
	{
		USART1->CR1 |= 1 << 15;     //8倍过采样
	}
	USARTDIV = (fck * 1000000) /(pck * (8 * (2- over8)));
	
	Mantissa = (u32)USARTDIV;
	Fraction = (USARTDIV - Mantissa) * 16;
	USART1->BRR =  Mantissa << 4 | Fraction;
	
	/* 接收中断使能 */
	USART1->CR1 |= 1 << 5;   //接收中断使能
	USART1->CR1 |=1<<4;//空闲中断使能
	USART1->CR3|=1<<6;           //使能串口1的DMA接收
	/* 配置NVIC */
	NVIC_SetPriorityGrouping(7 - 2);  //优先级分组为第2组
	Priority =  NVIC_EncodePriority (7 - 2,  1 , 2) ;
	NVIC_SetPriority(USART1_IRQn, Priority );
	NVIC_EnableIRQ(USART1_IRQn) ;
	
	/* 使能串口 */
	USART1->CR1 |= 1 << 13;   //使能串口
}


void USART1_IRQHandler(void)
{
	static u8 i=0;
	if( USART1->SR & (1<<5))//接收到数据
	{
		usart1.rx_buff[i]=USART1->DR ;
		i++;
	}
	else if( USART1->SR & (1<<4) )//空闲中断
	{
		usart1.rx_buff[i] = USART1->SR ;
		usart1.rx_buff[i] = USART1->DR ;
		usart1.rx_buff[i]='\0';
		usart1.rx_flag = 1;
		i= 0;
	}
}



/*
函数的功能：USART1发送字符串
形参：需要传输的字符串
返回值：void
备注：
*/
void USART1_Send_str(u8 * str) //hello world\0
{
	while(*str != '\0')
	{
		USART1->DR = *str;
		while(!(USART1->SR & (1 << 7)))
		{
			/* 等待发送完成 */
		}
		str++;
	}
}

/*
函数的功能：printf的底层函数
形参：需要传输的字符
返回值：void
备注：
*/
int fputc( int ch, FILE *stream )
{
	USART1->DR = ch;
	while(!(USART1->SR & (1 << 7)))
	{
		/* 等待发送完成 */
	}
	return ch;
}


