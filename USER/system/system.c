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
extern vu8 USB_STATUS_REG;		//USB״̬
extern vu8 bDeviceState;		//USB���� ���
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
		if(USB_STA!=USB_STATUS_REG)//״̬�ı��� 
		{	 						   
			if(USB_STATUS_REG&0x01)//����д		  
			{
				LED2_OFF;
				printf("USB Writing...\r\n");//��ʾUSB����д������	 
			}
			if(USB_STATUS_REG&0x02)//���ڶ�
			{
				LED2_OFF;
				printf("USB Reading...");//��ʾUSB���ڶ�������  		 
			}	 										  
			if(USB_STATUS_REG&0x04)printf("USB Write Err \r\n");//��ʾд�����
			 
			if(USB_STATUS_REG&0x08)printf("USB Read  Err \r\n");//��ʾ��������
			 
			USB_STA=USB_STATUS_REG;//��¼����״̬
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)printf("USB Connected    \r\n");//��ʾUSB�����Ѿ�����
			else printf("USB DisConnected \r\n");//��ʾUSB���γ���
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED2_ON;
			LED1_XOR;//��ʾϵͳ������
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB������,�����offline������
				bDeviceState=1;
			}else//û�еõ���ѯ 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=0;//2s��û�յ����߱��,����USB���γ���
			}
			USB_STATUS_REG=0;
		}
	}  
}

	FATFS fsobject;
	BYTE work[FF_MAX_SS]; 

	FIL fp ;
	const char write_buf[] = "FATFS����";
	char  read_buf[4096] = {0};
	UINT bw;
	UINT br;
//fatfs test
void fatfs_test(void)
{
	FRESULT  res ;
 
		
		res = f_mount(&fsobject,  "1:",  1);   //�����ļ�ϵͳ
		printf("f_mount res = %d\r\n",res);
		printf("����һ��FATFS�ļ�ϵͳ��ֲʵ��\r\n");
		if(res == FR_NO_FILESYSTEM)
		{
			res = f_mkfs("1:",0,work,sizeof(work));
			printf("f_mkfs  is  over\r\n");
			printf("res = %d\r\n",res);
			res = f_mount(NULL,  "1:",  1);   //ȡ���ļ�ϵͳ
				res = f_mount(&fsobject,  "1:",  1);   //�����ļ�ϵͳ
			printf("res = %d\r\n",res);
		}
			
		printf("hello word......\r\n");
		
		res = f_open(&fp , "1:fatfs_test_file.txt" , FA_OPEN_ALWAYS|FA_READ |FA_WRITE );//????&fp/fp
		//printf("f_open res = %d fp = %d\r\n",res,fp);
		if(res == RES_OK)
		{
			f_lseek(&fp,0);
			 res = f_write(&fp,"sfad�ɻ�����",sizeof(write_buf),&bw); //bwд��ɹ���
			 printf("bw=%d\r\n",bw);
			 if(res == RES_OK)
			 {
					f_lseek(&fp,0);
					res = f_read(&fp ,read_buf,f_size(&fp),&br);
					if(res == RES_OK)
					{
						 printf("�ļ�����Ϊ��%s ,br=%d\r\n",read_buf,br);
					}
			 }
		}
		res = f_close(&fp);
		printf("f_close res = %d\r\n",res);
		
		res = f_open(&fp , "1:�ļ�ϵͳ�����ļ�.c" , FA_OPEN_ALWAYS|FA_READ |FA_WRITE );//????&fp/fp
		//printf("f_open res = %d fp = %d\r\n",res,fp);
		if(res == RES_OK)
		{
			f_lseek(&fp,0);
			 res = f_write(&fp,"�ļ�ϵͳ�����ļ���������Ϊ��abcdefghijk",sizeof(write_buf),&bw); //bwд��ɹ���
			 printf("bw=%d\r\n",bw);
			 if(res == RES_OK)
			 {
					f_lseek(&fp,0);
					res = f_read(&fp ,read_buf,f_size(&fp),&br);
					if(res == RES_OK)
					{
						 printf("�ļ�����Ϊ��%s ,br=%d\r\n",read_buf,br);
					}
			 }
		}
		res = f_close(&fp);
		printf("f_close res = %d\r\n",res);
//		while(1)
//		{
//		}

}

