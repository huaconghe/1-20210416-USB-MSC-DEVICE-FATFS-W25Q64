#include "usart2.h"
#include <stdio.h>
#include "systick.h"

Usart2 usart2={{"���ڴ�ѧ���ӿ�ѧ�뼼��ѧԺ"},0,0};

/*****************************************************************************************
*�������ܣ�USART2��ʼ��
*�����β�:��
*��������ֵ����
*������ע:USART2_TX/PA2	USART2_RX/PA3
*
*****************************************************************************************/

void USART2_Init(u32 pck, u32 fck, u8 over8)
{
	u32 Priority;
	float USARTDIV;
	u32  Mantissa;  //����
	u32  Fraction;  //С������
	
	RCC->AHB1ENR |= 1 << 0;  //ʹ��GPIOAʱ��
	
	/* PA2 -- TX*/
	GPIOA->MODER &= ~(3 << (2 * 2));
	GPIOA->MODER |= 2 << (2 * 2);  //����ģʽ
	GPIOA->AFR[0] &= ~(15 << (4 *2));
	GPIOA->AFR[0] |= 7 << (4 *2);  //����Ϊ����2
	
	/* PA3 -- RX*/
	GPIOA->MODER &= ~(3 << (2 * 3));
	GPIOA->MODER |= 2 << (2 * 3);  //����ģʽ
	GPIOA->AFR[0] &= ~(15 << (4 *3));
	GPIOA->AFR[0] |= 7 << (4 *3);  //����Ϊ����2
	
	/* ���ô���2 */
	RCC->APB1ENR |= 1 << 17;     //ʹ�ܴ���2ʱ��
	USART2->CR1 &= ~(1 << 12);  //8λ����λ
	USART2->CR2 &= ~(3 << 12);  //1λֹͣλ
	USART2->CR1 |= 1 << 3;      //����ʹ��
	USART2->CR1 |= 1 << 2;      //����ʹ��
	
	/* ���������� */
	if(over8 == 0)
	{
		USART2->CR1 &= ~(1 << 15);   //16��������
	}
	else
	{
		USART2->CR1 |= 1 << 15;     //8��������
	}
	USARTDIV = (fck * 1000000) /(pck * (8 * (2- over8)));
	
	Mantissa = (u32)USARTDIV;
	Fraction = (USARTDIV - Mantissa) * 16;
	USART2->BRR =  Mantissa << 4 | Fraction;
	
	/* �����ж�ʹ�� */
	USART2->CR1 |= 1 << 5;   //�����ж�ʹ��
	USART2->CR1 |=1<<4;//�����ж�ʹ��
	
	/* ����NVIC */
	NVIC_SetPriorityGrouping(6 - 2);  //���ȼ�����Ϊ��2��
	Priority =  NVIC_EncodePriority (6 - 2,  1 , 2) ;
	NVIC_SetPriority(USART2_IRQn, Priority );
	NVIC_EnableIRQ(USART2_IRQn) ;
	
	/* ʹ�ܴ��� */
	USART2->CR1 |= 1 << 13;   //ʹ�ܴ���
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
�����Ĺ��ܣ�USART1�����ַ���
�βΣ���Ҫ������ַ���
����ֵ��void
��ע��
*/
void USART2_Send_str(u8 * str) //hello world\0
{
	while(*str != '\0')
	{
		USART2->DR = *str;
		while(!(USART2->SR & (1 << 7)))
		{
			/* �ȴ�������� */
		}
		str++;
	}
}








