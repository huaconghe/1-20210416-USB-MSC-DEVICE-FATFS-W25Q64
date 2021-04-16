#include "w25q64.h"
#include "spi.h"
#include <stdio.h>
#include <string.h>

/*****************************************************************************************
*函数功能：读取w25q64器件ID
*函数形参:无
*函数返回值：器件ID
*函数备注:
*
*****************************************************************************************/
u16 w25q64_read_ID(void)
{
	u16 device_id=0;
	Flash_CS_L;//拉低片选
	SPI1_Send_Read(0x90);//发送读id指令
	SPI1_Send_Read(0x00);
	SPI1_Send_Read(0x00);
	SPI1_Send_Read(0x00);//发送24位地址
	device_id |= SPI1_Send_Read(0xFF);//读取高8位
	device_id <<=8;
	device_id |= SPI1_Send_Read(0xFF);//读取低8位
	Flash_CS_H;//拉高片选
	return device_id;
}


/*****************************************************************************************
*函数功能：读取w25q64状态寄存器的值
*函数形参:无
*函数返回值：状态寄存器的值
*函数备注:
*
*****************************************************************************************/
u8 w25q64_read_status(void)
{
	u8 status = 0;
	Flash_CS_L;//拉低片选
	SPI1_Send_Read(0x05);//发送读状态指令
	status = SPI1_Send_Read(0x00);//发送任意数据读取状态寄存器的值
	Flash_CS_H;//拉高片选
	return status;
}

/*****************************************************************************************
*函数功能：写使能
*函数形参:无
*函数返回值：无
*函数备注:
*
*****************************************************************************************/
void w25q64_write_enable(void)
{
	Flash_CS_L;//拉低片选
	SPI1_Send_Read(0x06);//发送写使能指令
	Flash_CS_H;//拉高片选
}

/*****************************************************************************************
*函数功能：扇区檫除
*函数形参:无
*函数返回值：无
*函数备注:8MB=128块（64KB）   1块=16扇区（4KB）   1扇区=16页（256B）
*
*****************************************************************************************/
u8 w25q64_sector_erase(u32 addr)
{
	u32 cnt = 0;
	u8 status = 0;
	if( addr > 0x7fffff ) return 0;//如果输入的地址大于w25q64的最大地址，则返回
	w25q64_write_enable();//写使能
	status = w25q64_read_status() & (1<<1);//获取写保护位WEL的状态
	while( status == 0 )//等待WEL位置1
	{
		status = w25q64_read_status() & (1<<1);//获取写保护位WEL的状态		
		if(++cnt>=0xFFFF)  return 0;   //没有进入写使能，扇区擦除失败
	}
	
	Flash_CS_L;//拉低片选
	SPI1_Send_Read(0x20);//发送扇区檫除指令
	SPI1_Send_Read( (addr >> 16) & 0xff );//发送地址高8位
	SPI1_Send_Read( (addr >> 8) & 0xff );//发送地址中8位
	SPI1_Send_Read( (addr >> 0) & 0xff );//发送地址低8位
	Flash_CS_H;//拉高片选	
	
	status = w25q64_read_status() & (1<<0);//获取写保护位BUSY位的状态
	while( status == 1 )//等待BUYS位置0,等待扇区檫除成功
	{
		status = w25q64_read_status() & (1<<0);//获取写保护位BUSY位的状态		
	}	
	return 1;
}

/*****************************************************************************************
*函数功能：块区檫除
*函数形参:无
*函数返回值：无
*函数备注:8MB=128块（64KB）   1块=16扇区（4KB）   1扇区=16页（256B）
*
*****************************************************************************************/
u8 w25q64_block_erase(u32 addr)
{
	u32 cnt = 0;
	u8 status = 0;
	if( addr > 0x7fffff ) return 0;//如果输入的地址大于w25q64的最大地址，则返回
	w25q64_write_enable();//写使能
	status = w25q64_read_status() & (1<<1);//获取写保护位WEL的状态
	while( status == 0 )//等待WEL位置1
	{
		status = w25q64_read_status() & (1<<1);//获取写保护位WEL的状态		
		if(++cnt>=0xFFFF)  return 0;   //没有进入写使能，块区擦除失败
	}
	
	Flash_CS_L;//拉低片选
	SPI1_Send_Read(0xD8);//发送块区檫除指令
	SPI1_Send_Read( (addr >> 16) & 0xff );//发送地址高8位
	SPI1_Send_Read( (addr >> 8) & 0xff );//发送地址中8位
	SPI1_Send_Read( (addr >> 0) & 0xff );//发送地址低8位
	Flash_CS_H;//拉高片选	
	
	status = w25q64_read_status() & (1<<0);//获取写保护位BUSY位的状态
	while( status == 1 )//等待BUYS位置0,等待块区檫除成功
	{
		status = w25q64_read_status() & (1<<0);//获取写保护位BUSY位的状态		
	}	
	return 1;
}

/*****************************************************************************************
*函数功能：芯片檫除
*函数形参:无
*函数返回值：无
*函数备注:8MB=128块（64KB）   1块=16扇区（4KB）   1扇区=16页（256B）
*
*****************************************************************************************/
u8 w25q64_chip_erase(void)
{
	u32 cnt = 0;
	u8 status = 0;
	w25q64_write_enable();//写使能
	status = w25q64_read_status() & (1<<1);//获取写保护位WEL的状态
	while( status == 0 )//等待WEL位置1
	{
		status = w25q64_read_status() & (1<<1);//获取写保护位WEL的状态		
		if(++cnt>=0xFFFF)  return 0;   //没有进入写使能，芯片擦除失败
	}
	
	Flash_CS_L;//拉低片选
	SPI1_Send_Read(0xD8);//发送芯片檫除指令
	Flash_CS_H;//拉高片选	
	
	status = w25q64_read_status() & (1<<0);//获取写保护位BUSY位的状态
	while( status == 1 )//等待BUYS位置0,等待芯片檫除成功
	{
		status = w25q64_read_status() & (1<<0);//获取写保护位BUSY位的状态		
	}	
	return 1;
}
/*****************************************************************************************
*函数功能：页编程
*函数形参:无
*函数返回值：无
*函数备注:8MB=128块（64KB）   1块=16扇区（4KB）   1扇区=16页（256B）
addr-需要页写的地址；*pdata-需要写入的数据；num-写入数据的个数
*
*****************************************************************************************/
u8 w25q64_page_program(u32 addr,u8 *pdata,int num)
{
	int i = 0;
	u32 cnt = 0;
	u8 status = 0;
	int last_num = 0;
	last_num = 256-(addr & 0xff);//此页可写入的数据
	if( last_num < num ) return 0;// 此页不能写完所有数据
	if( addr > 0x7fffff ) return 0;//如果输入的地址大于w25q64的最大地址，则返回
	w25q64_write_enable();//写使能
	status = w25q64_read_status() & (1<<1);//获取写保护位WEL的状态
	while( status == 0 )//等待WEL位置1
	{
		status = w25q64_read_status() & (1<<1);//获取写保护位WEL的状态		
//		lite_debug("status = %d",status);
		if(++cnt>=0xFFFF)  return 0;   //没有进入写使能，页编程擦除失败
	}
	
	Flash_CS_L;//拉低片选
	SPI1_Send_Read(0x02);//发送页编程指令
	SPI1_Send_Read( (addr >> 16) & 0xff );//发送地址高8位
	SPI1_Send_Read( (addr >> 8) & 0xff );//发送地址中8位
	SPI1_Send_Read( (addr >> 0) & 0xff );//发送地址低8位
//lite_debug("num = %d",num);
	for( i =0 ; i < num ; i++ ) 
	{
		SPI1_Send_Read( pdata[i]);//传入需要存储的数据
//		lite_debug("pdata[%d] = %d",i,pdata[i]);
	}
	
//		lite_debug("status = %d",status);
	Flash_CS_H;//拉高片选	
	
	cnt = 0;
	status = w25q64_read_status() & (1<<0);//获取写保护位BUSY位的状态
//	lite_debug("status = %d",status);
	while( status == 1 )//等待BUYS位置0,等待页编程成功
	{
		status = w25q64_read_status() & (1<<0);//获取写保护位BUSY位的状态	
		if(++cnt>=0xFFFFFFFF)  return 0;   //没有进入写使能，页编程失败		
	}	
	return 1;
}
/*****************************************************************************************
*函数功能：任意写
*函数形参:无
*函数返回值：无
*函数备注:8MB=128块（64KB）   1块=16扇区（4KB）   1扇区=16页（256B）
addr-需要写入的地址；*pdata-需要写入的数据；num-写入数据的个数
*
*****************************************************************************************/
u8 w25q64_write(u32 addr,u8 *pdata,int num)
{
	u8 err = 0;
	int last_num = 0;
	if( addr > 0x7fffff ) return 0;//地址超出w25q64的地址
	last_num = 0x800000 - addr;//w25q64剩下的空余空间
	if( last_num < num ) return 0;//w25q64不能写完所有数据
	last_num = 256 - (addr &0xff);//此页可写入的数据
	if(last_num >= num) last_num =num;//此页可以写完所有数据
	lite_debug("write buf  num =%d",num);
	for(int i = 0;i<num;i++)
	printf("%02x ",pdata[i]);
	while( num )
	{
		err = w25q64_page_program(addr,pdata,last_num);
		//lite_debug("addr = 0x%x ",addr);
		if( err ==0 ) return 0;//写入失败
		addr += last_num;
		pdata +=last_num;
		num -=last_num;
		
		if( num > 256) last_num = 256; 
		else last_num = num;
		
		
	}
	return 1;
}
/*****************************************************************************************
*函数功能：任意读
*函数形参:无
*函数返回值：无
*函数备注:8MB=128块（64KB）   1块=16扇区（4KB）   1扇区=16页（256B）
addr-需要写入的地址；*pdata-需要写入的数据；num-写入数据的个数
*
*****************************************************************************************/
u8 w25q64_read(u32 addr,u8 *pdata,int num)
{
	u32 i=0;
	if( addr>0x7FFFFF )    return 0;
	Flash_CS_L;//拉低片选
	SPI1_Send_Read(0x03);//发送读数据指令
	SPI1_Send_Read( (addr >> 16) & 0xff );//发送地址高8位
	SPI1_Send_Read( (addr >> 8) & 0xff );//发送地址中8位
	SPI1_Send_Read( (addr >> 0) & 0xff );//发送地址低8位
	
	for(i=0;i<num;i++) pdata[i]=SPI1_Send_Read(0x00);//读取数据
	
	Flash_CS_H;//拉高片选		
	
	return 1;
}




