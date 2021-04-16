

#include "stm32f4xx_conf.h"


#define WIFI_UART_PORT 2

#define WIFI_DEBUG 1

#if WIFI_DEBUG==1
#define wifi_log	 printf
#define wifi_log_printf log_printf
#else
#define wifi_log(...)
#define wifi_log_printf(...)
#endif

static int g_wifi_init=0;
static QueueHandle_t g_wifi_uart_mutex=NULL;
static QueueHandle_t g_wifi_idle_mutex=NULL;


static int wifi_create_mutex(void)
{
	g_wifi_uart_mutex=xSemaphoreCreateMutex();
	if(g_wifi_uart_mutex==NULL)	
		return -1;

	g_wifi_idle_mutex=xSemaphoreCreateMutex();
	if(g_wifi_idle_mutex==NULL)
		return -2;

	return 0;
}


static int wifi_delete_mutex(void)
{
	vSemaphoreDelete(g_wifi_uart_mutex);
	vSemaphoreDelete(g_wifi_idle_mutex);
	return 0;
}


static TaskHandle_t g_wifi_task_handle=NULL;

static void wifi_task(void *para);

int wifi_create_task(void)
{
	BaseType_t xReturn;
	
	lite_debug("create wifi task ");
	xReturn = xTaskCreate(	(TaskFunction_t )wifi_task,
							(const char*    )"wifi_task",
							(uint16_t       )500,	//stack:2KB, cipstatus use about:50*5+16
							(void*          )NULL,
							(UBaseType_t    )NORMAL_TASK_PRIORITY, 
							&g_wifi_task_handle);
	lite_debug("ret=%ld", xReturn);

	return !xReturn;
}


int wifi_delete_task(void)
{
	lite_debug("delete wifi task");

	vTaskDelete(g_wifi_task_handle);
	
	return 0;
}


static wifi_state_enum_t g_wifi_state=WIFI_STATE_RESET;
static int g_wifi_task_delay=500;


static const char *g_wifi_state_string[]={
		"WIFI_STATE_PROBE",
		"WIFI_STATE_CONFIG",
		"WIFI_STATE_IDLE",
		"WIFI_STATE_TCP_CONNECT",
		"WIFI_STATE_TCP_SEND",
		"WIFI_STATE_TCP_RCV",
		"WIFI_STATE_TCP_DISCONNECT",
		"WIFI_STATE_RESET",
	};

static int wifi_change_state(wifi_state_enum_t state, int task_delay)
{	
	lite_debug("change state to %s", g_wifi_state_string[state]);

	g_wifi_state=state;
	g_wifi_task_delay=task_delay;
	if(g_wifi_task_delay==0)	g_wifi_task_delay=500;
	
	return 0;
}

static wifi_state_enum_t wifi_get_state(void)
{
	return g_wifi_state;
}




static int wifi_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//rst output high
  GPIO_InitStructure.GPIO_Pin = WIFI_RST_PIN; //WIFI_RST_PIN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(WIFI_RST_PORT,&GPIO_InitStructure); //初始化PA9，PA10
	GPIO_SetBits(WIFI_RST_PORT, WIFI_RST_PIN);
	
	//rts output high
  GPIO_InitStructure.GPIO_Pin = WIFI_UART_RTS_PIN;//WIFI_UART_RTS_PIN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(WIFI_UART_RTS_PORT, &GPIO_InitStructure);//初始化GPIO
	GPIO_ResetBits(WIFI_UART_RTS_PORT, WIFI_UART_RTS_PIN);
	
	//cts input high
  GPIO_InitStructure.GPIO_Pin = WIFI_UART_CTS_PIN;//WIFI_UART_CTS_PIN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(WIFI_UART_RTS_PORT, &GPIO_InitStructure);//初始化GPIO
	GPIO_ResetBits(WIFI_UART_CTS_PORT, WIFI_UART_CTS_PIN);


	//en output high
  GPIO_InitStructure.GPIO_Pin = WIFI_EN_PIN;//WIFI_EN_PIN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(WIFI_UART_RTS_PORT, &GPIO_InitStructure);//初始化GPIO
	GPIO_ResetBits(WIFI_EN_PORT, WIFI_EN_PIN);

	lite_debug("wifi gpio init");

	return 0;
	
}


int wifi_hard_reset(void)
{
	GPIO_ResetBits(WIFI_RST_PORT, WIFI_RST_PIN);
	delay_ms(50);
	GPIO_SetBits(WIFI_RST_PORT, WIFI_RST_PIN);
	delay_ms(50);

	lite_debug("wifi hard reset");

	return 0;
}


int wifi_power_on(void)
{
	lite_debug("wifi power on");
	
	GPIO_SetBits(WIFI_EN_PORT, WIFI_EN_PIN);
	return 0;
}

int wifi_power_off(void)
{
	lite_debug("wifi power off");

	GPIO_ResetBits(WIFI_EN_PORT, WIFI_EN_PIN);
	return 0;
}



/*
	wifi esp8266 chip driver init
	input: none
	output: none
	return: 0-success,others-fail
*/

int wifi_init(void)
{
	if(g_wifi_init)	return 0;

	lite_debug("wifi init");
	
	if(uart_open(WIFI_UART_PORT, 115200)){
		lite_debug("wifi uart %d open fail", WIFI_UART_PORT);
		return -1;
	}
	else{
		lite_debug("wifi uart %d open ok", WIFI_UART_PORT);
	}

	//gpio init
	wifi_gpio_init();
	//reset
	wifi_hard_reset();

	if(wifi_create_task())
	{
		lite_debug("wifi create task fail");
		goto FAIL;
	}

	if(wifi_create_mutex())
	{
		lite_debug("wifi create mutex fail");
		goto FAIL;
	}

	uart_clearbuffer(WIFI_UART_PORT);

	lite_debug("ok");
	g_wifi_init=1;

	return 0;

FAIL:
	lite_debug("fail");
	uart_close(WIFI_UART_PORT);
	wifi_delete_task();
	wifi_delete_mutex();
	wifi_power_off();
	
	return -2;
	
}


int wifi_deinit(void)
{
	if(!g_wifi_init)	return 0;

	lite_debug("wifi deinit");
	
	uart_close(WIFI_UART_PORT);
	wifi_delete_task();
	wifi_delete_mutex();
	wifi_power_off();
	
	g_wifi_init=0;
	return 0;
}





/*
	send AT test command to probe wifi module
	input: none
	output: none
	return: 0-success, others-fail
*/
int wifi_at_probe(void)
{
	char cmd[100];
	int ret;

	sprintf(cmd, "AT\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 500, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
	
}


/*
	wifi software reset
	input: none
	output: none
	return: 0-success,others-fail
*/
int wifi_software_reset(void)
{
	char cmd[100];
	int ret;

	sprintf(cmd, "AT+RST\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 500, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
}

/*
	wifi read module firmware version
	input: none
	outupt: version
	return: 0-success,others-fail
*/
int wifi_read_version(char *version, int version_buffer_size)
{
	char cmd[200];
	int ret;

	sprintf(cmd, "AT+GMR\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 200, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	//AT version:1.7.4.0(Jun 10 2020 12:25:03) OK
	if(strstr(cmd, "OK"))	
	{
		char *p=strstr(cmd, "AT version:");
		char *p2;
		if(!p)	return -3;

		p=strstr(p, ":");
		p+=1;
		p2=strstr(p, "(");
		if(!p2)	return -4;

		if((p2-p)<version_buffer_size)
		{
			memcpy(version, p, (p2-p));
			version[p2-p]=0;
		}
		
		return 0;
	}
	else	
	{
		return -2;
	}
}

/*
	wifi open or close command echo back
	input: option: 0-close,1-open
	output: none
	return: 0-success,others-fail
*/
int wifi_echo_control(int option)
{
	char cmd[100];
	int ret;

	if(option!=0 && option!=1)	return -1;

	sprintf(cmd, "ATE%d\r\n", option);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
}


/*
	wifi tcp data rcv mode
	input: option:  0-active mode,when data comming from tcp,module send to mcu directly
					1-passive mode,when data comming from tcp,module save it in module tcp buffer
	output: none
	return: 0-success,others-fail
*/
int wifi_tcp_rcvmode_control(int option)
{
	char cmd[100];
	int ret;

	if(option!=0 && option!=1)	return -1;

	sprintf(cmd, "AT+CIPRECVMODE=%d\r\n", option);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
}


/*
	wifi set wifi module mode
	input: wifi_mode
	output: none
	return: 0-success,other-fail
*/
int wifi_set_mode(wifi_mode_enum_t mode)
{
	char cmd[100];
	int ret;

	sprintf(cmd, "AT+CWMODE_DEF=%d\r\n", mode);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
	
}



/*
AT+UART_CUR=<baudrate>,<databits>,<stopbits>
,<parity>,<flow	control>

*/
// "230400, 8, 1, 0, 0"
int wifi_set_baudrate(int baud)
{
	char cmd[100];
	int ret;

	sprintf(cmd, "AT+UART_CUR=%d,8,1,0,0\r\n", baud);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}



	uart_close(WIFI_UART_PORT);

	if(uart_open(WIFI_UART_PORT, baud)){
		lite_debug("wifi uart %d open fail", WIFI_UART_PORT);
		return -1;
	}
	else{
		lite_debug("wifi uart %d open ok", WIFI_UART_PORT);
	}


	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
	
}

int wifi_get_baudrate()
{
	char cmd[100];
	int ret;

	sprintf(cmd, "AT+UART_CUR?\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}


	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
	
}

/*
	wifi set CWLAP(ap list) command format: sorted by rssi, enable all ap info
	input: sort,mask
	output: none
	return: 0-success,others-fail
*/
int wifi_set_CWLAP_format(int sort, int mask)
{
	char cmd[100];
	int ret;

	sprintf(cmd, "AT+CWLAPOPT=%d,%d\r\n", sort, mask);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}



	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
}



/*
	wifi list specific ap
	input: ap_name-ap name to list
	output: info
	return: 0-success,others-fail
*/

int wifi_list_ap_by_name(char *ap_name, wifi_ap_info_t *info)
{
	char cmd[400];
	int ret;

	if(!ap_name) 	return -1;
	if(!info)		return -2;
	if(wifi_tcp_is_available())	{
		memset(info, 0, sizeof(*info));
		return -2;
	}

	memset(info, 0, sizeof(*info));
	sprintf(cmd, "AT+CWLAP=\"%s\"\r\n", ap_name);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 3000, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	//+CWLAP:(3,"UROVOAP",-62,"30:7b:ac:69:41:90",1,-24,0,4,4,7,0) OK
	if(strstr(cmd, "OK"))	
	{
		char *p=strstr(cmd, "+CWLAP:(");
		char *p2;
		if(!p) return -3;

		p=strstr(p, "(");
		p+=1;
		sscanf(p, "%d", &info->ecn);

		p=strstr(p, ",\"");
		if(!p) return -4;
		p+=2;
		p2=strstr(p, "\"");
		if(!p2)	return -5;
		if(sizeof(info->ssid)>(p2-p))
		{
			memcpy(info->ssid, p, p2-p);
			info->ssid[p2-p]=0;
		}

		p=strstr(p, ",");
		if(!p) return -6;
		p+=1;
		sscanf(p, "%d", &info->rssi);

		p=strstr(p, "\"");
		if(!p) return -7;
		p+=1;
		p2=strstr(p, "\",");
		if(!p2) return -8;
		if(sizeof(info->mac)>(p2-p))
		{
			memcpy(info->mac, p, p2-p);
			info->mac[p2-p]=0;
		}

		p=strstr(p, ",");
		if(!p) return -9;
		p+=1;
		sscanf(p, "%d", &info->channel);

		p=strstr(p, ",");
		if(!p) return -10;
		p+=1;
		sscanf(p, "%d", &info->freq_offset);

		p=strstr(p, ",");
		if(!p) return -11;
		p+=1;
		sscanf(p, "%d", &info->freq_cali);

		p=strstr(p, ",");
		if(!p) return -12;
		p+=1;
		sscanf(p, "%d", &info->pairwise_cipher);

		p=strstr(p, ",");
		if(!p) return -13;
		p+=1;
		sscanf(p, "%d", &info->group_cipher);

		p=strstr(p, ",");
		if(!p) return -14;
		p+=1;
		sscanf(p, "%d", &info->bgn);

		p=strstr(p, ",");
		if(!p) return -15;
		p+=1;
		sscanf(p, "%d", &info->wps);
		
		return 0;
	}
	else	
	{
		return -2;
	}
}


/*
	wifi station connect to ap
	input: ssid,password
	output: none
	return: 0-success,others-fail
*/

int wifi_connect_ap(char *ssid, char *password)
{
	char cmd[100];
	int ret;
	
	if(!ssid)		return -1;
	if(!password)	return -1;
	if(wifi_tcp_is_available())	return -2;

	sprintf(cmd, "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", ssid, password);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 3000, 0, g_wifi_uart_mutex, 3000);
	if(ret<0){		
		return -1;
	}

	if(	strstr(cmd, "OK")){
		return 0;
	}
	else if(strstr(cmd, "WIFI CONNECTED") || 
			strstr(cmd, "WIFI GOT IP") || 
			strstr(cmd, "WIFI DISCONNECT")||
			ret==0)	//timeout
	{
		//connect ap is the cost most time, wait until it is over
		TimeOut_t tick;
		TickType_t timeout=pdMS_TO_TICKS(2000);
		sys_get_tick(&tick);
		while(1)
		{
			if(sys_is_timeout(&tick, &timeout))
				break;
			if(wifi_at_probe() == 0){
				if(ret)	return 0;	//previous is not timeout
				else	return -3;	//previous is timeout
			}
		}
		return -4;
	}
	else
		return -2;

}


/*
	wifi station disconnect ap
	input: none
	output: none
	return: 0-success,others-fail
*/
int wifi_disconnect_ap(void)
{
	char cmd[100];
	int ret;

	sprintf(cmd, "AT+CWQAP\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 500, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
}


/*
	wifi station list ap info connected
	input: none
	output: wifi_ap_info_t
	return: 0-success,others-fail
*/

int wifi_list_ap_void(wifi_ap_info_t *info)
{
	char cmd[200];
	int ret;

	//if(!ap_name) 	return -1;
	if(!info)		return -2;
	if(wifi_tcp_is_available())	{
		memset(info, 0, sizeof(*info));
		return -2;
	}

	memset(info, 0, sizeof(*info));
	sprintf(cmd, "AT+CWJAP_DEF?\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 3000, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	//+CWJAP_DEF:<ssid>,<bssid>,<channel>,<rssi> OK
	if(strstr(cmd, "OK"))	
	{
		char *p=strstr(cmd, "+CWJAP_DEF:");
		char *p2;
		if(!p) return -3;

		p=strstr(p, ":\"");
		if(!p) return -4;
		p+=2;
		p2=strstr(p, "\"");
		if(!p2)	return -5;
		if(sizeof(info->ssid)>(p2-p))
		{
			memcpy(info->ssid, p, p2-p);
			info->ssid[p2-p]=0;
		}

		p=strstr(p, ",\"");
		if(!p) return -7;
		p+=2;
		p2=strstr(p, "\",");
		if(!p2) return -8;
		if(sizeof(info->mac)>(p2-p))
		{
			memcpy(info->mac, p, p2-p);
			info->mac[p2-p]=0;
		}

		p=p2;

		p=strstr(p, ",");
		if(!p) return -9;
		p+=1;
		sscanf(p, "%d", &info->channel);

		p=strstr(p, ",");
		if(!p) return -6;
		p+=1;
		sscanf(p, "%d", &info->rssi);
		
		return 0;
	}
	else	
	{
		return -2;
	}
}



/*	
	wifi auto connect ap control
	input: option
	output: none
	return: 0-success,others-fail
*/
int wifi_auto_reconnect_ap_control(int option)
{
	char cmd[100];
	int ret;

	if(option!=0 && option!=1)	return -1;

	sprintf(cmd, "AT+CWAUTOCONN=%d\r\n", option);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 500, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
}




/*
	wifi tcp multiple socket control
	input: option: 1-enable multi socket, 0-disable
	output: none
	return: 0-success,others-fail
*/
int wifi_tcp_multi_socket_control(int option)
{
	char cmd[100];
	int ret;

	if(option!=0 && option!=1)	return -1;

	sprintf(cmd, "AT+CIPMUX=%d\r\n", option);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
}


/*	
	wifi get tcp socket status, only can get status if socket is created
	input: socket_id
	output: socket_status, stat
	return: 0-success,others-fail
*/
int wifi_tcp_get_status(int socket_id, wifi_socket_status_t *socket_status, wifi_status_enum_t *stat)
{
	char cmd[400];
	int ret;

	if(socket_id<0 && socket_id>4)	return -1;
	if(!socket_status) 	return -1;
	if(!stat)	return -1;

	memset(socket_status, 0, sizeof(*socket_status));
	sprintf(cmd, "AT+CIPSTATUS\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	//+CIPSTATUS:0,"TCP","10.10.11.35",1000,9551,0 OK
	if(strstr(cmd, "OK"))	
	{
		char str[20];
		char *p,*p2;
		
		p=strstr(cmd, "STATUS:");
		if(!p) return -3;
		p=strstr(p, ":");
		p+=1;
		sscanf(p, "%d", (int*)stat);

		//tcp not connected yet
		if(*stat!=WIFI_STATUS_CONNECT_TCP) {
			memset(socket_status, 0, sizeof(*socket_status));
			return 0;	
		}
		
		sprintf(str, "+CIPSTATUS:%d,", socket_id);
		p=strstr(cmd, str);
		if(!p) {
			p=strstr(cmd, "+CIPSTATUS:");	//tcp connect, but input socket_id is not the socket_id which is connect
			if(p) 	return 0;
			else 	return -4;
		}

		socket_status->link_id=socket_id;

		p=strstr(p, "\"");
		if(!p) return -5;
		p+=1;
		p2=strstr(p, "\"");
		if(!p2) return -6;
		if(sizeof(socket_status->type) > (p2-p))
		{
			memcpy(socket_status->type, p, p2-p);
			socket_status->type[p2-p]=0;
		}

		p=strstr(p, ",\"");
		if(!p) return -7;
		p+=2;
		p2=strstr(p, "\"");
		if(!p2) return -8;
		if(sizeof(socket_status->remote_ip) > (p2-p))
		{
			memcpy(socket_status->remote_ip, p, p2-p);
			socket_status->remote_ip[p2-p]=0;
		}

		p=strstr(p, "\",");
		if(!p) return -9;
		p+=2;
		sscanf(p, "%d", &socket_status->remote_port);

		p=strstr(p, ",");
		if(!p) return -10;
		p+=1;
		sscanf(p, "%d", &socket_status->local_port);

		p=strstr(p, ",");
		if(!p) return -11;
		p+=1;
		sscanf(p, "%d", &socket_status->tetype);
		
		return 0;
	}
	else	return -2;

	
}


/*
	wifi check tcp api is ready or not
	input: none
	output: none
	return: 0-success,others-fail
*/
int wifi_tcp_is_available(void)
{
	int ret=-1;

	switch(wifi_get_state())
	{
		case WIFI_STATE_PROBE:
		case WIFI_STATE_CONFIG:
		break;
		
		case WIFI_STATE_IDLE:		
		case WIFI_STATE_TCP_CONNECT:
		case WIFI_STATE_TCP_SEND:
		case WIFI_STATE_TCP_RCV:
		case WIFI_STATE_TCP_DISCONNECT:
			ret=0;
		break;

		case WIFI_STATE_RESET:
		break;
	}
lite_debug(">>>>>>>>>>>>>wifi_tcp_is_available = %d  wifi_get_state =%d\r\n",ret,wifi_get_state());
	return ret;
}



/*
	wifi tcp connect server
	input: socket_id, remote_ip, remote_port, keepalive_time_second
	output: none
	return: 0-success,others-fail
*/
int wifi_tcp_connect(int socket_id, char *remote_ip, int remote_port, int keepalive_time_second)
{
	char cmd[100];
	int ret;
	
	if(socket_id<0 && socket_id>4)	return -1;
	if(!remote_ip) return -1;
	if(wifi_tcp_is_available())	return -2;

	sprintf(cmd, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n", socket_id, remote_ip, remote_port, keepalive_time_second);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 2*1000, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	
	{
		char str[20];
		sprintf(str, "%d,CONNECT", socket_id);

		if(strstr(cmd, str)){
			delay_ms(200);	//some delay after tcp connect
			return 0;
		}
		else{
			return -3;
		}
	}
	else if(strstr(cmd, "ALREADY CONNECTED"))
		return 0;
	else	
		return -4;
	
}


/*
	wifi tcp send data
	input:socket_id,data,len
	output: none
	return: 0-success,others-fail
*/
#if 1
int wifi_tcp_send(int socket_id, uint8_t *data, int len)
{
#define  MAX_SEND_NETWORK_TIMEOUT	(2000)
	char cmd[100];
	int ret;
//	int extra_timeoutMs = 0;
	
	if(socket_id<0 && socket_id>4)	return -1;
	if(!data) return -1;
	if(wifi_tcp_is_available())	return -2;
	
	sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", socket_id, len);

	//take idle mutex
	xSemaphoreTake( g_wifi_idle_mutex, portMAX_DELAY );
	
	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 200, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){ 	
		//give idle mutex
		xSemaphoreGive(g_wifi_idle_mutex);
		return -1;
	}	
	if(strstr(cmd, "OK") && strstr(cmd, ">"))	
	{
		int busyFlag = 0;
		TimeOut_t tick;
		TickType_t timeout=pdMS_TO_TICKS(5000);
		sys_get_tick(&tick);


		while (1) {
			memset(cmd, 0, sizeof(cmd));
			if (busyFlag == 0) {
				ret=uart_at_command_transceive_ext(WIFI_UART_PORT, data, len, (uint8_t*)cmd, sizeof(cmd), 200, 0, g_wifi_uart_mutex, 30);//200);
				if(ret<=0){ 	
					//give idle mutex
					printf("Lib-wifi_tcp_send ret:%d...\r\n", ret);
					xSemaphoreGive(g_wifi_idle_mutex);
					return -1003;
				}
			}
			else {
				ret=uart_at_command_recevier_ext(WIFI_UART_PORT, (uint8_t*)cmd, sizeof(cmd), 200, g_wifi_uart_mutex, 30);//200);
				if(ret<=0){ 	
					//give idle mutex
					printf("Lib-wifi_tcp_send ret:%d...\r\n", ret);
					//xSemaphoreGive(g_wifi_idle_mutex);
					//return -1003;
					
					if(sys_is_timeout(&tick, &timeout)) {
						ret = -1001;
						break;
					}
					
					continue;
				}
			}
			

			char str[30];
			sprintf(str, "Recv %d bytes", len);

			if(strstr(cmd, "SEND OK")){
				//printf("recevie SEND OK...\r\n");
				ret = 0;
				break;
			} else if (strstr(cmd, "CLOSED")) {
				//printf("recevie CLOSED..\r\n");
				ret = -1004;
				break;
			} else if (strstr(cmd, "SEND FAIL")) {
				//printf("recevie SEND FAIL..\r\n");
				ret = -1005;
				break;
			} else if (strstr(cmd, "busy s...")) {
				//printf("recevie busy s...\r\n");
				busyFlag = 1;
				//continue;
			} else if(strstr(cmd, str)) {
				//printf("recevie Recv bytes...\r\n");
				busyFlag = 1;
				//continue;
			} else {
				printf("recevie other:%s...\r\n",cmd);
				busyFlag = 1;
			}

			if(sys_is_timeout(&tick, &timeout)) {
				ret = -1001;
				break;
			}

		}

		
		//give idle mutex
		xSemaphoreGive(g_wifi_idle_mutex);
		return ret;
	}
	else	
	{
		//give idle mutex
		xSemaphoreGive(g_wifi_idle_mutex);
		return -1002;
	}
	
}
#else
int wifi_tcp_send(int socket_id, uint8_t *data, int len)
{
#define  MAX_SEND_NETWORK_TIMEOUT	(2000)
	char cmd[100];
	int ret;
	int extra_timeoutMs = 0;
	
	if(socket_id<0 && socket_id>4)	return -1;
	if(!data) return -1;
	if(wifi_tcp_is_available())	return -2;
	
	sprintf(cmd, "AT+CIPSEND=%d,%d\r\n", socket_id, len);

	//take idle mutex

	xSemaphoreTake( g_wifi_idle_mutex, portMAX_DELAY );
	
	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 200, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){ 	
		//give idle mutex
		xSemaphoreGive(g_wifi_idle_mutex);
		return -1;
	}	

	if(strstr(cmd, "OK") && strstr(cmd, ">"))	
	{
		ret=uart_at_command_transceive_ext(WIFI_UART_PORT, data, len, (uint8_t*)cmd, sizeof(cmd), 200, 0, g_wifi_uart_mutex, 30);//200);
		if(ret<=0){ 	
			//give idle mutex
			xSemaphoreGive(g_wifi_idle_mutex);
			return -1;
		}
		
		//give idle mutex
		xSemaphoreGive(g_wifi_idle_mutex);

		char str[30];
		sprintf(str, "Recv %d bytes", len);

		if(strstr(cmd, "SEND OK")){
			return 0;
		}
		else if(strstr(cmd, str)){
			TimeOut_t tick;
			TickType_t timeout=pdMS_TO_TICKS(2000);
			lite_debug("enter wifi_tcp_send recv xx bytes");
			sys_get_tick(&tick);
			while(1)
			{
				if(sys_is_timeout(&tick, &timeout))
					break;
				if(wifi_at_probe() == 0){
					return 0;
				}
			}
			return -3;
		}
		else{
			return -4;
		}
		
	}
	else	
	{
		//give idle mutex
		xSemaphoreGive(g_wifi_idle_mutex);
		return -3;
	}
	
}
#endif

/*
	wifi tcp rcv data
	input: socket_id, read_data, expect_len, timeout_ms
	output: read_len
	return: 0-success,others-fail
*/

int wifi_tcp_read(int socket_id, uint8_t *read_data, int expect_len, int timeout_ms, int *read_len)
{
#define WIFI_MODULE_MAX_RECV_LEN		(2048)
	char cmd[WIFI_MODULE_MAX_RECV_LEN +100];
	int ret;
	int templen,rcvdlen;
	TimeOut_t tick;
	//TickType_t timeout=pdMS_TO_TICKS(timeout_ms+200);
	TickType_t timeout=pdMS_TO_TICKS(timeout_ms);

	if(socket_id<0 && socket_id>4)	return -1;
	if(!read_data) 	return -2;
	if(!read_len)	return -3;
	if(wifi_tcp_is_available())	return -4;

	*read_len=0;
	templen=0;
	rcvdlen=0;
	sys_get_tick(&tick);
	while(1)
	{
		if(sys_is_timeout(&tick, &timeout))
			break;

		if(rcvdlen>=expect_len)
			break;

		if(expect_len>WIFI_MODULE_MAX_RECV_LEN)	templen=WIFI_MODULE_MAX_RECV_LEN;
		else							templen=expect_len;

		sprintf(cmd, "AT+CIPRECVDATA=%d,%d\r\n", socket_id, templen);
		ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 100, 0, g_wifi_uart_mutex,100);
		if(ret<=0){
			return -5;
		}

		//+CIPRECVDATA,14:0123456789abcd OK
		if(strstr(cmd, "+CIPRECVDATA,")){

			char *p=strstr(cmd, "+CIPRECVDATA,");
			int len;
			p=strstr(p, ",");
			p+=1;
			
			sscanf(p, "%d", &len);
			p=strstr(p, ":");
			p+=1;
			
			memcpy(read_data+rcvdlen, p, len);
			rcvdlen+=len;
		}
		else if(strstr(cmd, "OK")){
			continue;	//if no data, only return "OK"
		}
		else if(strstr(cmd, "ERROR")){
			return -6;
		}
		else {
			return -7;
		}
		
	}

	*read_len=rcvdlen;

	return 0;
}


int wifi_tcp_close(int socket_id)
{
	char cmd[100];
	int ret;

	if(socket_id<0 && socket_id>4)	return -1;
	if(wifi_tcp_is_available())	return -3;

	sprintf(cmd, "AT+CIPCLOSE=%d\r\n", socket_id);

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 500, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	return 0;
	else	return -2;
}


int wifi_tcp_get_buffered_data_len(int socket_id, int *buffered_len)
{
	char cmd[100];
	int ret;

	if(socket_id<0 && socket_id>4)	return -1;
	if(wifi_tcp_is_available())	return -3;

	sprintf(cmd, "AT+CIPRECVLEN?\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, sizeof(cmd), 500, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		return -1;
	}

	if(strstr(cmd, "OK"))	
	{
		int len[5]={0};
		char *p=strstr(cmd, "+CIPRECVLEN:");
		if(!p) return -4;

		p=strstr(p, ":");
		p+=1;

		for(int i=0;i<5;i++)
		{
			sscanf(p, "%d", &len[i]);
			p=strstr(p, ",");
			if(!p) return -5;
			p+=1;
		}

		*buffered_len=len[socket_id];
		
		return 0;
	}
	else	return -2;
}


static volatile wifi_status_enum_t g_wifi_status=WIFI_STATUS_DISCONNECT_AP;
static volatile int g_wifi_rssi=0;

wifi_status_enum_t wifi_get_status(void)
{
	return g_wifi_status;
}


int wifi_refresh_icon(int rssi)
{
	static wifi_state_enum_t state_bak=WIFI_STATE_IDLE;
	static wifi_status_enum_t wifi_status_bak=WIFI_STATUS_DISCONNECT_AP;
	static int rssi_bak=0;
	int ret=-1;
	wifi_state_enum_t state=wifi_get_state();
	wifi_status_enum_t wifi_status=wifi_get_status();
	int rssi_index=1;

	if(	state_bak!=state ||
		wifi_status_bak!=wifi_status ||
		abs(rssi_bak-rssi)>=5)
	{
		state_bak=state;
		wifi_status_bak=wifi_status;
		rssi_bak=rssi;
		ret=0;
		if(rssi<0)
		{
			if(rssi<=(-90))			rssi_index=1;
			else if(rssi<=(-70))	rssi_index=2;
			else if(rssi<=(-50))	rssi_index=3;
			else					rssi_index=4;
		}

		lite_debug("wifi refresh state=%d status=%d rssi=%d", state, wifi_status, rssi);
		switch(state)
		{
			case WIFI_STATE_PROBE:
			case WIFI_STATE_CONFIG:
			case WIFI_STATE_RESET:
				//tft_display_wifi(0, 0);
			lite_debug("%s = %d\r\n",__func__,0);
			break;
			
			case WIFI_STATE_IDLE:		
			case WIFI_STATE_TCP_CONNECT:
			case WIFI_STATE_TCP_SEND:
			case WIFI_STATE_TCP_RCV:
			case WIFI_STATE_TCP_DISCONNECT:
				if(	wifi_status==WIFI_STATUS_CONNECT_AP)
					//tft_display_wifi(1, 0);
				lite_debug("%s = %d\r\n",__func__,1);
				else if(wifi_status==WIFI_STATUS_CONNECT_TCP || 
						wifi_status==WIFI_STATUS_DISCONNECT_TCP)
					//tft_display_wifi(1, rssi_index);
				lite_debug("%s = %d\r\n",__func__,rssi_index);
				else if(wifi_status==WIFI_STATUS_DISCONNECT_AP)
					lite_debug("%s = %d\r\n",__func__,wifi_status);
					//tft_display_wifi(0, 0);
			break;
		}
	}
	

	return ret;
}


static void wifi_task(void *para)
{
	int ret;
	int wifi_idle_check_fail_cnt=0;

	lite_debug("in %s", __func__);

	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(g_wifi_task_delay));

		wifi_refresh_icon(g_wifi_rssi);

		switch(wifi_get_state())
		{
			case WIFI_STATE_PROBE:
				wifi_log("wifi probe");
				ret=wifi_at_probe();
				if(!ret){
					char version[15];
					ret=wifi_read_version(version, sizeof(version));
					wifi_log("wifi version ret=%d,%s", ret,version);
					if(!ret){
						wifi_change_state(WIFI_STATE_CONFIG, 500);
					}
				}
			break;

			case WIFI_STATE_CONFIG:
				wifi_log("wifi_config");
				ret=wifi_echo_control(0);
				wifi_log("wifi close echo ret=%d", ret);
				ret=wifi_tcp_rcvmode_control(1);
				wifi_log("wifi set tcp rcv mode ret=%d", ret);
				ret=wifi_set_mode(WIFI_MODE_STATION);
				wifi_log("wifi set mode ret=%d", ret);
				ret=wifi_set_CWLAP_format(1, 2047);
				wifi_log("wifi CWLAP opt ret=%d", ret);
				ret=wifi_tcp_multi_socket_control(1);
				wifi_log("wifi tcp multi socket ret=%d", ret);
				ret=wifi_auto_reconnect_ap_control(1);
				wifi_log("wifi auto reconnect ret=%d", ret);
				if(!ret){
					wifi_change_state(WIFI_STATE_IDLE, 3000);
				}
				else{
					wifi_change_state(WIFI_STATE_PROBE, 500);
				}
			break;

			case WIFI_STATE_IDLE:
			{
				wifi_socket_status_t sock_stat;
				wifi_status_enum_t stat;
				wifi_ap_info_t info;

				xSemaphoreTake( g_wifi_idle_mutex, portMAX_DELAY );
				ret=wifi_tcp_get_status(0, &sock_stat, &stat);
				if(!ret)
				{
					wifi_list_ap_void(&info);
					g_wifi_rssi=info.rssi;
					//wifi_log("wifi rssi=%d", info.rssi);
				}
				xSemaphoreGive(g_wifi_idle_mutex);

				wifi_log("wifi idle get status ret=%d %d %d", ret, stat, wifi_idle_check_fail_cnt);
				if(ret && wifi_idle_check_fail_cnt++>2){
					wifi_idle_check_fail_cnt=0;
					wifi_change_state(WIFI_STATE_RESET, 500);
				}
				else{
					if(!ret){
						g_wifi_status=stat;				
					}
					wifi_idle_check_fail_cnt=0;
				}
			}
			break;

			case WIFI_STATE_TCP_CONNECT:
			case WIFI_STATE_TCP_DISCONNECT:
			case WIFI_STATE_TCP_SEND:
			case WIFI_STATE_TCP_RCV:
			break;

			case WIFI_STATE_RESET:

				g_wifi_status=WIFI_STATUS_DISCONNECT_AP;	//wifi status reset
				g_wifi_rssi=0;
			
				wifi_log("wifi reset");
				ret=wifi_software_reset();
				wifi_log("wifi software reset ret=%d", ret);
				wifi_change_state(WIFI_STATE_PROBE, 500);
			break;
		}
	}
	
}



int wifi_suspend_task(void)
{
	if(g_wifi_task_handle==NULL)	return -1;

	vTaskSuspend(g_wifi_task_handle);
	wifi_change_state(WIFI_STATE_RESET, 500);
	
	wifi_power_off();

	//tft_display_wifi(0, 0);	//clear wifi icon
	lite_debug("%s\r\n",__func__);
	return 0;
}


int wifi_resume_task(void)
{
	if(g_wifi_task_handle==NULL)	return -1;

	wifi_power_on();
	vTaskResume(g_wifi_task_handle);

	return 0;
}



/*
	wifi scan all ap around
	input: 	size- info item count in buffer
	output: info- buffer to store wifi ap info
	return: >=0-success, ap info item scanned
			others-fail
*/

#define WIFI_SCAN_AP_BUFFER_SIZE 300

int wifi_scan_ap(wifi_ap_info_t *info, int cnt)
{
	char *cmd;
	int ret;
	int i;

	if(!info)		return -17;
	if(wifi_tcp_is_available())	{
		memset(info, 0, sizeof(*info)*cnt);
		return -2;
	}

	//malloc memory
	cmd=(char*)pvPortMalloc(WIFI_SCAN_AP_BUFFER_SIZE);
	wifi_log("in %s malloc %x", __func__, cmd);

	i=0;
	memset(info, 0, sizeof(*info)*cnt);
	sprintf(cmd, "AT+CWLAP\r\n");

	ret=uart_at_command_transceive(WIFI_UART_PORT, cmd, (uint8_t*)cmd, WIFI_SCAN_AP_BUFFER_SIZE, 1000, 0, g_wifi_uart_mutex, 30);
	if(ret<=0){		
		goto SCAN_EXIT;
	}
	wifi_log("11111\r\n");
	//+CWLAP:(3,"UROVOAP",-62,"30:7b:ac:69:41:90",1,-24,0,4,4,7,0) OK
	if(strstr(cmd, "OK"))
	{
		//set original position
		char *p=cmd;

		for(i=0;i<cnt;i++)
		{
			p=strstr(p, "+CWLAP:(");
			char *p2;
			if(!p) 
			{
				ret=-3;
				goto SCAN_EXIT;
			}

			p=strstr(p, "(");
			p+=1;
			sscanf(p, "%d", &info->ecn);

			p=strstr(p, ",\"");
			if(!p) 
			{
				ret=-4;
				goto SCAN_EXIT;
			}
			p+=2;
			p2=strstr(p, "\"");
			if(!p2)	
			{
				ret=-5;
				goto SCAN_EXIT;
			}
			if(sizeof(info->ssid)>(p2-p))
			{
				memcpy(info->ssid, p, p2-p);
				info->ssid[p2-p]=0;
			}

			p=strstr(p, ",");
			if(!p) 
			{
				ret=-6;
				goto SCAN_EXIT;
			}
			p+=1;
			sscanf(p, "%d", &info->rssi);

			p=strstr(p, "\"");
			if(!p) 
			{
				ret=-7;
				goto SCAN_EXIT;
			}
			p+=1;
			p2=strstr(p, "\",");
			if(!p2) 
			{
				ret=-8;
				goto SCAN_EXIT;
			}
			if(sizeof(info->mac)>(p2-p))
			{
				memcpy(info->mac, p, p2-p);
				info->mac[p2-p]=0;
			}

			p=strstr(p, ",");
			if(!p) 
			{
				ret=-9;
				goto SCAN_EXIT;
			}
			p+=1;
			sscanf(p, "%d", &info->channel);

			p=strstr(p, ",");
			if(!p) 
			{
				ret=-10;
				goto SCAN_EXIT;
			}
			p+=1;
			sscanf(p, "%d", &info->freq_offset);

			p=strstr(p, ",");
			if(!p)
			{
				ret=-11;
				goto SCAN_EXIT;
			}
			p+=1;
			sscanf(p, "%d", &info->freq_cali);

			p=strstr(p, ",");
			if(!p) 
			{
				ret=-12;
				goto SCAN_EXIT;
			}
			p+=1;
			sscanf(p, "%d", &info->pairwise_cipher);

			p=strstr(p, ",");
			if(!p) 
			{
				ret=-13;
				goto SCAN_EXIT;
			}
			p+=1;
			sscanf(p, "%d", &info->group_cipher);

			p=strstr(p, ",");
			if(!p) 
			{
				ret=-14;
				goto SCAN_EXIT;
			}
			p+=1;
			sscanf(p, "%d", &info->bgn);

			p=strstr(p, ",");
			if(!p) 
			{
				ret=-15;
				goto SCAN_EXIT;
			}
			p+=1;
			sscanf(p, "%d", &info->wps);

			//if end or continue
			p2=strstr(p, "\r\nOK\r\n");
			if((p2-p) <= 4)
				break;
			else
				info++;
		}
		
		ret=0;
	}
	else	
	{
		ret=-18;
	}

	//free memory
	vPortFree(cmd);
	wifi_log("in %s free %x", __func__, cmd);

SCAN_EXIT:

	if(!ret)
	{
		ret=i;
		wifi_log("wifi_scan_ap exit  \r\n");
	}

	return ret;
	
}






#define AP_NAME 	"802A"
#define AP_PASSWORD "802A888888"
#define REMOTE_IP	"192.168.82.106"
#define REMOTE_PORT	8000


void wifi_test(void)
{
	wifi_ap_info_t ap_info[10]={0};
	int ret;
	int socket_id=1;
	uint8_t temp[100];
	
	int len;

	ret=wifi_list_ap_by_name(AP_NAME, &ap_info[0]);
	wifi_log("wifi list ap ret=%d,%d,%s,%d,%s,%d,%d,%d,%d,%d,%d,%d\r\n", ret, 
			ap_info[0].ecn, 
			ap_info[0].ssid,
			ap_info[0].rssi,
			ap_info[0].mac,
			ap_info[0].channel,
			ap_info[0].freq_offset,
			ap_info[0].freq_cali,
			ap_info[0].pairwise_cipher,
			ap_info[0].group_cipher,
			ap_info[0].bgn,
			ap_info[0].wps);

	//while(1)
	{		
		vTaskDelay(pdMS_TO_TICKS(2000));

		if(wifi_tcp_is_available())
			//continue;
		wifi_log("wifi tcp is not ready\r\n");
		else wifi_log("wifi tcp is ready\r\n");
		
		ret=wifi_connect_ap(AP_NAME, AP_PASSWORD);
		wifi_log("connect ap ret=%d\r\n", ret);
		if(!ret)
		{
			ret=wifi_list_ap_void(&ap_info[0]);
			wifi_log("list connected ap ret=%d\r\n", ret);
			if(!ret)
			{
				wifi_log("wifi list ap void ret=%d,%d,%s,%d,%s,%d,%d,%d,%d,%d,%d,%d\r\n", ret, 
						ap_info[0].ecn, 
						ap_info[0].ssid,
						ap_info[0].rssi,
						ap_info[0].mac,
						ap_info[0].channel,
						ap_info[0].freq_offset,
						ap_info[0].freq_cali,
						ap_info[0].pairwise_cipher,
						ap_info[0].group_cipher,
						ap_info[0].bgn,
						ap_info[0].wps);
			}

			ret=wifi_scan_ap(ap_info, sizeof(ap_info)/sizeof(ap_info[0]));
			wifi_log("wifi scan ap ret=%d\r\n", ret);
			if(ret>=0)
			{
				wifi_log("wifi scan ap:");
				for(int i=0;i<ret;i++)
				{
					wifi_log("%d,%s,%d,%s,%d,%d,%d,%d,%d,%d,%d\r\n", 
						ap_info[i].ecn, 
						ap_info[i].ssid,
						ap_info[i].rssi,
						ap_info[i].mac,
						ap_info[i].channel,
						ap_info[i].freq_offset,
						ap_info[i].freq_cali,
						ap_info[i].pairwise_cipher,
						ap_info[i].group_cipher,
						ap_info[i].bgn,
						ap_info[i].wps);
				}
			}			

			//for(int j=0;j<5;j++)
			{
			
				ret=wifi_tcp_connect(socket_id, REMOTE_IP, REMOTE_PORT, 60);
				wifi_log("tcp connect %d %s %d ret=%d\r\n", socket_id, REMOTE_IP, REMOTE_PORT, ret);

				if(!ret)
				{
					strcpy((char*)temp, "0123456789abcdefg");
					ret=wifi_tcp_send(socket_id, temp, strlen((char*)temp));	
					wifi_log("tcp send %d ret=%d", socket_id, ret);

					memset(temp, 0, sizeof(temp));
					ret=wifi_tcp_read(socket_id, temp, sizeof(temp), 2000, &len);
					wifi_log("tcp read %d ret=%d %d", socket_id, ret, len);
					if(!ret)
					{
						wifi_log("data:");
						for(int i=0;i<len;i++)
						{
							wifi_log("%02x ", temp[i]);
						}
					}
				}
				
				ret=wifi_tcp_close(socket_id);
				wifi_log("tcp close %d ret=%d", socket_id, ret);

			}
			
		}
		
		ret=wifi_disconnect_ap();
		wifi_log("disconnect ap ret=%d", ret);

		ret=wifi_list_ap_void(&ap_info[0]);
		wifi_log("list connected ap ret=%d", ret);
	}
	return;
}





