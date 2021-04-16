#ifndef SPI_H
#define SPI_H

#include "stm32f4xx.h"
#define Flash_CS_H ( GPIOB->BSRRL |= 1 << 14 )//置位拉高
#define Flash_CS_L ( GPIOB->BSRRH |= 1 << 14 )//复位拉低

void Spi1_init(void);
u8 SPI1_Send_Read(u8 data);

void SPI1_Init(void);
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI1_ReadWriteByte(u8 TxData);
#endif
