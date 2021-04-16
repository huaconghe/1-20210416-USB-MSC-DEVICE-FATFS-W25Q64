#include "rtc.h"


dateAndTime_t  rtc_dev={18,12,13,5,20,32,9};
void rtc_init(void)
{
	RCC->APB1ENR |=1<<28;	//使能PWR电源控制器时钟
	PWR->CR |=1<<8;//DBP位置1去除RTC寄存器的写访问
	
	//配置LSI作为RTC的时钟
	RCC->CSR |=1<<0;//开启LSI振荡器
	while( !(RCC->CSR & (1<<1)));//等待LSI振荡器就绪
	RCC->BDCR &=~(3<<8);	//清零
	RCC->BDCR |=2<<8;//选择LSI振荡时钟
	RCC->BDCR |=1<<15;//rtc时钟使能
	RTC->WPR =0XCA;
	RTC->WPR =0X53;//去除所有RTC寄存器的写保护
	
	if(RTC->ISR & (1<<4))
	{
		;
	}
	else 
	{
	//RTC初始化和配置	
	RTC->ISR |=1<<7;//开启RTC初始化模式
	while( !(RTC->ISR & (1<<6)));//等待进入初始化模式
	RTC->PRER |=0XFF<<0;//同步预分频256
	RTC->PRER |=0X7F<<16;//128预步预分频
	RTC->CR &=~(1<<6);//24小时格式
	RTC->CR &=~(1<<5);//使用日历影子寄存器
	}
	
	RTC->ISR &=~(1<<7);//退出RTC初始化模式
	RTC->WPR        = 0;      //写入任意错误值，激活保护
	PWR->CR        &=~(1<<8); //使能RTC写保护
	rtc_set(&rtc_dev);
}


u8 rtc_set(dateAndTime_t *rtc)
{
	u32 date=0;
	u32 time=0;
	if(  (rtc->year > 99)  || (rtc->month > 12) || (rtc->day > 31) || (rtc->week > 7) 
	     ||(rtc->hour>23)  ||(rtc->minute > 59) || (rtc->second > 59) )
	    return 0;  //参数出错，设置失败。
	PWR->CR        |=1<<8;   //解除RTC写保护
	/* RTC时钟源配置 */
	RTC->WPR        = 0xCA;
	RTC->WPR        = 0x53;   //解除所有RTC寄存器写保护
	//RTC初始化和配置	
	RTC->ISR       |=1<<7;    //使能初始化模式
	while( ( RTC->ISR &(1<<6) )==0 ); //等待进入初始化模式
	//设置RTC时间和日期
	time |=dectobcd(rtc->hour)<<16;//设置小时
	time |=dectobcd(rtc->minute)<<8;//设置分钟
	time |=dectobcd(rtc->second)<<0;//设置秒
	RTC->TR =time;	 
	date |=dectobcd(rtc->year)<<16;//设置年
	date |=dectobcd(rtc->month)<<8;//设置月
	date |=dectobcd(rtc->day)<<0;//设置日
	date |=dectobcd(rtc->week)<<13;//设置星期	
	RTC->DR =date;
	
	RTC->ISR       &=~(1<<7); //退出初始化模式
	RTC->WPR        = 0;      //写入任意错误值，激活保护
	PWR->CR        &=~(1<<8); //使能RTC写保护	
	return 1;
}


void rtc_get(dateAndTime_t*rtc)
{
	u32 date=0;
	u32 time=0;
	while(!(RTC->ISR & (1<<5)));//等待影子寄存器同步
	date = RTC->DR;  
	time = RTC->TR; 
	rtc->year  = bcdtodec( (date>>16)&0xFF);
	rtc->month = bcdtodec( (date>>8)&0x1F );
	rtc->day   = bcdtodec( (date>>0)&0x3F );
	rtc->week  = bcdtodec( (date>>13)&0x7 );
	
	rtc->hour   = bcdtodec( (time>>16)&0x3F );
	rtc->minute = bcdtodec( (time>>8)&0x7F );
	rtc->second = bcdtodec( (time>>0)&0x7F );	

}



u8 dectobcd(u8 dec)
{
	u8 BCD=0;
	while( dec/10 )
	{
		dec -=10;
		BCD++;
	}
	BCD = BCD*16+dec;
	return BCD;	
}


u8 bcdtodec(u8 bcd)
{
	u8 DEC=0;
	while( bcd/16 )
	{
		bcd -=16;
		DEC ++;
	}
	DEC = DEC*10+bcd;
	return DEC;	
}


