#include "key.h"
#include "systick.h"

/****************************************************************************************
*函数功能：按键初始化
*函数形参:无
*函数返回值：无
*函数备注:KEY1-PA0  KEY2-PE2  KEY3-PE3  KEY4-PE4
*
*****************************************************************************************/
void KYE_Init(void)
{
	RCC->AHB1ENR |=1<<0;//打开PA组时钟
	RCC->AHB1ENR |=1<<4;//打开PE组时钟
	
	/*KEY1-PA0*/
	GPIOA->MODER &=~(3<<(2*0));//配置为输入模式
	GPIOA->PUPDR &=~(3<<(2*0));//无上下拉
	                     
	/*KEY2-PE2*/         
	GPIOE->MODER &=~(3<<(2*2));//配置为输入模式
	GPIOE->PUPDR &=~(3<<(2*2));//无上下拉
	                     
	/*KEY3-PE3*/         
	GPIOE->MODER &=~(3<<(2*3));//配置为输入模式
	GPIOE->PUPDR &=~(3<<(2*3));//无上下拉
                       
	/*KE4-PE4*/          
	GPIOE->MODER &=~(3<<(2*4));//配置为输入模式
	GPIOE->PUPDR &=~(3<<(2*4));//无上下拉
	
}

/****************************************************************************************
*函数功能：按键检测
*函数形参:无
*函数返回值：键位值
*函数备注:
*
*****************************************************************************************/
u8 key_scan(void)
{
	static u8 key_flag = 0;
	
	if( ( key_flag == 0 ) &&  (( KEY1 == 1) || ( KEY2 == 1 ) || ( KEY3 == 1 ) || ( KEY4 == 1 ) ))
	{
		key_flag = 1;
		SysTick_delay_ms(40);
		if( KEY1 == 1 ) return 1;
		else if( KEY2 == 1 ) return 2;
		else if( KEY3 == 1 ) return 3;
		else if( KEY4 == 1 ) return 4;
	}
	else if( ( KEY1 == 0 ) && ( KEY2 == 0 ) && ( KEY3 == 0) && ( KEY4 == 0) )
	{
		key_flag = 0;
	}
	return 0;
}

