#ifndef UART_H
#define UART_H

#include "stm32f4xx.h"

#define EN_USART1_RX 1
#define EN_USART2_RX 1
#define EN_USART3_RX 1

int uart_data_write(uint8_t port, uint8_t *data, uint16_t len);
int uart_clearbuffer(int port);
int uart_open(int port, int baud);
int uart_close(int port);
int uart_sendbuffer(int port, uint8_t *data, uint16_t len);
int uart_rcvbuffer(int port, uint8_t *data, uint16_t len, uint16_t timeout);
int uart_at_command_transceive(int port, 
									const char *const at_cmd, 
									uint8_t *const rsp, 
									uint16_t rsp_size, 
									uint16_t timeout, 
									uint8_t retry,
									QueueHandle_t mutex,
									int cwt_ms);
int uart_at_command_transceive_ext(int port, 
									const uint8_t *const cmd,
									uint16_t len,
									uint8_t *const rsp, uint16_t rsp_size, 
									uint16_t timeout, 
									uint8_t retry,
									QueueHandle_t mutex,
									int cwt_ms);
int uart_at_command_recevier_ext(int port, 
											uint8_t *const rsp, uint16_t rsp_size, 
											uint16_t timeout, 
											QueueHandle_t mutex,
											int cwt_ms);



#endif

