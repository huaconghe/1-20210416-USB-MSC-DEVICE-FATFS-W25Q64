#include "w25q64.h"
#include "spi.h"
#include <stdio.h>
#include <string.h>

/*****************************************************************************************
*�������ܣ���ȡw25q64����ID
*�����β�:��
*��������ֵ������ID
*������ע:
*
*****************************************************************************************/
u16 w25q64_read_ID(void)
{
	u16 device_id=0;
	Flash_CS_L;//����Ƭѡ
	SPI1_Send_Read(0x90);//���Ͷ�idָ��
	SPI1_Send_Read(0x00);
	SPI1_Send_Read(0x00);
	SPI1_Send_Read(0x00);//����24λ��ַ
	device_id |= SPI1_Send_Read(0xFF);//��ȡ��8λ
	device_id <<=8;
	device_id |= SPI1_Send_Read(0xFF);//��ȡ��8λ
	Flash_CS_H;//����Ƭѡ
	return device_id;
}


/*****************************************************************************************
*�������ܣ���ȡw25q64״̬�Ĵ�����ֵ
*�����β�:��
*��������ֵ��״̬�Ĵ�����ֵ
*������ע:
*
*****************************************************************************************/
u8 w25q64_read_status(void)
{
	u8 status = 0;
	Flash_CS_L;//����Ƭѡ
	SPI1_Send_Read(0x05);//���Ͷ�״ָ̬��
	status = SPI1_Send_Read(0x00);//�����������ݶ�ȡ״̬�Ĵ�����ֵ
	Flash_CS_H;//����Ƭѡ
	return status;
}

/*****************************************************************************************
*�������ܣ�дʹ��
*�����β�:��
*��������ֵ����
*������ע:
*
*****************************************************************************************/
void w25q64_write_enable(void)
{
	Flash_CS_L;//����Ƭѡ
	SPI1_Send_Read(0x06);//����дʹ��ָ��
	Flash_CS_H;//����Ƭѡ
}

/*****************************************************************************************
*�������ܣ������߳�
*�����β�:��
*��������ֵ����
*������ע:8MB=128�飨64KB��   1��=16������4KB��   1����=16ҳ��256B��
*
*****************************************************************************************/
u8 w25q64_sector_erase(u32 addr)
{
	u32 cnt = 0;
	u8 status = 0;
	if( addr > 0x7fffff ) return 0;//�������ĵ�ַ����w25q64������ַ���򷵻�
	w25q64_write_enable();//дʹ��
	status = w25q64_read_status() & (1<<1);//��ȡд����λWEL��״̬
	while( status == 0 )//�ȴ�WELλ��1
	{
		status = w25q64_read_status() & (1<<1);//��ȡд����λWEL��״̬		
		if(++cnt>=0xFFFF)  return 0;   //û�н���дʹ�ܣ���������ʧ��
	}
	
	Flash_CS_L;//����Ƭѡ
	SPI1_Send_Read(0x20);//���������߳�ָ��
	SPI1_Send_Read( (addr >> 16) & 0xff );//���͵�ַ��8λ
	SPI1_Send_Read( (addr >> 8) & 0xff );//���͵�ַ��8λ
	SPI1_Send_Read( (addr >> 0) & 0xff );//���͵�ַ��8λ
	Flash_CS_H;//����Ƭѡ	
	
	status = w25q64_read_status() & (1<<0);//��ȡд����λBUSYλ��״̬
	while( status == 1 )//�ȴ�BUYSλ��0,�ȴ������߳��ɹ�
	{
		status = w25q64_read_status() & (1<<0);//��ȡд����λBUSYλ��״̬		
	}	
	return 1;
}

/*****************************************************************************************
*�������ܣ������߳�
*�����β�:��
*��������ֵ����
*������ע:8MB=128�飨64KB��   1��=16������4KB��   1����=16ҳ��256B��
*
*****************************************************************************************/
u8 w25q64_block_erase(u32 addr)
{
	u32 cnt = 0;
	u8 status = 0;
	if( addr > 0x7fffff ) return 0;//�������ĵ�ַ����w25q64������ַ���򷵻�
	w25q64_write_enable();//дʹ��
	status = w25q64_read_status() & (1<<1);//��ȡд����λWEL��״̬
	while( status == 0 )//�ȴ�WELλ��1
	{
		status = w25q64_read_status() & (1<<1);//��ȡд����λWEL��״̬		
		if(++cnt>=0xFFFF)  return 0;   //û�н���дʹ�ܣ���������ʧ��
	}
	
	Flash_CS_L;//����Ƭѡ
	SPI1_Send_Read(0xD8);//���Ϳ����߳�ָ��
	SPI1_Send_Read( (addr >> 16) & 0xff );//���͵�ַ��8λ
	SPI1_Send_Read( (addr >> 8) & 0xff );//���͵�ַ��8λ
	SPI1_Send_Read( (addr >> 0) & 0xff );//���͵�ַ��8λ
	Flash_CS_H;//����Ƭѡ	
	
	status = w25q64_read_status() & (1<<0);//��ȡд����λBUSYλ��״̬
	while( status == 1 )//�ȴ�BUYSλ��0,�ȴ������߳��ɹ�
	{
		status = w25q64_read_status() & (1<<0);//��ȡд����λBUSYλ��״̬		
	}	
	return 1;
}

/*****************************************************************************************
*�������ܣ�оƬ�߳�
*�����β�:��
*��������ֵ����
*������ע:8MB=128�飨64KB��   1��=16������4KB��   1����=16ҳ��256B��
*
*****************************************************************************************/
u8 w25q64_chip_erase(void)
{
	u32 cnt = 0;
	u8 status = 0;
	w25q64_write_enable();//дʹ��
	status = w25q64_read_status() & (1<<1);//��ȡд����λWEL��״̬
	while( status == 0 )//�ȴ�WELλ��1
	{
		status = w25q64_read_status() & (1<<1);//��ȡд����λWEL��״̬		
		if(++cnt>=0xFFFF)  return 0;   //û�н���дʹ�ܣ�оƬ����ʧ��
	}
	
	Flash_CS_L;//����Ƭѡ
	SPI1_Send_Read(0xD8);//����оƬ�߳�ָ��
	Flash_CS_H;//����Ƭѡ	
	
	status = w25q64_read_status() & (1<<0);//��ȡд����λBUSYλ��״̬
	while( status == 1 )//�ȴ�BUYSλ��0,�ȴ�оƬ�߳��ɹ�
	{
		status = w25q64_read_status() & (1<<0);//��ȡд����λBUSYλ��״̬		
	}	
	return 1;
}
/*****************************************************************************************
*�������ܣ�ҳ���
*�����β�:��
*��������ֵ����
*������ע:8MB=128�飨64KB��   1��=16������4KB��   1����=16ҳ��256B��
addr-��Ҫҳд�ĵ�ַ��*pdata-��Ҫд������ݣ�num-д�����ݵĸ���
*
*****************************************************************************************/
u8 w25q64_page_program(u32 addr,u8 *pdata,int num)
{
	int i = 0;
	u32 cnt = 0;
	u8 status = 0;
	int last_num = 0;
	last_num = 256-(addr & 0xff);//��ҳ��д�������
	if( last_num < num ) return 0;// ��ҳ����д����������
	if( addr > 0x7fffff ) return 0;//�������ĵ�ַ����w25q64������ַ���򷵻�
	w25q64_write_enable();//дʹ��
	status = w25q64_read_status() & (1<<1);//��ȡд����λWEL��״̬
	while( status == 0 )//�ȴ�WELλ��1
	{
		status = w25q64_read_status() & (1<<1);//��ȡд����λWEL��״̬		
//		lite_debug("status = %d",status);
		if(++cnt>=0xFFFF)  return 0;   //û�н���дʹ�ܣ�ҳ��̲���ʧ��
	}
	
	Flash_CS_L;//����Ƭѡ
	SPI1_Send_Read(0x02);//����ҳ���ָ��
	SPI1_Send_Read( (addr >> 16) & 0xff );//���͵�ַ��8λ
	SPI1_Send_Read( (addr >> 8) & 0xff );//���͵�ַ��8λ
	SPI1_Send_Read( (addr >> 0) & 0xff );//���͵�ַ��8λ
//lite_debug("num = %d",num);
	for( i =0 ; i < num ; i++ ) 
	{
		SPI1_Send_Read( pdata[i]);//������Ҫ�洢������
//		lite_debug("pdata[%d] = %d",i,pdata[i]);
	}
	
//		lite_debug("status = %d",status);
	Flash_CS_H;//����Ƭѡ	
	
	cnt = 0;
	status = w25q64_read_status() & (1<<0);//��ȡд����λBUSYλ��״̬
//	lite_debug("status = %d",status);
	while( status == 1 )//�ȴ�BUYSλ��0,�ȴ�ҳ��̳ɹ�
	{
		status = w25q64_read_status() & (1<<0);//��ȡд����λBUSYλ��״̬	
		if(++cnt>=0xFFFFFFFF)  return 0;   //û�н���дʹ�ܣ�ҳ���ʧ��		
	}	
	return 1;
}
/*****************************************************************************************
*�������ܣ�����д
*�����β�:��
*��������ֵ����
*������ע:8MB=128�飨64KB��   1��=16������4KB��   1����=16ҳ��256B��
addr-��Ҫд��ĵ�ַ��*pdata-��Ҫд������ݣ�num-д�����ݵĸ���
*
*****************************************************************************************/
u8 w25q64_write(u32 addr,u8 *pdata,int num)
{
	u8 err = 0;
	int last_num = 0;
	if( addr > 0x7fffff ) return 0;//��ַ����w25q64�ĵ�ַ
	last_num = 0x800000 - addr;//w25q64ʣ�µĿ���ռ�
	if( last_num < num ) return 0;//w25q64����д����������
	last_num = 256 - (addr &0xff);//��ҳ��д�������
	if(last_num >= num) last_num =num;//��ҳ����д����������
	lite_debug("write buf  num =%d",num);
	for(int i = 0;i<num;i++)
	printf("%02x ",pdata[i]);
	while( num )
	{
		err = w25q64_page_program(addr,pdata,last_num);
		//lite_debug("addr = 0x%x ",addr);
		if( err ==0 ) return 0;//д��ʧ��
		addr += last_num;
		pdata +=last_num;
		num -=last_num;
		
		if( num > 256) last_num = 256; 
		else last_num = num;
		
		
	}
	return 1;
}
/*****************************************************************************************
*�������ܣ������
*�����β�:��
*��������ֵ����
*������ע:8MB=128�飨64KB��   1��=16������4KB��   1����=16ҳ��256B��
addr-��Ҫд��ĵ�ַ��*pdata-��Ҫд������ݣ�num-д�����ݵĸ���
*
*****************************************************************************************/
u8 w25q64_read(u32 addr,u8 *pdata,int num)
{
	u32 i=0;
	if( addr>0x7FFFFF )    return 0;
	Flash_CS_L;//����Ƭѡ
	SPI1_Send_Read(0x03);//���Ͷ�����ָ��
	SPI1_Send_Read( (addr >> 16) & 0xff );//���͵�ַ��8λ
	SPI1_Send_Read( (addr >> 8) & 0xff );//���͵�ַ��8λ
	SPI1_Send_Read( (addr >> 0) & 0xff );//���͵�ַ��8λ
	
	for(i=0;i<num;i++) pdata[i]=SPI1_Send_Read(0x00);//��ȡ����
	
	Flash_CS_H;//����Ƭѡ		
	
	return 1;
}




