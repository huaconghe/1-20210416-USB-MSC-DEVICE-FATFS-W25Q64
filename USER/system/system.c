#include "stm32f4xx_conf.h"
#include "stdio.h"
#include "system.h"
#include "system_config.h"
#include "ff.h"
/*usb*/  
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 
#include "usbd_msc_mem.h"
/*
	get rtos tick
	input: none
	output: pxTimeOut
	return: none
*/
void sys_get_tick(TimeOut_t * const pxTimeOut)
{
	vTaskSetTimeOutState(pxTimeOut);
}

/*
	check if timeout from tick pxTimeOut
	input: 	pxTimeOut-tick before
			ms-interval to check
	output: none
	return: 0-not timeout, 1-timeout
*/
int sys_is_timeout(TimeOut_t * const pxTimeOut, TickType_t *pxTicksToWait)
{
	//TickType_t pxTicksToWait=pdMS_TO_TICKS(ms);
	
	return xTaskCheckForTimeOut(pxTimeOut, pxTicksToWait);
}

void AppTask(void* parameter)
{
//W25QXX_Erase_Chip();
	//wifi_init();
	while(1)
	{
		//LED1_XOR;
		printf("w25q64_read_ID = %x\r\n",W25QXX_ReadID());
		//wifi_test();
		//sdk_wifi_test();
		fatfs_test();
		usb_msc_test();
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}
//USB_OTG_CORE_HANDLE USB_OTG_dev;

USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况
void usb_msc_test(void)
{
	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;
USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
	delay_ms(1800);	
	while(1)
	{	
		delay_ms(1);				  
		if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   
			if(USB_STATUS_REG&0x01)//正在写		  
			{
				LED2_OFF;
				printf("USB Writing...\r\n");//提示USB正在写入数据	 
			}
			if(USB_STATUS_REG&0x02)//正在读
			{
				LED2_OFF;
				printf("USB Reading...");//提示USB正在读出数据  		 
			}	 										  
			if(USB_STATUS_REG&0x04)printf("USB Write Err \r\n");//提示写入错误
			 
			if(USB_STATUS_REG&0x08)printf("USB Read  Err \r\n");//提示读出错误
			 
			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)printf("USB Connected    \r\n");//提示USB连接已经建立
			else printf("USB DisConnected \r\n");//提示USB被拔出了
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED2_ON;
			LED1_XOR;//提示系统在运行
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=1;
			}else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG=0;
		}
	}  
}

	FATFS fsobject;
	BYTE work[FF_MAX_SS]; 

	FIL fp ;
	const char write_buf[] = "FATFS测试";
	char  read_buf[4096] = {0};
	UINT bw;
	UINT br;
//fatfs test
void fatfs_test(void)
{
	FRESULT  res ;
 
		
		res = f_mount(&fsobject,  "1:",  1);   //挂载文件系统
		printf("f_mount res = %d\r\n",res);
		printf("这是一个FATFS文件系统移植实验\r\n");
		if(res == FR_NO_FILESYSTEM)
		{
			res = f_mkfs("1:",0,work,sizeof(work));
			printf("f_mkfs  is  over\r\n");
			printf("res = %d\r\n",res);
			res = f_mount(NULL,  "1:",  1);   //取消文件系统
				res = f_mount(&fsobject,  "1:",  1);   //挂载文件系统
			printf("res = %d\r\n",res);
		}
			
		printf("hello word......\r\n");
		
		res = f_open(&fp , "1:fatfs_test_file.txt" , FA_OPEN_ALWAYS|FA_READ |FA_WRITE );//????&fp/fp
		//printf("f_open res = %d fp = %d\r\n",res,fp);
		if(res == RES_OK)
		{
			f_lseek(&fp,0);
			 res = f_write(&fp,"sfad飞机开发",sizeof(write_buf),&bw); //bw写入成功的
			 printf("bw=%d\r\n",bw);
			 if(res == RES_OK)
			 {
					f_lseek(&fp,0);
					res = f_read(&fp ,read_buf,f_size(&fp),&br);
					if(res == RES_OK)
					{
						 printf("文件内容为：%s ,br=%d\r\n",read_buf,br);
					}
			 }
		}
		res = f_close(&fp);
		printf("f_close res = %d\r\n",res);
		
		res = f_open(&fp , "1:文件系统测试文件.c" , FA_OPEN_ALWAYS|FA_READ |FA_WRITE );//????&fp/fp
		//printf("f_open res = %d fp = %d\r\n",res,fp);
		if(res == RES_OK)
		{
			f_lseek(&fp,0);
			 res = f_write(&fp,"文件系统测试文件测试内容为：abcdefghijk",sizeof(write_buf),&bw); //bw写入成功的
			 printf("bw=%d\r\n",bw);
			 if(res == RES_OK)
			 {
					f_lseek(&fp,0);
					res = f_read(&fp ,read_buf,f_size(&fp),&br);
					if(res == RES_OK)
					{
						 printf("文件内容为：%s ,br=%d\r\n",read_buf,br);
					}
			 }
		}
		res = f_close(&fp);
		printf("f_close res = %d\r\n",res);
//		while(1)
//		{
//		}

}

