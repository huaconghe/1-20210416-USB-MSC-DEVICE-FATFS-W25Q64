

#include "stm32f4xx_conf.h"
#include "sdk.h"



unsigned int GetSysTickCount(void)
{
	TickType_t tick;
	
	tick=xTaskGetTickCount();

	return tick;
}


static comm_device_id_enum g_comm_device_id=DEV_ID_NULL;

void DelayMs(unsigned int ms)
{
	delay_ms(ms);
}

//使能通信设备
//id : 网络设备id,0 为GPRS, 1 为WIFI, 2 为LAN
//= 0 - 设备打开成功。
//< 0 - 设备打开失败。
int CommDeviceOpen(int id)
{
	int ret=-1;

	switch(id)
	{
		//gprs
		case 0:
			//4g always enabled
//			cat1_resume_task(1);
//			ret=0;
//			g_comm_device_id=DEV_ID_GPRS;
		break;

		//wifi
		case 1:
			//wifi always enabled
			wifi_resume_task();
			ret=0;
			g_comm_device_id=DEV_ID_WIFI;
		break;
	}

	return ret;
}
//关闭通信设备
//id : 通信设备id
//= 0 - 设备关闭成功。
//< 0 - 设备关闭失败。
int CommDeviceClose(int id)
{
	int ret=-1;

	if(id != g_comm_device_id)
	{
		lite_debug("close wrong dev_id %d", id);
		return -2;
	}
	
	switch(id)
	{
		//gprs
		case 0:
//			cat1_suspend_task(1);
//			ret=0;
//			g_comm_device_id=DEV_ID_NULL;
		break;

		//wifi
		case 1:
			wifi_suspend_task();
			ret=0;
			g_comm_device_id=DEV_ID_NULL;
		break;
	}	

	return ret;
}

//设置通信设备信息
//ID : 通信设备id。
//Type : 设置类型。
//Info : [入参]设置信息。
//>= 0 - 设置成功。
//< 0 - 设置失败。
int CommSetDeviceInfo(int id, int type, void *info)
{	
	return -1;
}


//获取通信设备信息，如GPRS 的APN 等
//ID : 通信设备id。
//Type : 获取类型。
//Info : [入参]获取信息。
int CommGetDeviceInfo(int id, int type, void *info, int size)
{
	int ret=-1;

	switch(id)
	{
		case 0:
			//id=0,type=1
//			if(type==1)
//			{
//				int mcc_temp[10],mnc_temp[10],lac_temp[10],cellid_temp[10],rss_temp[10];
//				tsNetWorkInfo *net_info=info;
//				int cnt=size/sizeof(*net_info);
//				if(cnt)
//				{
//					ret = cat1_get_cell_info( mcc_temp, 10, 
//											  mnc_temp, 10, 
//											  lac_temp, 10, 
//											  cellid_temp, 10, 
//											  rss_temp, 10);
//					if(ret>0)
//					{
//						for(int i=0;(i<ret)&&(i<cnt);i++)
//						{
//							net_info[i].gprsState=1;
//							net_info[i].roaming=0;
//							net_info[i].mcc=mcc_temp[i];
//							net_info[i].mnc=mnc_temp[i];
//							net_info[i].mnc_num=1;
//							net_info[i].lac=lac_temp[i];
//							net_info[i].cid=cellid_temp[i];
//						}
//					}
//				}
//			}
		break;
	
		case 1:
			//id==1,type==0
			if(type==0)
			{
				wifi_ap_info_t *wifi_info=info;
				int cnt=size/sizeof(*wifi_info);

				if(cnt)
				{
					ret=wifi_scan_ap(wifi_info, cnt);
				}
			}
		break;
	}

	return ret;
}

//连接远端设备
//ID : 通信设备id。
//User_name : 用户名。
//psw : 密码。
//=0 - 连接成功。
//<0 - 连接失败。
int CommDeviceConnect(int id, char *user_name, char *psw)
{
	int ret=-1;

	switch(id)
	{
		//wifi
		case 1:
			ret=wifi_connect_ap(user_name, psw);
		break;
	}

	return ret;
}

//检测通信设备状态。
//ID : 通信设备id。
//status : [出参]检测的状态值。
//0 Bit 0 : 1 有GPRS 信号 0 没有GPRS 信号
//	BIT 1 : 1 PPP 拨号关闭
//	Bit 2 : 1 PPP 正在拨号
//	Bit 3 : 1 PPP 拨号成功
//	Bit 4 : 1 PPP 拨号失败
//	Bit 5 : 1 网络繁忙
//1 Bit 0 : 1 WIFI 未连接
//	Bit 1 : 1 WIFI 关闭态
//	Bit 2 : 1 WIFI 已连接，未取得IP
//	Bit 3 : 1 WIFI 已连接，并获得IP
//2 Bit 0 : 1 LAN 口未连接
//	Bit 3 : 1 LAN 口连接正常
//=0 - 检测成功。
//<0 - 检测失败
int CommDeviceCheck(int id, unsigned int *Status)
{
	int ret=-1;

	switch(id)
	{
		//gprs
		case 0:
		{
//			cat1_state_enum_t state;
//			state=cat1_get_state();

//			ret=0;
//			*Status=0;
//			switch(state)
//			{
//				case STATE_PROBE:
//				case STATE_CHECK_SIM:
//				break;
//				
//				case STATE_CHECK_4G_SERVICE:
//				case STATE_CHECK_4G_ACTIVE:
//					*Status=(1<<0)|(1<<2);
//				break;
//				
//				case STATE_TCP_CONNECT:
//				case STATE_TCP_SEND:
//				case STATE_TCP_RCV:
//				case STATE_TCP_ALIVE:
//				case STATE_TCP_DISCONNECT:
//				case STATE_IDLE:
//					*Status=(1<<0)|(1<<3);
//				break;
//				
//				case STATE_RESET:				
//				case STATE_ERROR:
//					*Status=(1<<4)|(1<<5);
//				break;
//			}
		}
		break;

		//wifi
		case 1:
		{
			wifi_status_enum_t wifi_status;
			wifi_status=wifi_get_status();
			int wifi_available=wifi_tcp_is_available();

			ret=0;
			*Status=0;
			
			if(wifi_available == 0)	//wifi is available
			{
				switch(wifi_status)
				{
					case WIFI_STATUS_CONNECT_AP:
						*Status=(1<<2);
					break;
					case WIFI_STATUS_CONNECT_TCP:
						*Status=(1<<3);
					break;
					case WIFI_STATUS_DISCONNECT_TCP:
						*Status=(1<<2);
					break;
					case WIFI_STATUS_DISCONNECT_AP:
						*Status=(1<<0);
					break;
				}
			}
			else	//wifi is not available
			{
				*Status=(1<<1);


			}
		}
		break;
	}

	return ret;
}
static const int g_socket_id_table[2][2]={
											{1,2}, 	//gprs
											{1,2},	//wifi
										 };
static int g_socket_id_valid[2][2]={0};

int sdk_net_get_free_socket_id(comm_device_id_enum dev_id, int *socket_id)
{
	if(dev_id >= DEV_ID_NULL) return -2;

	for(int i=0;i<sizeof(g_socket_id_valid[dev_id])/sizeof(g_socket_id_valid[dev_id][0]);i++)
	{
		if(g_socket_id_valid[dev_id][i]==0)
		{
			*socket_id=g_socket_id_table[dev_id][i];
			return 0;
		}
	}

	return -1;
}

int sdk_net_set_socket_id_valid(comm_device_id_enum dev_id, int socket_id)
{
	if(dev_id >= DEV_ID_NULL) return -2;

	for(int i=0;i<sizeof(g_socket_id_table[dev_id])/sizeof(g_socket_id_table[dev_id][0]);i++)
	{
		if(g_socket_id_table[dev_id][i] == socket_id)
		{
			g_socket_id_valid[dev_id][i]=1;
			return 0;
		}
	}

	return -1;
}

int sdk_net_clear_socket_id_valid(comm_device_id_enum dev_id, int socket_id)
{
	if(dev_id >= DEV_ID_NULL) return -2;

	for(int i=0;i<sizeof(g_socket_id_table[dev_id])/sizeof(g_socket_id_table[dev_id][0]);i++)
	{
		if(g_socket_id_table[dev_id][i] == socket_id)
		{
			g_socket_id_valid[dev_id][i]=0;
			return 0;
		}
	}

	return -1;
}

int sdk_net_socket_id_is_valid(comm_device_id_enum dev_id, int socket_id)
{
	if(dev_id >= DEV_ID_NULL) return 0;
	
	for(int i=0;i<sizeof(g_socket_id_table[dev_id])/sizeof(g_socket_id_table[dev_id][0]);i++)
	{
		if(g_socket_id_table[dev_id][i] == socket_id)
		{
			if(g_socket_id_valid[dev_id][i]==1)
				return 1;
		}
	}

	return 0;
}
//与远程后台主机建立SOCKET 网络通信连接
//Id : 网络设备id。
//type : 网络类型(0 – TCP/1 - UDP)。
//remote_addr : 远程主机地址(形如“192.168.0.1”或者”www.baidu.com”)。
//port : 应用端口号。
//>= 0 - 创建连接成功，返回套接字标识(socket_id)。
//< 0 - 创建连接失败。
int CommSocketConnect(int id, int type, char *remote_addr, unsigned short port)
{
	int ret=-1;
	int socket_id;

	if(g_comm_device_id!=(comm_device_id_enum)id)
	{
		lite_debug("device id not equal to value set in CommOpen");
		return -2;
	}
	
	ret=sdk_net_get_free_socket_id((comm_device_id_enum)id, &socket_id);
	if(ret) 
	{
		lite_debug("can not find free socket id");
		return -3;
	}
	else
	{	
		lite_debug("find free socket id %d %d", id, socket_id);
	}

	switch(id)
	{
		//gprs
		case 0:
//			ret=cat1_socket_open(socket_id, remote_addr, port);
//			if(!ret){
//				ret=cat1_socket_keep_alive(1, 60, 10, 3);
//			}
		break;

		//wifi
		case 1:
			ret=wifi_tcp_connect(socket_id, remote_addr, port, 60);
		break;

	}

	if(!ret)
	{
		//g_comm_device_id=(comm_device_id_enum)id;
		sdk_net_set_socket_id_valid(g_comm_device_id, socket_id);
		ret=socket_id;
	}

	return ret;
}


//断开远端设备
//ID : 通信设备id。
//=0 - 断开成功。
//<0 - 断开失败。
int CommDeviceDisconnect(int id)
{
	int ret=-1;

	switch(id)
	{
		//wifi
		case 1:
			ret=wifi_disconnect_ap();
		break;
	}

	return ret;
}


//发送数据到远程后台主机
//socket : 套接字标识符。
//send : 待发送数据缓冲区。
//len : 待发送数据长度。
//>= 0 - 发送成功。
//< 0 - 发送失败。
int CommSocketSend(int socket, unsigned char *sendbuf, int len)
{
	int ret=-1;
	int socket_id=socket;

	if(!sdk_net_socket_id_is_valid(g_comm_device_id, socket_id))
	{
		lite_debug("socket id is not valid %d %d", g_comm_device_id, socket_id);
		return -1;
	}

	switch(g_comm_device_id)
	{
		case DEV_ID_GPRS:
//			ret=cat1_socket_send(socket_id, sendbuf, len);
		break;

		case DEV_ID_WIFI:
			ret=wifi_tcp_send(socket_id, sendbuf, len);
		break;
	}

	if(!ret)
	{
		ret=len;
	}

	return ret;
}

//从远程主机接收数据
//socket : 套接字标识符。
//send : 数据接收缓冲区。
//len : 要接收的数据长度。
//>= 0 - 接收到的实际数据长度。
//<0 - 接收失败。
int CommSocketRecv(int socket, unsigned char *recvbuf, int len)
{
	int ret=-1;
	int socket_id=socket;

	if(!sdk_net_socket_id_is_valid(g_comm_device_id, socket_id))
	{
		lite_debug("socket id is not valid %d %d", g_comm_device_id, socket_id);
		return -1;
	}

	switch(g_comm_device_id)
	{
		case DEV_ID_GPRS:
		{
			int readlen;
//			ret=cat1_socket_read(socket_id, recvbuf, len, 30, &readlen);
			if(!ret)
			{	
				ret=readlen;
			}
		}
		break;

		case DEV_ID_WIFI:
		{
			int readlen;
			ret=wifi_tcp_read(socket_id, recvbuf, len, 30, &readlen);
			if(!ret)
			{	
				ret=readlen;
			}
		}
		break;
	}

	return ret;
	
}



//释放一个网络通信的套接字资源，包括连接等资源。
//socket : 套接字标识符
//=0 - 关闭成功。
//<0 - 关闭失败。
int CommSocketClose(int socket)
{
	int ret=-1;
	int socket_id=socket;
	
	if(!sdk_net_socket_id_is_valid(g_comm_device_id, socket_id))
	{
		lite_debug("socket id is not valid %d %d", g_comm_device_id, socket_id);
		return -1;
	}


	switch(g_comm_device_id)
	{
		case DEV_ID_GPRS:
//			ret=cat1_socket_close(socket_id);
		break;

		case DEV_ID_WIFI:
//			ret=wifi_tcp_close(socket_id);
		break;
	}

	//if(!ret)
	{
		sdk_net_clear_socket_id_valid(g_comm_device_id, socket_id);
	}

	return ret;
}

void sdk_wifi_test()
{	
	int ret,fd, count;
	uint32_t status;
	uint8_t temp[1000];
	uint8_t test_send[1000];
	comm_wifi_scan_t *info=(comm_wifi_scan_t*)temp;
	int size=sizeof(temp);

	#define AP_NAME 	"802A"
	#define AP_PASSWORD "802A888888"
	#define REMOTE_IP	"192.168.82.106"
	#define REMOTE_PORT	8080



	ret=CommDeviceOpen(1);
	lite_debug("wifi open ret=%d", ret);

	while(1)
	{
		delay_ms(1000);
		ret=CommDeviceCheck(1, &status);
		lite_debug("wifi check ret=%d, status=0x%x", ret, status);
		if(ret || !(status&(1<<1)))
			break;
	}

//	//CommDeviceSetBaudate(1, 230400);

//	ret=CommDeviceConnect(1, AP_NAME, AP_PASSWORD);
//	lite_debug("wifi connect ap ret=%d", ret);
//	if(ret)
//	{
//		lite_debug("wifi connect ap fail %d", ret);
//		goto FAIL;
//	}

	ret=CommGetDeviceInfo(1, 0, info, size);
	lite_debug(">>>wifi scan ret = %d",ret);
	if(ret>=0)
	{
		for(int i=0;i<ret;i++)
		{
			lite_debug("%d,%s,%d,%s,%d,%d,%d,%d,%d,%d,%d", 
				info[i].ecn, 
				info[i].ssid,
				info[i].rssi,
				info[i].mac,
				info[i].channel,
				info[i].freq_offset,
				info[i].freq_cali,
				info[i].pairwise_cipher,
				info[i].group_cipher,
				info[i].bgn,
				info[i].wps);
		}
	}

	delay_ms(1000);
//if(ret>=0)
{
	fd=CommSocketConnect(1, 0, REMOTE_IP, REMOTE_PORT);
	lite_debug("wifi tcp connect ret=%d", fd);

	delay_ms(1000);
	memset(test_send, 0x33, sizeof(test_send));
	count = 3;
	if(fd>=0)
	{
		delay_ms(2000);

		while (count--) 
		//while(1)
		{
			//ret=CommSocketSend(fd, "0123456789abcdef", 15);
			#if 0
			ret=CommSocketSend(fd, test_send, sizeof(test_send));
			lite_debug("wifi tcp send ret=%d", ret);
			#endif
			memset(temp, 0, sizeof(temp));
			ret=CommSocketRecv(fd, temp, sizeof(temp));
			lite_debug("wifi tcp read ret=%d\r\n", ret);
			printf("wifi tcp read ret=%d\r\n", ret);
			if(ret>0)
			{
					for(int i=0;i<ret;i++)
					{
						printf("%02x ", temp[i]);
					}							}
		}
		ret=CommSocketClose(fd);
		lite_debug("wifi tcp close ret=%d", ret);

		delay_ms(1000);
	}

	ret=CommDeviceDisconnect(1);
	lite_debug("wifi disconnect ap ret=%d", ret);

	//CommDeviceSetBaudate(1, 115200);

}


	ret=CommDeviceClose(1);
	lite_debug("wifi close ret=%d", ret);

	
}
















