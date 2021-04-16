/**
  ******************************************************************************
  * @file    usbd_storage_msd.c
  * @author  MCD application Team
  * @version V1.2.1
  * @date    17-March-2018
  * @brief   This file provides the disk operations functions.
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
#include "usbd_msc_mem.h"
#include "w25q64.h"
#include "usb_dcd_int.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup STORAGE
  * @brief media storage application module
  * @{
  */

/** @defgroup STORAGE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup STORAGE_Private_Defines
  * @{
  */

#define STORAGE_LUN_NBR                  1
////////////////////////////自己定义的一个标记USB状态的寄存器///////////////////
//bit0:表示电脑正在向SD卡写入数据
//bit1:表示电脑正从SD卡读出数据
//bit2:SD卡写数据错误标志位
//bit3:SD卡读数据错误标志位
//bit4:1,表示电脑有轮询操作(表明连接还保持着)
 vu8 USB_STATUS_REG=0;

////////////////////////////////////////////////////////////////////////////////
/**
  * @}
  */


/** @defgroup STORAGE_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup STORAGE_Private_Variables
  * @{
  */
/* USB Mass storage Standard Inquiry Data */
const int8_t STORAGE_Inquirydata[] = {  // 36

//  /* LUN 0 */
//  0x00,
//  0x80,
//  0x02,
//  0x02,
//  (USBD_STD_INQUIRY_LENGTH - 4),
//  0x00,
//  0x00,
//  0x00,
//  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
//  'm', 'i', 'c', 'r', 'o', 'S', 'D', ' ', /* Product : 16 Bytes */
//  'F', 'l', 'a', 's', 'h', ' ', ' ', ' ',
//  '1', '.', '0', '0',           /* Version : 4 Bytes */
//  /* LUN 1 */
  0x00,
  0x80,
  0x02,
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,
  0x00,
    /* Vendor Identification */
    'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',//9字节
    /* Product Identification */
    'S', 'P', 'I', ' ', 'F', 'l', 'a', 's', 'h',//15字节
    ' ','D', 'i', 's', 'k', ' ',
    /* Product Revision Level */	
    '1', '.', '0', ' ',							//4字节		
};

/**
  * @}
  */


/** @defgroup STORAGE_Private_FunctionPrototypes
  * @{
  */
int8_t STORAGE_Init(uint8_t lun);

int8_t STORAGE_GetCapacity(uint8_t lun,
                           uint32_t * block_num, uint32_t * block_size);

int8_t STORAGE_IsReady(uint8_t lun);

int8_t STORAGE_IsWriteProtected(uint8_t lun);

int8_t STORAGE_Read(uint8_t lun,
                    uint8_t * buf, uint32_t blk_addr, uint16_t blk_len);

int8_t STORAGE_Write(uint8_t lun,
                     uint8_t * buf, uint32_t blk_addr, uint16_t blk_len);

int8_t STORAGE_GetMaxLun(void);


USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops = {
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *) STORAGE_Inquirydata,
};

USBD_STORAGE_cb_TypeDef *USBD_STORAGE_fops = &USBD_MICRO_SDIO_fops;
#ifndef USE_STM3210C_EVAL
//extern SD_CardInfo SDCardInfo;
#endif
__IO uint32_t count = 0;
/**
  * @}
  */


/** @defgroup STORAGE_Private_Functions
  * @{
  */


/**
  * @brief  Initialize the storage medium
  * @param  lun : logical unit number
  * @retval Status
  */

int8_t STORAGE_Init(uint8_t lun)
{
	Spi1_init();//w25q64 init
  return 0;

}

/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t * block_num,
                           uint32_t * block_size)
{
//	if(lun ==0)//sd
//	{

//	}
//	else//spi flash
	{
		*block_num = 4096;
		*block_size = 1024*1024*6;
	}
  return (0);
}

/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t STORAGE_IsReady(uint8_t lun)
{
	USB_STATUS_REG |= 0X10;//电脑有轮询操作
  return (0);
}

/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
  return 0;
}

/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Read(uint8_t lun,
                    uint8_t * buf, uint32_t blk_addr, uint16_t blk_len)
{
	USB_STATUS_REG |= 0X02;//标记正在读数据
//	if(lun ==0)//sd
//	{

//	}
//	else//spi flash
	{
		w25q64_read(blk_addr*4096,buf,blk_len*4096);
		printf("%s\r\n",__func__);
	}
  return 0;
}

/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Write(uint8_t lun,
                     uint8_t * buf, uint32_t blk_addr, uint16_t blk_len)
{
	int result;
	USB_STATUS_REG |= 0X01;//标记正在写入数据
//	if(lun ==0)//sd
//	{

//	}
//	else//spi flash
	{
//		for(int i=0; i< blk_len;i++)
//		{
//			result = w25q64_sector_erase(blk_addr+i*4096);
//			lite_debug("result = %d sector = %d blk_len = %d",result,i,blk_len);
//		}
		for(int i=blk_addr; i<blk_addr+blk_len ;i++)
		{
			result = w25q64_sector_erase(i*4096);
			lite_debug("result = %d sector = %d blk_len = %d",result,i,blk_len);
		}
		w25q64_write(blk_addr*4096,buf,blk_len*4096);
	}
  return (0);
}

/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */

int8_t STORAGE_GetMaxLun(void)
{
  return (STORAGE_LUN_NBR - 1);
}

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
