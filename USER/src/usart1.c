#include "usart1.h"
#include <stdio.h>
#include "dma.h"
Usart1 usart1={0,0,0};

/*****************************************************************************************
*�������ܣ�USART1��ʼ��
*�����β�:��
*��������ֵ����
*������ע:USART1_TX/PA9	USART1_RX/PA10
*
*****************************************************************************************/

void USART1_Init(u32 pck, u32 fck, u8 over8)
{
	u32 Priority;
	float USARTDIV;
	u32  Mantissa;  //����
	u32  Fraction;  //С������
	
	RCC->AHB1ENR |= 1 << 0;  //ʹ��GPIOAʱ��
	
	/* PA9 -- TX*/
	GPIOA->MODER &= ~(3 << (2 * 9));
	GPIOA->MODER |= 2 << (2 * 9);  //����ģʽ
	GPIOA->AFR[1] &= ~(15 << (4 *(9 - 8)));
	GPIOA->AFR[1] |= 7 << (4 *(9 - 8));  //����Ϊ����1
	
	/* PA10 -- RX*/
	GPIOA->MODER &= ~(3 << (2 * 10));
	GPIOA->MODER |= 2 << (2 * 10);  //����ģʽ
	GPIOA->AFR[1] &= ~(15 << (4 *(10 - 8)));
	GPIOA->AFR[1] |= 7 << (4 *(10 - 8));  //����Ϊ����1
	
	/* ���ô���1 */
	RCC->APB2ENR |= 1 << 4;     //ʹ�ܴ���1ʱ��
	USART1->CR1 &= ~(1 << 12);  //8λ����λ
	USART1->CR2 &= ~(3 << 12);  //1λֹͣλ
	USART1->CR1 |= 1 << 3;      //����ʹ��
	USART1->CR1 |= 1 << 2;      //����ʹ��
	
	/* ���������� */
	if(over8 == 0)
	{
		USART1->CR1 &= ~(1 << 15);   //16��������
	}
	else
	{
		USART1->CR1 |= 1 << 15;     //8��������
	}
	USARTDIV = (fck * 1000000) /(pck * (8 * (2- over8)));
	
	Mantissa = (u32)USARTDIV;
	Fraction = (USARTDIV - Mantissa) * 16;
	USART1->BRR =  Mantissa << 4 | Fraction;
	
	/* �����ж�ʹ�� */
	USART1->CR1 |= 1 << 5;   //�����ж�ʹ��
	USART1->CR1 |=1<<4;//�����ж�ʹ��
	USART1->CR3|=1<<6;           //ʹ�ܴ���1��DMA����
	/* ����NVIC */
	NVIC_SetPriorityGrouping(7 - 2);  //���ȼ�����Ϊ��2��
	Priority =  NVIC_EncodePriority (7 - 2,  1 , 2) ;
	NVIC_SetPriority(USART1_IRQn, Priority );
	NVIC_EnableIRQ(USART1_IRQn) ;
	
	/* ʹ�ܴ��� */
	USART1->CR1 |= 1 << 13;   //ʹ�ܴ���
}


void USART1_IRQHandler(void)
{
	static u8 i=0;
	if( USART1->SR & (1<<5))//���յ�����
	{
		usart1.rx_buff[i]=USART1->DR ;
		i++;
	}
	else if( USART1->SR & (1<<4) )//�����ж�
	{
		usart1.rx_buff[i] = USART1->SR ;
		usart1.rx_buff[i] = USART1->DR ;
		usart1.rx_buff[i]='\0';
		usart1.rx_flag = 1;
		i= 0;
	}
}



/*
�����Ĺ��ܣ�USART1�����ַ���
�βΣ���Ҫ������ַ���
����ֵ��void
��ע��
*/
void USART1_Send_str(u8 * str) //hello world\0
{
	while(*str != '\0')
	{
		USART1->DR = *str;
		while(!(USART1->SR & (1 << 7)))
		{
			/* �ȴ�������� */
		}
		str++;
	}
}

/*
�����Ĺ��ܣ�printf�ĵײ㺯��
�βΣ���Ҫ������ַ�
����ֵ��void
��ע��
*/
int fputc( int ch, FILE *stream )
{
	USART1->DR = ch;
	while(!(USART1->SR & (1 << 7)))
	{
		/* �ȴ�������� */
	}
	return ch;
}


