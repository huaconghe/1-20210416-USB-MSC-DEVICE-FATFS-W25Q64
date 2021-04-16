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
*函数功能：IO口初始化
*函数形参:无
*函数返回值：无
*函数备注:PE14--LK_SCK	PE12--LK_SI	PE10--LK_OE	PE8--LK_CS
*
*****************************************************************************************/

void GPIO_INIT(void)
{
	RCC->AHB1ENR |=1<<4;//打开PE组时钟
	
	//PE14--LK_SCK
	GPIOE->MODER &=~(3<<(2*14));//清零
	GPIOE->MODER |=1<<(2*14);//配置为输出模式
	GPIOE->OTYPER &=~(1<<14);//推挽模式
	GPIOE->OSPEEDR &=~(3<<(2*14));//清零
//	GPIOE->OSPEEDR |=1<<(2*14);//25M输出速度
	SHCP_L;//时钟线初始化拉低
	
	//PE12--LK_SI
	GPIOE->MODER &=~(3<<(2*12));//清零
	GPIOE->MODER |=1<<(2*12);//配置为输出模式
	GPIOE->OTYPER &=~(1<<12);//推挽模式
	GPIOE->OSPEEDR &=~(3<<(2*12));//清零
//	GPIOE->OSPEEDR |=1<<(2*12);//25M输出速度

	//PE10--LK_OE
	GPIOE->MODER &=~(3<<(2*10));//清零
	GPIOE->MODER |=1<<(2*10);//配置为输出模式
	GPIOE->OTYPER &=~(1<<10);//推挽模式
	GPIOE->OSPEEDR &=~(3<<(2*10));//清零
//	GPIOE->OSPEEDR |=1<<(2*10);//25M输出速度
	OE_H;//595使能位拉高
	
	//PE8--LK_CS
	GPIOE->MODER &=~(3<<(2*8));//清零
	GPIOE->MODER |=1<<(2*8);//配置为输出模式
	GPIOE->OTYPER &=~(1<<8);//推挽模式
	GPIOE->OSPEEDR &=~(3<<(2*8));//清零
//	GPIOE->OSPEEDR |=1<<(2*8);//25M输出速度
	STCP_H;//片选拉高
	
}

/*****************************************************************************************
*函数功能：分别向74HC595发送行和列控制的16位数据，一共32位
*函数形参:无
*函数返回值：无
*函数备注:PE14--LK_SCK	PE12--LK_SI	PE10--LK_OE	PB8--LK_CS
*
*****************************************************************************************/
void out595_datdu(uchar du0, uchar du1, uchar HBIT, uchar LBIT ) 
{
	uchar i,j,k,z;
	STCP_L;
	SHCP_L;
	for (i = 0; i < 8; i++)//发送8位高位列控制信号
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
	for (j = 0; j < 8; j++)//发送8位低位列控制信号
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

	for (k = 0; k < 8; k++)//发送8位高位行控制信号
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
	for (z = 0; z < 8; z++)//发送8位行低位控制信号
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
*函数功能：显示4个汉字
*函数形参:无
*函数返回值：无
*函数备注:
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
*函数功能：显示一组汉字,每一行4个汉字
*函数形参:无
*函数返回值：buff--一组汉字的首地址
*函数备注:
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

		
		/*输出显示文字*/
		GPIO_INIT();
		OE_L;			
    while(1)
    {
			/*字模读取*/
			for(i=0;i< ( strlen((char*)buff) )/2;i++)
			{
				addr  = (buff[i*2]-0xA1)*94+(buff[i*2+1]-0xA1); // 在GB2312编码表当中的位置
				addr  =addr*32;  // 在W25Q64当中的取模数据的基地址
				w25q64_read(addr,r_buff[i],32);//从w25q42读取字模到r_buff
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
				num+=4;//按键按下，切换显示下一行4个汉字
			}
			lest_num = strlen((char*)buff)/2 - num;//剩下没显示的汉字个数
			//printf("buff :%d  num :%d  lest_num%d \r\n",strlen((char*)buff),num,lest_num);
			
			if( lest_num <= 0 )//剩余最后一行，返回开始
			{
				num = 0;
			}
		
			
			tmp = 1;
			for(j=0;j<16;j++)
			{
					for(i=3+num;i>=0+num;i--)out595_datdu(r_buff[i][j*2+1], r_buff[i][j*2],~(tmp>>8),~tmp);//把4个汉字的每一行字模分别发送到hc595
					tmp <<= 1;
					STCP_H;
					SysTick_delay_us(10);//hc595需要1微妙以上的锁存时间，要不然输出乱码
					STCP_L;
			}
			if( key == 1) break;
    }
}

/*****************************************************************************************
*函数功能：显示字符串
*函数形参:无
*函数返回值：buff--字符串的首地址
*函数备注:
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

		/*输出显示文字*/
		GPIO_INIT();
		OE_L;			
    while(1)
    {
			/*字模读取*/
			last = strlen((char*)English_buff);//字符串大小
			for( j = 0; j < last ; j += 2 )//取每相邻的两个字符的一个模作为一行输出
			{
				addr0 = (English_buff[j] - 32)*16;//字符取模数据基址地址
				for( i = 0; i < 32; i+=2)
				{
					r_buff[j/2][i] = English_fonts[addr0];
					addr0++;
				}
				addr1 = (English_buff[j+1] - 32)*16;//字符取模数据基址地址
				for( i = 1; i < 32; i+=2)
				{
					r_buff[j/2][i] = English_fonts[addr1];
					addr1++;
				}
					//for(i=0;i<32;i++) printf("0x%x ",r_buff[0][i]);
			}
			if( ( last % 2 ) != 0 )	//字符串个数是单数
			{
					for( i = 1; i < 32; i+=2)//字符串个数为单数,则在最后一个字符的取模数据加0xff
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
				num+=4;//按键按下，切换显示下一行4个汉字
			}
			lest_num = strlen((char*)English_buff)/2 - num;//剩下没显示的汉字个数
			//printf("buff :%d  num :%d  lest_num%d \r\n",strlen((char*)buff),num,lest_num);
			
			if( lest_num <= 0 )//剩余最后一行，返回开始
			{
				num = 0;
			}

      tmp = 1;
      for(j=0;j<16;j++)
      {
          for(i=3+num;i>=0+num;i--)out595_datdu(r_buff[i][j*2+1], r_buff[i][j*2],~(tmp>>8),~tmp);//把4个汉字的每一行字模分别发送到hc595
          tmp <<= 1;
					STCP_H;
					SysTick_delay_us(10);//hc595需要1微妙以上的锁存时间，要不然输出乱码
					STCP_L;
      }
			if( key == 1) break;
    }
}

/*****************************************************************************************
*函数功能：显示时间
*函数形参:无
*函数返回值：字符串的首地址
*函数备注:
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

		/*输出显示文字*/
		GPIO_INIT();
		OE_L;			
    while(1)
    {
			rtc_get(&rtc_dev);//获取时间日历
			sprintf((char*)time_buff,"%02d:%02d:%02d",rtc_dev.hour,rtc_dev.minute,rtc_dev.second);//获取时间
				/*字模读取*/
			last = strlen((char*)time_buff);//字符串大小
			for( j = 0; j < last ; j += 2 )//取每相邻的两个字符的一个模作为一行输出
			{
					addr0 = (time_buff[j] - 32)*16;//字符取模数据基址地址
					for( i = 0; i < 32; i+=2)
					{
						r_buff[j/2][i] = English_fonts[addr0];
						addr0++;
					}
					addr1 = (time_buff[j+1] - 32)*16;//字符取模数据基址地址
					for( i = 1; i < 32; i+=2)
					{
						r_buff[j/2][i] = English_fonts[addr1];
						addr1++;
					}
					//for(i=0;i<32;i++) printf("0x%x ",r_buff[0][i]);
			}
			if( ( last % 2 ) != 0 )	//字符串个数是单数
			{
					for( i = 1; i < 32; i+=2)//字符串个数为单数,则在最后一个字符的取模数据加0xff
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
				num+=4;//按键按下，切换显示下一行4个汉字
			}
			lest_num = strlen((char*)time_buff)/2 - num;//剩下没显示的汉字个数
			//printf("buff :%d  num :%d  lest_num%d \r\n",strlen((char*)buff),num,lest_num);
			
			if( lest_num <= 0 )//剩余最后一行，返回开始
			{
				num = 0;
			}
	
			
      tmp = 1;
      for(j=0;j<16;j++)
      {
          for(i=3+num;i>=0+num;i--)out595_datdu(r_buff[i][j*2+1], r_buff[i][j*2],~(tmp>>8),~tmp);//把4个汉字的每一行字模分别发送到hc595
          tmp <<= 1;
					STCP_H;
					SysTick_delay_us(1);//hc595需要1微妙以上的锁存时间，要不然输出乱码
					STCP_L;
      }
			if( key == 1) break;
    }
}

