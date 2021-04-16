

#ifndef SYSTEM_H
#define SYSTEM_H

#include "freertos.h"
#include "task.h"

extern void system_init(void);
extern void sys_cpu_clk_config(void);
extern int SER_PutChar (int ch);
//extern void delay_ms(int ms);
extern void mdelay(int ms);



//must < configMAX_PRIORITIES
#define DAEMON_TASK_PRIORITY 	2	//daemon task priority
#define NORMAL_TASK_PRIORITY 	4	//normal task priority
#define URGENT_TASK_PRIORITY	8	//urgent task priority
#define CRITICAL_TASK_PRIORITY  (configMAX_PRIORITIES-1)	//critical task priority


extern void AppTask(void* parameter);
extern void sys_get_tick(TimeOut_t * const pxTimeOut);
extern int sys_is_timeout(TimeOut_t * const pxTimeOut, TickType_t *pxTicksToWait);
void fatfs_test(void);
void usb_msc_test(void);


typedef enum
{
	LED_GREEN=1,
	LED_YELLOW=2,
	LED_BLUE=3,
	LED_RED=4,
}
led_num_enum_t;

#define MODEL_PORT GPIOC
#define MODEL_PIN  GPIO_Pin_2

#define PWR_EN_PORT GPIOC
#define PWR_EN_PIN	GPIO_Pin_6
#define PWR_KEY_PORT GPIOC
#define PWR_KEY_PIN	 GPIO_Pin_7	
#define PWR_3V3_EN_PORT GPIOH
#define PWR_3V3_EN_PIN	GPIO_Pin_10
#define PWR_ADC_PORT 	GPIOC
#define PWR_ADC_PIN		GPIO_Pin_0

#define PWR_CHARGE_PORT GPIOC
#define PWR_CHARGE_PIN  GPIO_Pin_4

#define sys_power_enable()	GPIO_SetBits(PWR_EN_PORT, PWR_EN_PIN)
#define sys_power_disable()	GPIO_ResetBits(PWR_EN_PORT, PWR_EN_PIN)

#define sys_power_3v3_enable()	GPIO_SetBits(PWR_3V3_EN_PORT, PWR_3V3_EN_PIN)
#define sys_power_3v3_disable()	GPIO_ResetBits(PWR_3V3_EN_PORT, PWR_3V3_EN_PIN)

#define sys_power_is_charging()		(0 == GPIO_ReadInputDataBit(PWR_CHARGE_PORT, PWR_CHARGE_PIN))


#define BAT_AD_LEVEL0 3280	//3.28v
#define BAT_AD_LEVEL1 3400	//3.4v
#define BAT_AD_LEVEL2 3800	//3.8v
#define BAT_AD_LEVEL3 4000	//4.0v



extern int sys_set_debug_port(USART_TypeDef *uartx);
extern char *sys_get_model(void);
void sys_get_tick(TimeOut_t * const pxTimeOut);
int sys_is_timeout(TimeOut_t * const pxTimeOut, TickType_t *pxTicksToWait);

typedef __packed struct
{
	char *image_name;
	char *image_version;
	char *image_date;
	char *image_time;
	uint32_t image_entry;
	uint32_t image_crc;
}
image_version_t;


#define ENABLE_CPU_SLEEP 1



#endif




