

#ifndef DEBUG_H
#define DEBUG_H

#include "system_config.h"

extern void debug_init(void);
extern int sys_log(char *log);


#if LIB_DEBUG_ENABLE==1
	#include "freertos.h"
	#include "semphr.h"
	#include "stdio.h"

	extern SemaphoreHandle_t debugMutex;
	extern void lite_printf_buff(const unsigned char *title,unsigned char *data_in, unsigned int len_in);

	//can not called in isr
	#define lite_debug(...)			 do{	xSemaphoreTake( debugMutex, portMAX_DELAY );\
										printf("\r\n%s %s %d:\t", strrchr(__FILE__,'\\')+1,__FUNCTION__,__LINE__);printf(__VA_ARGS__);	\
										xSemaphoreGive( debugMutex ); \
									}while(0)
	//can be called everywhere
	#define log_printf(...)		do{printf(__VA_ARGS__);}while(0)

//	#define lite_debug		printf
	#define lite_debug_buff		lite_printf_buff

#else

	#include "freertos.h"
	#include "semphr.h"
	#include "stdio.h"
	extern SemaphoreHandle_t debugMutex;

	#define lite_debug(fmt, ...)
	#define log_printf(fmt, ...)

//	#define lite_debug(...)
	#define lite_debug_buff(...)
#endif

#endif

