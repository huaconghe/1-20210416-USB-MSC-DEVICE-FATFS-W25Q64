/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "stm32f4xx_conf.h"
#define SD_CARD	 0  //SD卡,卷标为0
#define EX_FLASH 1	//外部flash,卷标为1

#define FLASH_SECTOR_SIZE 	512		
u16	    FLASH_SECTOR_COUNT=2048*6;
#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区
/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
//	DSTATUS stat;
	int result;

	switch (pdrv) {
	case SD_CARD :
		//result = RAM_disk_initialize();

		// translate the reslut code here

		return RES_OK;

	case EX_FLASH :
			W25QXX_Init();
			FLASH_SECTOR_COUNT=2048*6;//W25Q64,前6M字节给FATFS占用 
		lite_debug("result = %d\r\n",result);
		return RES_OK;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return RES_OK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
//	DRESULT res;
//	int result;

	switch (pdrv) 
	{
		case SD_CARD :
			// translate the arguments here

			//result = RAM_disk_read(buff, sector, count);

			// translate the reslut code here

			return RES_OK;

		case EX_FLASH :
			// translate the arguments here
			//w25q64_read( sector*4096 ,buff , count*4096);
				for(;count>0;count--)
				{
					W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
					sector++;
					buff+=FLASH_SECTOR_SIZE;
				}
			return RES_OK;

		case DEV_USB :
			// translate the arguments here

			//result = USB_disk_read(buff, sector, count);

			// translate the reslut code here

			return RES_OK;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
//	DRESULT res;
	//int result;

	switch (pdrv) {
	case SD_CARD :

		return RES_OK;

	case EX_FLASH :
			for(;count>0;count--)
			{										    
				W25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
		return RES_OK;;

	case DEV_USB :
		return RES_OK;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	u8 res;

	switch (pdrv) 
	{
		case SD_CARD :
			{
				return RES_OK;
			}
		case EX_FLASH :
			{
				switch(cmd)
				{
					case CTRL_SYNC:
					res = RES_OK; 
							break;	 
					case GET_SECTOR_SIZE:
							*(WORD*)buff = FLASH_SECTOR_SIZE;
							res = RES_OK;
							break;	 
					case GET_BLOCK_SIZE:
							*(WORD*)buff = FLASH_BLOCK_SIZE;
							res = RES_OK;
							break;	 
					case GET_SECTOR_COUNT:
							*(DWORD*)buff = FLASH_SECTOR_COUNT;
							res = RES_OK;
							break;
					default:
							res = RES_PARERR;
							break;				
				}		
				if(res == 0x00)return RES_OK;	 
				else return RES_ERROR;	
			}
			default:return RES_ERROR;
	}
}

//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}		

