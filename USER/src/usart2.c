#include "usart2.h"
#include <stdio.h>
#include "systick.h"

Usart2 usart2={{"深圳大学电子科学与技术学院"},0,0};

/*****************************************************************************************
*函数功能：USART2初始化
*函数形参:无
*函数返回值：无
*函数备注:USART2_TX/PA2	USART2_RX/PA3
*
*****************************************************************************************/

void USART2_Init(u32 pck, u32 fck, u8 over8)
{
	u32 Priority;
	float USARTDIV;
	u32  Mantissa;  //整数
	u32  Fraction;  //小数部分
	
	RCC->AHB1ENR |= 1 << 0;  //使能GPIOA时钟
	
	/* PA2 -- TX*/
	GPIOA->MODER &= ~(3 << (2 * 2));
	GPIOA->MODER |= 2 << (2 * 2);  //复用模式
	GPIOA->AFR[0] &= ~(15 << (4 *2));
	GPIOA->AFR[0] |= 7 << (4 *2);  //复用为串口2
	
	/* PA3 -- RX*/
	GPIOA->MODER &= ~(3 << (2 * 3));
	GPIOA->MODER |= 2 << (2 * 3);  //复用模式
	GPIOA->AFR[0] &= ~(15 << (4 *3));
	GPIOA->AFR[0] |= 7 << (4 *3);  //复用为串口2
	
	/* 配置串口2 */
	RCC->APB1ENR |= 1 << 17;     //使能串口2时钟
	USART2->CR1 &= ~(1 << 12);  //8位数据位
	USART2->CR2 &= ~(3 << 12);  //1位停止位
	USART2->CR1 |= 1 << 3;      //发送使能
	USART2->CR1 |= 1 << 2;      //接收使能
	
	/* 波特率配置 */
	if(over8 == 0)
	{
		USART2->CR1 &= ~(1 << 15);   //16倍过采样
	}
	else
	{
		USART2->CR1 |= 1 << 15;     //8倍过采样
	}
	USARTDIV = (fck * 1000000) /(pck * (8 * (2- over8)));
	
	Mantissa = (u32)USARTDIV;
	Fraction = (USARTDIV - Mantissa) * 16;
	USART2->BRR =  Mantissa << 4 | Fraction;
	
	/* 接收中断使能 */
	USART2->CR1 |= 1 << 5;   //接收中断使能
	USART2->CR1 |=1<<4;//空闲中断使能
	
	/* 配置NVIC */
	NVIC_SetPriorityGrouping(6 - 2);  //优先级分组为第2组
	Priority =  NVIC_EncodePriority (6 - 2,  1 , 2) ;
	NVIC_SetPriority(USART2_IRQn, Priority );
	NVIC_EnableIRQ(USART2_IRQn) ;
	
	/* 使能串口 */
	USART2->CR1 |= 1 << 13;   //使能串口
}

//void USART2_IRQHandler(void)
//{
//	static u8 i=0;
//	if( USART2->SR & (1<<5))
//	{
//		usart2.rx_buff[i]=USART2->DR ;
//		i++;
//	}
//	else if( USART2->SR & (1<<4) )
//	{
//		usart2.rx_buff[i] = USART2->SR ;
//		usart2.rx_buff[i] = USART2->DR ;
//		usart2.rx_buff[i] = '\0';
//		i = 0;
//		usart2.rx_flag = 1;
//	}
//}
/*
函数的功能：USART1发送字符串
形参：需要传输的字符串
返回值：void
备注：
*/
void USART2_Send_str(u8 * str) //hello world\0
{
	while(*str != '\0')
	{
		USART2->DR = *str;
		while(!(USART2->SR & (1 << 7)))
		{
			/* 等待发送完成 */
		}
		str++;
	}
}








