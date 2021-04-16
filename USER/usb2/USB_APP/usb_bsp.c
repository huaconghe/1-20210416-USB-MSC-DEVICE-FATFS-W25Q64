/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    17-March-2018
  * @brief   This file is responsible to offer board support package and is
  *          configurable by user.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      <http://www.st.com/SLA0044>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include "usb_bsp.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USB_BSP
* @brief This file is responsible to offer board support package
* @{
*/

/** @defgroup USB_BSP_Private_Defines
* @{
*/
/**
* @}
*/


/** @defgroup USB_BSP_Private_TypesDefinitions
* @{
*/
/**
* @}
*/





/** @defgroup USB_BSP_Private_Macros
* @{
*/
/**
* @}
*/

/** @defgroup USBH_BSP_Private_Variables
* @{
*/

/**
* @}
*/

/** @defgroup USBH_BSP_Private_FunctionPrototypes
* @{
*/
/**
* @}
*/

/** @defgroup USB_BSP_Private_Functions
* @{
*/


/**
* @brief  USB_OTG_BSP_Init
*         Initializes BSP configurations
* @param  None
* @retval None
*/
//USB������Դ���ƿ�
#define USB_HOST_PWRCTRL 	PAout(15)	//PA15
void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE * pdev)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);//ʹ��USB OTGʱ��	��
  //GPIOA11,A12����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;//PA11/12���ù������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PA15�������		
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//�������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
  USB_HOST_PWRCTRL=1;			//����USB HOST��Դ����

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG_FS);//PA11,AF10(USB)
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG_FS);//PA12,AF10(USB)
}

/**
* @brief  USB_OTG_BSP_EnableInterrupt
*         Enable USB Global interrupt
* @param  None
* @retval None
*/
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE * pdev)
{
 	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//�����ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ��ͨ��
  NVIC_Init(&NVIC_InitStructure);//����  
}
//USB OTG �ж�����,����USB FS�ж�
//pdev:USB OTG�ں˽ṹ��ָ��
void USB_OTG_BSP_DisableInterrupt(void)
{ 
}
//USB OTG �˿ڹ�������(������δ�õ�)
//pdev:USB OTG�ں˽ṹ��ָ��
//state:0,�ϵ�;1,�ϵ�
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{ 
}
//USB_OTG �˿ڹ���IO����(������δ�õ�)
//pdev:USB OTG�ں˽ṹ��ָ��
void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{ 
} 
//USB_OTG us����ʱ����
//�����̲���SYSTEM�ļ��е�delay.c�����delay_us����ʵ��
//�ٷ����̲��õ��Ƕ�ʱ��2��ʵ�ֵ�.
//usec:Ҫ��ʱ��us��.
void USB_OTG_BSP_uDelay (const uint32_t usec)
{ 
   	delay_us(usec);
}
//USB_OTG ms����ʱ����
//�����̲���SYSTEM�ļ��е�delay.c�����delay_ms����ʵ��
//�ٷ����̲��õ��Ƕ�ʱ��2��ʵ�ֵ�.
//msec:Ҫ��ʱ��ms��.
void USB_OTG_BSP_mDelay (const uint32_t msec)
{  
	delay_ms(msec);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
