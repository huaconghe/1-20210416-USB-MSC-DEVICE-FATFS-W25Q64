#ifndef	W25Q64_H
#define W25Q64_H

#include "stm32f4xx.h"
#define W25Q64_ID 0xef16

u16 w25q64_read_ID(void);
u8 w25q64_read_status(void);
void w25q64_write_enable(void);
u8 w25q64_sector_erase(u32 addr);
u8 w25q64_block_erase(u32 addr);
u8 w25q64_chip_erase(void);
u8 w25q64_page_program(u32 addr,u8 *pdata,int num);
u8 w25q64_write(u32 addr,u8 *pdata,int num);
u8 w25q64_read(u32 addr,u8 *pdata,int num);
#endif
