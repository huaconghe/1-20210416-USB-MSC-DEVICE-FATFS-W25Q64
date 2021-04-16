

#include "stm32f4xx_conf.h"
#include "semphr.h"


/*----------------------------------------*/
#define LOG_SEND_CHAR	SER_PutChar

#define LOG_FALSE	0
#define LOG_TRUE	1
#define LOG_NULL	((void *)0)

/*------------------------------------*/

SemaphoreHandle_t debugMutex=NULL;


/*
	create a mutex for printf
*/
void debug_init(void)
{
	//USART_TypeDef *debug_uart=USART1;

	uart_open(1, 115200);	
	//sys_set_debug_port(debug_uart);
	
	debugMutex=xSemaphoreCreateMutex();
}

