#include "stm32f4xx_conf.h"






/************************************************
 ALIENTEK 探索者STM32F407开发板 FreeRTOS实验2-1
 FreeRTOS移植实验-库函数版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/


//任务优先级
#define START_TASK_PRIO			1
//任务堆栈大小	
#define START_STK_SIZE 			256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LOW_TASK_PRIO			2
//任务堆栈大小	
#define LOW_STK_SIZE 			256  
//任务句柄
TaskHandle_t LowTask_Handler;
//任务函数
void low_task(void *pvParameters);

//任务优先级
#define MIDDLE_TASK_PRIO 		3
//任务堆栈大小	
#define MIDDLE_STK_SIZE  		256 
//任务句柄
TaskHandle_t MiddleTask_Handler;
//任务函数
void middle_task(void *pvParameters);

//任务优先级
#define HIGH_TASK_PRIO 			4
//任务堆栈大小	
#define HIGH_STK_SIZE  			256 
//任务句柄
TaskHandle_t HighTask_Handler;
//任务函数
void high_task(void *pvParameters);

//互斥信号量句柄
SemaphoreHandle_t MutexSemaphore;	//互斥信号量
void mutex_test(void);

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	debug_init();				//debug初始化
	LED_Init();		        //初始化LED端口
	//Spi1_init();//w25q64
	W25QXX_Init();
	//show version
	printf("\r\nkernel enter, build date:%s, build time:%s, build version=%s\r\n", __DATE__, __TIME__, KERNEL_VERSION);
		
	//create main task
	BaseType_t xReturn = pdPASS;
	xReturn = xTaskCreate((TaskFunction_t )AppTask,
						(const char*    )"app_task",
						(uint16_t       )2000,	//stack:4KB
						(void*          )NULL,
						(UBaseType_t    )DAEMON_TASK_PRIORITY, 
						NULL);
						
	lite_debug("create apptask ret=%d\r\n", xReturn) ;
	if(pdPASS == xReturn)
		vTaskStartScheduler();
	else
		return -1;  

	while(1)
	{
		lite_debug("unexpected while(1)");
	}
}
 
