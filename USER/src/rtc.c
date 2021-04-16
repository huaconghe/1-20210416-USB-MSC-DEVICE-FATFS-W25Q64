#include "rtc.h"


dateAndTime_t  rtc_dev={18,12,13,5,20,32,9};
void rtc_init(void)
{
	RCC->APB1ENR |=1<<28;	//ʹ��PWR��Դ������ʱ��
	PWR->CR |=1<<8;//DBPλ��1ȥ��RTC�Ĵ�����д����
	
	//����LSI��ΪRTC��ʱ��
	RCC->CSR |=1<<0;//����LSI����
	while( !(RCC->CSR & (1<<1)));//�ȴ�LSI��������
	RCC->BDCR &=~(3<<8);	//����
	RCC->BDCR |=2<<8;//ѡ��LSI��ʱ��
	RCC->BDCR |=1<<15;//rtcʱ��ʹ��
	RTC->WPR =0XCA;
	RTC->WPR =0X53;//ȥ������RTC�Ĵ�����д����
	
	if(RTC->ISR & (1<<4))
	{
		;
	}
	else 
	{
	//RTC��ʼ��������	
	RTC->ISR |=1<<7;//����RTC��ʼ��ģʽ
	while( !(RTC->ISR & (1<<6)));//�ȴ������ʼ��ģʽ
	RTC->PRER |=0XFF<<0;//ͬ��Ԥ��Ƶ256
	RTC->PRER |=0X7F<<16;//128Ԥ��Ԥ��Ƶ
	RTC->CR &=~(1<<6);//24Сʱ��ʽ
	RTC->CR &=~(1<<5);//ʹ������Ӱ�ӼĴ���
	}
	
	RTC->ISR &=~(1<<7);//�˳�RTC��ʼ��ģʽ
	RTC->WPR        = 0;      //д���������ֵ�������
	PWR->CR        &=~(1<<8); //ʹ��RTCд����
	rtc_set(&rtc_dev);
}


u8 rtc_set(dateAndTime_t *rtc)
{
	u32 date=0;
	u32 time=0;
	if(  (rtc->year > 99)  || (rtc->month > 12) || (rtc->day > 31) || (rtc->week > 7) 
	     ||(rtc->hour>23)  ||(rtc->minute > 59) || (rtc->second > 59) )
	    return 0;  //������������ʧ�ܡ�
	PWR->CR        |=1<<8;   //���RTCд����
	/* RTCʱ��Դ���� */
	RTC->WPR        = 0xCA;
	RTC->WPR        = 0x53;   //�������RTC�Ĵ���д����
	//RTC��ʼ��������	
	RTC->ISR       |=1<<7;    //ʹ�ܳ�ʼ��ģʽ
	while( ( RTC->ISR &(1<<6) )==0 ); //�ȴ������ʼ��ģʽ
	//����RTCʱ�������
	time |=dectobcd(rtc->hour)<<16;//����Сʱ
	time |=dectobcd(rtc->minute)<<8;//���÷���
	time |=dectobcd(rtc->second)<<0;//������
	RTC->TR =time;	 
	date |=dectobcd(rtc->year)<<16;//������
	date |=dectobcd(rtc->month)<<8;//������
	date |=dectobcd(rtc->day)<<0;//������
	date |=dectobcd(rtc->week)<<13;//��������	
	RTC->DR =date;
	
	RTC->ISR       &=~(1<<7); //�˳���ʼ��ģʽ
	RTC->WPR        = 0;      //д���������ֵ�������
	PWR->CR        &=~(1<<8); //ʹ��RTCд����	
	return 1;
}


void rtc_get(dateAndTime_t*rtc)
{
	u32 date=0;
	u32 time=0;
	while(!(RTC->ISR & (1<<5)));//�ȴ�Ӱ�ӼĴ���ͬ��
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


