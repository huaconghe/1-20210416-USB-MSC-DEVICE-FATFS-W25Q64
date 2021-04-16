#include "ili9486.h"
#include "systick.h"

/******************��ӯ��*****************************
�������ܣ�ili9486��MCU�����ӹܽ�����
�����βΣ���
��������ֵ����
������ע: 
CS:PD7  WR:PD5  RS:PD6  RD:PD4	//������� Ĭ��״̬Ϊ�ߵ�ƽ
BL:PB15  D0:PD14  D1:PD15  D2:PD0
D3:PD1   D4:PE7   D5:PE8   D6:PE9
D7:PE10  D8:PE11  D9:PE12  D10:PE13
D11:PE14 D12:PE15  D13:PD8 D14:PD9
D15:PD10						//���ó��������  Ĭ��״̬Ϊ�͵�ƽ
******************************************************/
void ILI9486_Port_Init(void)
{
	RCC->AHB1ENR |= 1 << 6 | 1 << 3 | 1 << 5 | 1 << 1 | 1 << 4;	//��GDFBE����IO��ʱ��
	
	/*CS*/
	GPIOD->MODER &= ~(3ul << (2 * 7));				//����
	GPIOD->MODER |= 1ul << (2 * 7);				//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 7));				//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 7);				//���óɸ���
	LCD_CS_H;										//CS����
	
	/*WR*/
	GPIOD->MODER &= ~(3ul << (2 * 5));				//����
	GPIOD->MODER |= 1ul << (2 * 5);					//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 5));				//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 5);					//���óɸ���
	LCD_WR_H;										//WR����
	
	/*RS*/
	GPIOD->MODER &= ~(3ul << (2 * 6));				//����
	GPIOD->MODER |= 1ul << (2 * 6);					//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 6));				//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 6);					//���óɸ���
	LCD_RS_H;	

	/*RD*/
	GPIOD->MODER &= ~(3ul << (2 * 4));				//����
	GPIOD->MODER |= 1ul << (2 * 4);					//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 4));				//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 4);				//���óɸ���
	LCD_RD_H;										//WR����

	
	/*BL*/
	GPIOB->MODER &= ~(3ul << (2 * 15));				//����
	GPIOB->MODER |= 1ul << (2 * 15);				//���ó����ģʽ
	GPIOB->OTYPER &= ~(1ul << (1 * 15));			//���ó�����
	GPIOB->OSPEEDR |= 3ul << (2 * 15);				//���óɸ���

	/*D0~D15*/
	GPIOD->MODER &= ~(3ul << (2 * 14));				//����
	GPIOD->MODER |= 1ul << (2 * 14);				//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 14));			//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 14);				//���óɸ���
	
	GPIOD->MODER &= ~(3ul << (2 * 15));				//����
	GPIOD->MODER |= 1ul << (2 * 15);				//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 15));			//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 15);				//���óɸ���
	
	GPIOD->MODER &= ~(3ul << (2 * 0));				//����
	GPIOD->MODER |= 1ul << (2 * 0);					//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 0));				//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 0);				//���óɸ���
	
	GPIOD->MODER &= ~(3ul << (2 * 1));				//����
	GPIOD->MODER |= 1ul << (2 * 1);					//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 1));				//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 1);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 7));				//����
	GPIOE->MODER |= 1ul << (2 * 7);					//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 7));				//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 7);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 8));				//����
	GPIOE->MODER |= 1ul << (2 * 8);					//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 8));				//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 8);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 9));				//����
	GPIOE->MODER |= 1ul << (2 * 9);					//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 9));				//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 9);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 10));				//����
	GPIOE->MODER |= 1ul << (2 * 10);				//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 10));			//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 10);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 11));				//����
	GPIOE->MODER |= 1ul << (2 * 11);				//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 11));			//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 11);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 12));				//����
	GPIOE->MODER |= 1ul << (2 * 12);				//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 12));			//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 12);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 13));				//����
	GPIOE->MODER |= 1ul << (2 * 13);				//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 13));			//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 13);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 14));				//����
	GPIOE->MODER |= 1ul << (2 * 14);				//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 14));			//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 14);				//���óɸ���
	
	GPIOE->MODER &= ~(3ul << (2 * 15));				//����
	GPIOE->MODER |= 1ul << (2 * 15);				//���ó����ģʽ
	GPIOE->OTYPER &= ~(1ul << (1 * 15));			//���ó�����
	GPIOE->OSPEEDR |= 3ul << (2 * 15);				//���óɸ���
	
	GPIOD->MODER &= ~(3ul << (2 * 8));				//����
	GPIOD->MODER |= 1ul << (2 * 8);					//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 8));				//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 8);				//���óɸ���
	
	GPIOD->MODER &= ~(3ul << (2 * 9));				//����
	GPIOD->MODER |= 1ul << (2 * 9);					//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 9));				//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 9);				//���óɸ���
	
	GPIOD->MODER &= ~(3ul << (2 * 10));				//����
	GPIOD->MODER |= 1ul << (2 * 10);				//���ó����ģʽ
	GPIOD->OTYPER &= ~(1ul << (1 * 10));			//���ó�����
	GPIOD->OSPEEDR |= 3ul << (2 * 10);				//���óɸ���
}

/******************��ӯ��*****************************
�������ܣ���16λ����ת���16�����������
�����βΣ�data:Ҫ�����16λ����
��������ֵ����
������ע: 
CS:PG12  WR:PD5  RS:PD6  RD:PD4	//������� Ĭ��״̬Ϊ�ߵ�ƽ
BL:PB15  D0:PD14  D1:PD15  D2:PD0
D3:PD1   D4:PE7   D5:PE8   D6:PE9
D7:PE10  D8:PE11  D9:PE12  D10:PE13
D11:PE14 D12:PE15  D13:PD8 D14:PD9
D15:PD10						//���ó��������  Ĭ��״̬Ϊ�͵�ƽ
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

/******************��ӯ��*****************************
�������ܣ���������
�����βΣ�cmd:����
��������ֵ����
������ע: 
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

/******************��ӯ��*****************************
�������ܣ���������
�����βΣ�data:����
��������ֵ����
������ע: 
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

/******************��ӯ��*****************************
�������ܣ�LCD��������
�����βΣ�sx:��ʼ�е�ַ  ex:�����е�ַ  sy:��ʼ�е�ַ ey:�����е�ַ  color:������ɫ
��������ֵ����
������ע: 
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

/******************��ӯ��*****************************
�������ܣ�LCD��ʼ������
�����βΣ���
��������ֵ����
������ע: ��������ɳ����ṩ
******************************************************/
void ILI9486_Init(void)
{
	ILI9486_Port_Init();				//��ʼ��GPIO��
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
	LCD_Send_Data(0x08);   //����RGB,�����ߵ���
//	LCD_Send_Data(0x00);   //����RGB���������ߵ���BGR
	LCD_Send_CMD(0x3A);
	LCD_Send_Data(0x05);   //����16λBPP
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
	LCD_Send_CMD(0x11);   //�˳�˯��
	SysTick_delay_ms(120);
	LCD_Send_CMD(0x29);   //������ʾ

	LCD_Send_CMD(0x36); //����ɨ�跽��
	LCD_Send_Data(0x08);//����ɨ������Ҵ��ϵ���,ѡ��RGB��ʽ
	LCD_Clear(0,319,0,479,0xffff);//0001000100010001   32 64 136
	GPIOB->BSRRL = 1 << 15;//�򿪱����
}


