#include "stm32f4xx_conf.h"






/************************************************
 ALIENTEK ̽����STM32F407������ FreeRTOSʵ��2-1
 FreeRTOS��ֲʵ��-�⺯���汾
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


//�������ȼ�
#define START_TASK_PRIO			1
//�����ջ��С	
#define START_STK_SIZE 			256  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LOW_TASK_PRIO			2
//�����ջ��С	
#define LOW_STK_SIZE 			256  
//������
TaskHandle_t LowTask_Handler;
//������
void low_task(void *pvParameters);

//�������ȼ�
#define MIDDLE_TASK_PRIO 		3
//�����ջ��С	
#define MIDDLE_STK_SIZE  		256 
//������
TaskHandle_t MiddleTask_Handler;
//������
void middle_task(void *pvParameters);

//�������ȼ�
#define HIGH_TASK_PRIO 			4
//�����ջ��С	
#define HIGH_STK_SIZE  			256 
//������
TaskHandle_t HighTask_Handler;
//������
void high_task(void *pvParameters);

//�����ź������
SemaphoreHandle_t MutexSemaphore;	//�����ź���
void mutex_test(void);

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	debug_init();				//debug��ʼ��
	LED_Init();		        //��ʼ��LED�˿�
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
 
