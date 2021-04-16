#include "stm32f4xx_conf.h"


//#define lite_debug(...)			 printf("\r\n%s %s %d:\t", strrchr(__FILE__,'\\')+1,__FUNCTION__,__LINE__);printf(__VA_ARGS__);	
//#define log_printf(...)		do{printf(__VA_ARGS__);}while(0)


#define UART_PORT_CNT	7
#define UART_BUFFER_SIZE 512




static uint8_t uart_buffer[UART_PORT_CNT][UART_BUFFER_SIZE]={0};
static uint16_t uart_read_index[UART_PORT_CNT]={0};
static uint16_t uart_write_index[UART_PORT_CNT]={0};

int uart_data_write(uint8_t port, uint8_t *data, uint16_t len)
{
	int i;

	__disable_irq();
	for(i=0;i<len;)
	{	
		uart_buffer[port][uart_write_index[port]++]=data[i];
		i++;
		
		if(uart_write_index[port]>=sizeof(uart_buffer[port]))	uart_write_index[port]=0;
		if(uart_write_index[port]==uart_read_index[port])	break;
	}
	__enable_irq();

	return i;
	
}

static int uart_data_read(uint8_t port, uint8_t *data, uint16_t len)
{
	int i;

	__disable_irq();
	for(i=0;i<len;i++)
	{		
		if(uart_read_index[port]==uart_write_index[port])	break;

		data[i]=uart_buffer[port][uart_read_index[port]++];
		if(uart_read_index[port]>=sizeof(uart_buffer[port]))	uart_read_index[port]=0;
	}
	__enable_irq();

	return i;
}

static void uart_data_clear(uint8_t port)
{
	uart_read_index[port]=uart_write_index[port];
}

static int uart_data_not_empty(uint8_t port)
{
	return (uart_read_index[port]!=uart_write_index[port]);
}

static void UART1_Configuration(int baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = baud;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
}
static void UART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void UART2_Configuration(int baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = baud;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
  USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
	
	//USART_ClearFlag(USART2, USART_FLAG_TC);

#if EN_USART2_RX	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
}


static void UART2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


static void UART3_Configuration(int baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10����ΪUSART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11��GPIOB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = baud;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	
  USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART3_RX	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
}


static void UART3_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


static void UART4_Configuration(int baud)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��UART4ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); //GPIOA0����ΪUSART4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); //GPIOA1����ΪUSART4
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0; //GPIOA1��GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = baud;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART3_RX	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif
}

static void UART4_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static int g_uart_init[UART_PORT_CNT]={0};

//USART3������
int uart_open(int port, int baud)
{
	int ret=-1;
	
	if(port>=UART_PORT_CNT)	return -1;
	if(g_uart_init[port])	return 0;

	switch(port)
	{ 	
		case 1:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
		UART1_Configuration(baud);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
		UART1_NVIC_Configuration();
			ret=0;
		break;

		case 2:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
			UART2_Configuration(baud);
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	
			UART2_NVIC_Configuration();
			ret=0;
		break;

		case 3:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
			UART3_Configuration(baud);
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
			UART3_NVIC_Configuration();
			ret=0;
		break;
		case 4:
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��USART4ʱ��
			UART4_Configuration(baud);
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	
			UART4_NVIC_Configuration();
			ret=0;
		break;
	}

	g_uart_init[port]=1;
	uart_data_clear(port);

	return ret;
}


int uart_close(int port)
{
	if(port>=UART_PORT_CNT)	return -1;
	if(!g_uart_init[port])	return 0;
	
	switch(port)
	{
		case 1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,DISABLE);//ʹ��USART1ʱ��
		break;

		case 2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE);//ʹ��USART2ʱ��
		break;

		case 3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,DISABLE);//ʹ��USART3ʱ��
		
		case 4:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,DISABLE);//ʹ��UART4ʱ��
		break;
	}

	g_uart_init[port]=0;

	return 0;
}



int uart_sendbuffer(int port, uint8_t *data, uint16_t len)
{
	int ret=-1;
	int i;

	if(port>=UART_PORT_CNT)	return -1;
	if(!g_uart_init[port])	return -2;

	switch(port)
	{	

		case 1:
			for(i=0;i<len;i++)
			{
				while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);//�ȴ��������ݼĴ���Ϊ��
				USART_SendData(USART1, data[i]);         //�򴮿�1��������
				
			}
			ret=i;
		break;

		case 2:
			for(i=0;i<len;i++)
			{
				while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!=SET);//�ȴ��������ݼĴ���Ϊ��
				USART_SendData(USART2, data[i]);         //�򴮿�2��������
			}
			ret=i;
		break;

		case 3:
			for(i=0;i<len;i++)
			{
				while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)!=SET);//�ȴ��������ݼĴ���Ϊ��
				USART_SendData(USART3, data[i]);         //�򴮿�3��������
			}
			ret=i;
		case 4:
			for(i=0;i<len;i++)
			{
				while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)!=SET);//�ȴ��������ݼĴ���Ϊ��
				USART_SendData(UART4, data[i]);         //�򴮿�4��������
			}
			ret=i;
		break;
	}

	return ret;
	
}



int uart_rcvbuffer(int port, uint8_t *data, uint16_t len, uint16_t timeout)
{
	int ret=-1;
	TimeOut_t tick;
	TickType_t timeout_tick=pdMS_TO_TICKS(timeout);
	int temp;

	if(port>=UART_PORT_CNT)	return -1;
	if(!g_uart_init[port])	return -2;

	ret=0;
	sys_get_tick(&tick);
	while(len)
	{
		//rcv buffer first
		if(uart_data_not_empty(port))
		{
			temp=uart_data_read(port, data, len);
			if(temp>0)
			{
				ret+=temp;
				data+=temp;
				len-=temp;
			}
			else if(temp<0)
			{
				break;
			}
		}

		//must check after rcv buffer
		if(sys_is_timeout(&tick, &timeout_tick))
			break;
	}
		

	return ret;
	
}


int uart_clearbuffer(int port)
{
	if(port>=UART_PORT_CNT)	return -1;
	if(!g_uart_init[port])	return -2;

	uart_data_clear(port);

	return 0;
}


/*
	uart send at command, first need to open uart port
	input: port,at_cmd,timeout,retry,rsp_size
	output:rsp
	return: <0 error, >=0 rcv len
*/

#define AT_DEBUG 1


// timeout: first char arrvied timeout.
// cwt_ms:  char wait timeout.
int uart_at_command_transceive(int port, 
											const char *const at_cmd, 
											uint8_t *const rsp, uint16_t rsp_size, 
											uint16_t timeout, 
											uint8_t retry,
											QueueHandle_t mutex,
											int cwt_ms)
{
	int ret;
	TimeOut_t old;
	int templen;
	TickType_t timeout_tick=pdMS_TO_TICKS(timeout);
	int rcv_start=0;

	if(port>=UART_PORT_CNT)	return -1;
	if(!g_uart_init[port])	return -2;
	if(!rsp)	return -3;
	if(!at_cmd)	return -4;
	if(!strstr(at_cmd, "\r"))	return -5;
	if(!mutex) return -6;
	if(cwt_ms==0)	cwt_ms=30;
	
RETRY:

	//take mutex
	xSemaphoreTake( mutex, portMAX_DELAY );
	#if AT_DEBUG==1
		printf("(%d,%d,%d)%s", port, strlen(at_cmd), rsp_size, at_cmd);
	#endif

	uart_data_clear(port);
	ret=uart_sendbuffer(port, (uint8_t*)at_cmd, strlen(at_cmd));
	if(ret<0)	
	{
		//give mutex
		xSemaphoreGive(mutex);
		return ret;
	}
	
	templen=0;
	rcv_start=0;
	sys_get_tick(&old);
	while(1)
	{
		if(templen>=rsp_size-1)
		{
			rsp[templen]=0;
			printf("rcv overflow (%d %d %d)%s", port, templen, rsp_size-1, rsp);
			break;
		}

		if(sys_is_timeout(&old, &timeout_tick))
		{
			if(rcv_start){
				rsp[templen]=0;
				
				#if AT_DEBUG==1
					printf("(%d %d)%s",port, templen, rsp);
				#endif

				break;	//rcv over
			}
			else{
				if(retry--){
					printf("rcv frame timeout retry (%d %d)", port, retry);
					printf("4444\r\n");
					goto RETRY;
				}
				else{
					printf("rcv frame timeout break(%d)", port);
					break;
				}
			}
		}

		ret=uart_rcvbuffer(port, &rsp[templen], rsp_size-templen-1, 0);
		if(ret<0){
			lite_debug("rcv buffer error(%d)", port);
			break;
		}
		else if(ret>0){
			templen+=ret;
			rcv_start=1;
			timeout_tick=pdMS_TO_TICKS(cwt_ms);
			sys_get_tick(&old);
		}
	}

	//give mutex
	xSemaphoreGive(mutex);
	return templen;
	
}

// timeout: first char arrvied timeout.
// cwt_ms:	char wait timeout.
int uart_at_command_transceive_ext(int port, 
											const uint8_t *const cmd,
											uint16_t len,
											uint8_t *const rsp, uint16_t rsp_size, 
											uint16_t timeout, 
											uint8_t retry,
											QueueHandle_t mutex,
											int cwt_ms)
{
	int ret;
	TimeOut_t old;
	int templen;
	int rcv_start=0;

	if(port>=UART_PORT_CNT)	return -1;
	if(!g_uart_init[port])	return -2;
	if(!rsp)	return -3;
	if(!cmd)	return -4;
	if(!mutex)	return -5;
	if(cwt_ms==0)	cwt_ms=30;

RETRY:

	//take mutex
	xSemaphoreTake( mutex, portMAX_DELAY );

	#if AT_DEBUG==1
		lite_debug("send len=(%d,%d,%d)", port, len, rsp_size);
	#endif

	uart_data_clear(port);
	ret=uart_sendbuffer(port, (uint8_t*)cmd, len);
	if(ret<0)	
	{
		//give mutex
		xSemaphoreGive(mutex);
		return ret;
	}
	
	TickType_t timeout_tick=pdMS_TO_TICKS(timeout);
	templen=0;
	rcv_start=0;
	sys_get_tick(&old);
	while(1)
	{
		if(templen>=rsp_size-1)
		{
			rsp[templen]=0;
			lite_debug("rcv overflow(%d) %d %d: %s", port, templen, rsp_size-1, rsp);
			break;
		}

		if(sys_is_timeout(&old, &timeout_tick))
		{
			if(rcv_start){
				rsp[templen]=0;
				
				#if AT_DEBUG==1
					lite_debug("(%d %d)%s",port, templen, rsp);
				#endif

				break;	//rcv over
			}
			else{
				if(retry--){
					lite_debug("rcv frame timeout retry(%d) %d", port, retry);
					goto RETRY;
				}
				else{
					lite_debug("rcv frame timeout break(%d)", port);
					break;
				}
			}
		}

		ret=uart_rcvbuffer(port, &rsp[templen], rsp_size-templen-1, 0);
		if(ret<0){
			lite_debug("rcv buffer error(%d)", port);
			break;
		}
		else if(ret>0){
			templen+=ret;
			rcv_start=1;
			timeout_tick=pdMS_TO_TICKS(cwt_ms);
			sys_get_tick(&old);
		}
	}

	//uart_data_clear(port);
	//give mutex
	xSemaphoreGive(mutex);
	return templen;
	
}



// timeout: first char arrvied timeout.
// cwt_ms:	char wait timeout.
int uart_at_command_recevier_ext(int port, 
											uint8_t *const rsp, uint16_t rsp_size, 
											uint16_t timeout, 
											QueueHandle_t mutex,
											int cwt_ms)
{
	int ret;
	TimeOut_t old;
	int templen;
	TickType_t timeout_tick=pdMS_TO_TICKS(timeout);
	int rcv_start=0;

	if(port>=UART_PORT_CNT) return -1;
	if(!g_uart_init[port])	return -2;
	if(!rsp)	return -3;
	if(!mutex)	return -5;
	if(cwt_ms==0)	cwt_ms=30;

//RETRY:

	//take mutex
	xSemaphoreTake( mutex, portMAX_DELAY );


	uart_data_clear(port);

	templen=0;
	rcv_start=0;
	sys_get_tick(&old);
	while(1)
	{
		if(templen>=rsp_size-1)
		{
			rsp[templen]=0;
			lite_debug("rcv overflow(%d) %d %d: %s", port, templen, rsp_size-1, rsp);
			break;
		}

		if(sys_is_timeout(&old, &timeout_tick))
		{
			if(rcv_start){
				rsp[templen]=0;
				
#if AT_DEBUG==1
					lite_debug("(%d %d)%s",port, templen, rsp);
#endif

				break;	//rcv over
			}
			else{

				lite_debug("rcv frame timeout break(%d)", port);
				break;
			}
		}

		ret=uart_rcvbuffer(port, &rsp[templen], rsp_size-templen-1, 0);
		if(ret<0){
			lite_debug("rcv buffer error(%d)", port);
			break;
		}
		else if(ret>0){
			templen+=ret;
			rcv_start=1;
			timeout_tick=pdMS_TO_TICKS(cwt_ms);
			sys_get_tick(&old);
		}
	}

	//uart_data_clear(port);
	//give mutex
	xSemaphoreGive(mutex);
	return templen;
	
}



void uart_test(int port)
{
	int ret;
	uint8_t temp[100];

	lite_debug("in %s", __func__);

	ret=uart_open(port, 115200);
	lite_debug("uart_open %d ret=%d", port, ret);
	
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(10));
		ret=uart_at_command_transceive(port, "at\r", temp, sizeof(temp), 1000, 0, 0, 30);
		if(ret>0)
		{
			lite_debug("uart_at_command_transceive ret=%d:", ret);
			int i=0;
			for(i=0;i<ret;i++)
			{
				log_printf("%02x ", temp[i]);
			}
		}
	
	/*
		vTaskDelay(pdMS_TO_TICKS(50));
		
		ret=uart_rcvbuffer(port, temp, sizeof(temp), 100);
		if(ret>0)
		{
			lite_debug("uart_rcvbuffer ret=%d:", ret);
			int i=0;
			for(i=0;i<ret;i++)
			{
				log_printf("%02x ", temp[i]);
			}

			ret=uart_sendbuffer(port, temp, ret);
			lite_debug("uart_sendbuffer ret=%d", ret);
		}*/
	}
	
	
}





#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
















