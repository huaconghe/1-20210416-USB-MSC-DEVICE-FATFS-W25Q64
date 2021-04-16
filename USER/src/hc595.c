#include "hc595.h"
#include "systick.h"
#include "w25q64.h"
#include <string.h>
#include <stdio.h>
#include "led.h"
#include "key.h"
#include "english_font.h"
#include "rtc.h"
#include "key.h"

#define us 1
#define ms 1

u8 r_buff[100][33] = {0};


/*****************************************************************************************
*�������ܣ�IO�ڳ�ʼ��
*�����β�:��
*��������ֵ����
*������ע:PE14--LK_SCK	PE12--LK_SI	PE10--LK_OE	PE8--LK_CS
*
*****************************************************************************************/

void GPIO_INIT(void)
{
	RCC->AHB1ENR |=1<<4;//��PE��ʱ��
	
	//PE14--LK_SCK
	GPIOE->MODER &=~(3<<(2*14));//����
	GPIOE->MODER |=1<<(2*14);//����Ϊ���ģʽ
	GPIOE->OTYPER &=~(1<<14);//����ģʽ
	GPIOE->OSPEEDR &=~(3<<(2*14));//����
//	GPIOE->OSPEEDR |=1<<(2*14);//25M����ٶ�
	SHCP_L;//ʱ���߳�ʼ������
	
	//PE12--LK_SI
	GPIOE->MODER &=~(3<<(2*12));//����
	GPIOE->MODER |=1<<(2*12);//����Ϊ���ģʽ
	GPIOE->OTYPER &=~(1<<12);//����ģʽ
	GPIOE->OSPEEDR &=~(3<<(2*12));//����
//	GPIOE->OSPEEDR |=1<<(2*12);//25M����ٶ�

	//PE10--LK_OE
	GPIOE->MODER &=~(3<<(2*10));//����
	GPIOE->MODER |=1<<(2*10);//����Ϊ���ģʽ
	GPIOE->OTYPER &=~(1<<10);//����ģʽ
	GPIOE->OSPEEDR &=~(3<<(2*10));//����
//	GPIOE->OSPEEDR |=1<<(2*10);//25M����ٶ�
	OE_H;//595ʹ��λ����
	
	//PE8--LK_CS
	GPIOE->MODER &=~(3<<(2*8));//����
	GPIOE->MODER |=1<<(2*8);//����Ϊ���ģʽ
	GPIOE->OTYPER &=~(1<<8);//����ģʽ
	GPIOE->OSPEEDR &=~(3<<(2*8));//����
//	GPIOE->OSPEEDR |=1<<(2*8);//25M����ٶ�
	STCP_H;//Ƭѡ����
	
}

/*****************************************************************************************
*�������ܣ��ֱ���74HC595�����к��п��Ƶ�16λ���ݣ�һ��32λ
*�����β�:��
*��������ֵ����
*������ע:PE14--LK_SCK	PE12--LK_SI	PE10--LK_OE	PB8--LK_CS
*
*****************************************************************************************/
void out595_datdu(uchar du0, uchar du1, uchar HBIT, uchar LBIT ) 
{
	uchar i,j,k,z;
	STCP_L;
	SHCP_L;
	for (i = 0; i < 8; i++)//����8λ��λ�п����ź�
	{
		SysTick_delay_us(us);
		if (du0 & 0x80)
		{
			DATE_H;
		}
		else  
		DATE_L;  
		
		SHCP_H;
		SysTick_delay_us(us);
		SHCP_L;
		du0 <<= 1;
  }
	for (j = 0; j < 8; j++)//����8λ��λ�п����ź�
	{
		SysTick_delay_us(us);
		if (du1 & 0x80)
		{
			DATE_H;
		}
		else  
		DATE_L;  
		
		SHCP_H;
		SysTick_delay_us(us);
		SHCP_L;
		du1 <<= 1;
  }

	for (k = 0; k < 8; k++)//����8λ��λ�п����ź�
	{
		SysTick_delay_us(us);
		if (HBIT & 0x80)
		{
			DATE_H;
		}
		else  
		DATE_L;  
		
		SHCP_H;
		SysTick_delay_us(us);
		SHCP_L;
		HBIT<<=1;
	}
	for (z = 0; z < 8; z++)//����8λ�е�λ�����ź�
	{
		SHCP_L;
		SysTick_delay_us(us);
		if (LBIT & 0x80)
		{
			DATE_H;
		}
		else  
		DATE_L;  
		
		SHCP_H;
		SysTick_delay_us(us);
		SHCP_L;
		LBIT<<=1;
  }
}
/*****************************************************************************************
*�������ܣ���ʾ4������
*�����β�:��
*��������ֵ����
*������ע:
*
*****************************************************************************************/	
void show_Chinese(void)
{
    unsigned char i,j;
    unsigned short int tmp; 
 
		GPIO_INIT();
		OE_L;
		
    while(1)
    {
        tmp = 1;
        for(j=0;j<16;j++)
        {
            for(i=0;i<4;i++)out595_datdu(r_buff[i][j*2+1], r_buff[i][j*2],~(tmp>>8),~tmp);//
            tmp <<= 1;
						STCP_H;
						SysTick_delay_us(500);
						STCP_L;
        }
    }
}

/*****************************************************************************************
*�������ܣ���ʾһ�麺��,ÿһ��4������
*�����β�:��
*��������ֵ��buff--һ�麺�ֵ��׵�ַ
*������ע:
*
*****************************************************************************************/	
void show_many_Chinese(u8*buff)
{
    int i=0,j=0;
    unsigned short int tmp; 
		u32 addr = 0;
		int num = 0;
		int lest_num = 0;
		int last =0;
		u8 key = 0;

		
		/*�����ʾ����*/
		GPIO_INIT();
		OE_L;			
    while(1)
    {
			/*��ģ��ȡ*/
			for(i=0;i< ( strlen((char*)buff) )/2;i++)
			{
				addr  = (buff[i*2]-0xA1)*94+(buff[i*2+1]-0xA1); // ��GB2312������е�λ��
				addr  =addr*32;  // ��W25Q64���е�ȡģ���ݵĻ���ַ
				w25q64_read(addr,r_buff[i],32);//��w25q42��ȡ��ģ��r_buff
			}
			
			last = ( strlen((char*)buff) )/2;
			if( (last % 4) != 0 )
			{
				for( i = last; i < (last + ( 4 - last % 4 )) ; i++ )
				{
					for(j=0;j<32;j++)
					r_buff[i][j] = 0xff;
				}			
			}			
			
			key =key_scan();
			if( key == 4 ) 
			{
				num+=4;//�������£��л���ʾ��һ��4������
			}
			lest_num = strlen((char*)buff)/2 - num;//ʣ��û��ʾ�ĺ��ָ���
			//printf("buff :%d  num :%d  lest_num%d \r\n",strlen((char*)buff),num,lest_num);
			
			if( lest_num <= 0 )//ʣ�����һ�У����ؿ�ʼ
			{
				num = 0;
			}
		
			
			tmp = 1;
			for(j=0;j<16;j++)
			{
					for(i=3+num;i>=0+num;i--)out595_datdu(r_buff[i][j*2+1], r_buff[i][j*2],~(tmp>>8),~tmp);//��4�����ֵ�ÿһ����ģ�ֱ��͵�hc595
					tmp <<= 1;
					STCP_H;
					SysTick_delay_us(10);//hc595��Ҫ1΢�����ϵ�����ʱ�䣬Ҫ��Ȼ�������
					STCP_L;
			}
			if( key == 1) break;
    }
}

/*****************************************************************************************
*�������ܣ���ʾ�ַ���
*�����β�:��
*��������ֵ��buff--�ַ������׵�ַ
*������ע:
*
*****************************************************************************************/	
void show_string(u8* English_buff)
{
    int i=0,j=0;
    unsigned short int tmp; 
		u32 addr0 = 0;
		u32 addr1= 0;
		int num = 0;
		int lest_num = 0;
		u8 last= 0;
		u8 key = 0;

		/*�����ʾ����*/
		GPIO_INIT();
		OE_L;			
    while(1)
    {
			/*��ģ��ȡ*/
			last = strlen((char*)English_buff);//�ַ�����С
			for( j = 0; j < last ; j += 2 )//ȡÿ���ڵ������ַ���һ��ģ��Ϊһ�����
			{
				addr0 = (English_buff[j] - 32)*16;//�ַ�ȡģ���ݻ�ַ��ַ
				for( i = 0; i < 32; i+=2)
				{
					r_buff[j/2][i] = English_fonts[addr0];
					addr0++;
				}
				addr1 = (English_buff[j+1] - 32)*16;//�ַ�ȡģ���ݻ�ַ��ַ
				for( i = 1; i < 32; i+=2)
				{
					r_buff[j/2][i] = English_fonts[addr1];
					addr1++;
				}
					//for(i=0;i<32;i++) printf("0x%x ",r_buff[0][i]);
			}
			if( ( last % 2 ) != 0 )	//�ַ��������ǵ���
			{
					for( i = 1; i < 32; i+=2)//�ַ�������Ϊ����,�������һ���ַ���ȡģ���ݼ�0xff
					{
						r_buff[last/2][i] = 0xff;
					}	
					last = (last+1) / 2;
					if( (last/2 % 4) != 0 )
					{
						for( i = last; i < (last + ( 4 - last % 4 )) ; i++ )
						{
							for(j=0;j<32;j++)
							r_buff[i][j] = 0xff;
						}			
					}			
			}			
			
			key =key_scan();
			if( key == 4 ) 
			{
				num+=4;//�������£��л���ʾ��һ��4������
			}
			lest_num = strlen((char*)English_buff)/2 - num;//ʣ��û��ʾ�ĺ��ָ���
			//printf("buff :%d  num :%d  lest_num%d \r\n",strlen((char*)buff),num,lest_num);
			
			if( lest_num <= 0 )//ʣ�����һ�У����ؿ�ʼ
			{
				num = 0;
			}

      tmp = 1;
      for(j=0;j<16;j++)
      {
          for(i=3+num;i>=0+num;i--)out595_datdu(r_buff[i][j*2+1], r_buff[i][j*2],~(tmp>>8),~tmp);//��4�����ֵ�ÿһ����ģ�ֱ��͵�hc595
          tmp <<= 1;
					STCP_H;
					SysTick_delay_us(10);//hc595��Ҫ1΢�����ϵ�����ʱ�䣬Ҫ��Ȼ�������
					STCP_L;
      }
			if( key == 1) break;
    }
}

/*****************************************************************************************
*�������ܣ���ʾʱ��
*�����β�:��
*��������ֵ���ַ������׵�ַ
*������ע:
*
*****************************************************************************************/	
void show_time(void)
{
    int i=0,j=0;
    unsigned short int tmp; 
		u32 addr0 = 0;
		u32 addr1= 0;
		int num = 0;
		int lest_num = 0;
		u8 last= 0;
		u8 key = 0;
		u8 time_buff[100]="0";

		/*�����ʾ����*/
		GPIO_INIT();
		OE_L;			
    while(1)
    {
			rtc_get(&rtc_dev);//��ȡʱ������
			sprintf((char*)time_buff,"%02d:%02d:%02d",rtc_dev.hour,rtc_dev.minute,rtc_dev.second);//��ȡʱ��
				/*��ģ��ȡ*/
			last = strlen((char*)time_buff);//�ַ�����С
			for( j = 0; j < last ; j += 2 )//ȡÿ���ڵ������ַ���һ��ģ��Ϊһ�����
			{
					addr0 = (time_buff[j] - 32)*16;//�ַ�ȡģ���ݻ�ַ��ַ
					for( i = 0; i < 32; i+=2)
					{
						r_buff[j/2][i] = English_fonts[addr0];
						addr0++;
					}
					addr1 = (time_buff[j+1] - 32)*16;//�ַ�ȡģ���ݻ�ַ��ַ
					for( i = 1; i < 32; i+=2)
					{
						r_buff[j/2][i] = English_fonts[addr1];
						addr1++;
					}
					//for(i=0;i<32;i++) printf("0x%x ",r_buff[0][i]);
			}
			if( ( last % 2 ) != 0 )	//�ַ��������ǵ���
			{
					for( i = 1; i < 32; i+=2)//�ַ�������Ϊ����,�������һ���ַ���ȡģ���ݼ�0xff
					{
						r_buff[last/2][i] = 0xff;
					}	
					last = (last+1) / 2;
					if( (last/2 % 4) != 0 )
					{
						for( i = last; i < (last + ( 4 - last % 4 )) ; i++ )
						{
							for(j=0;j<32;j++)
							r_buff[i][j] = 0xff;
						}			
					}			
			}	
			key =key_scan();
			if( key == 4 ) 
			{
				num+=4;//�������£��л���ʾ��һ��4������
			}
			lest_num = strlen((char*)time_buff)/2 - num;//ʣ��û��ʾ�ĺ��ָ���
			//printf("buff :%d  num :%d  lest_num%d \r\n",strlen((char*)buff),num,lest_num);
			
			if( lest_num <= 0 )//ʣ�����һ�У����ؿ�ʼ
			{
				num = 0;
			}
	
			
      tmp = 1;
      for(j=0;j<16;j++)
      {
          for(i=3+num;i>=0+num;i--)out595_datdu(r_buff[i][j*2+1], r_buff[i][j*2],~(tmp>>8),~tmp);//��4�����ֵ�ÿһ����ģ�ֱ��͵�hc595
          tmp <<= 1;
					STCP_H;
					SysTick_delay_us(1);//hc595��Ҫ1΢�����ϵ�����ʱ�䣬Ҫ��Ȼ�������
					STCP_L;
      }
			if( key == 1) break;
    }
}

