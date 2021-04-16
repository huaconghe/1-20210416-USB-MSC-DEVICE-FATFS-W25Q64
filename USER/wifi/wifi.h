
#ifndef WIFI_H
#define WIFI_H


typedef enum{

	WIFI_STATE_PROBE,
	WIFI_STATE_CONFIG,
	WIFI_STATE_IDLE,
	WIFI_STATE_TCP_CONNECT,
	WIFI_STATE_TCP_SEND,
	WIFI_STATE_TCP_RCV,
	WIFI_STATE_TCP_DISCONNECT,
	WIFI_STATE_RESET,
}
wifi_state_enum_t;


typedef enum{

	WIFI_MODE_STATION=1,
	WIFI_MODE_AP=2,
	WIFI_MODE_STATION_AND_AP=3,
}
wifi_mode_enum_t;



typedef __packed struct
{
	int ecn;
	char ssid[50];
	int rssi;
	char mac[20];
	int channel;
	int freq_offset;
	int freq_cali;
	int pairwise_cipher;
	int group_cipher;
	int bgn;
	int wps;
}
wifi_ap_info_t;


typedef enum
{
	WIFI_STATUS_CONNECT_AP=2,
	WIFI_STATUS_CONNECT_TCP=3,
	WIFI_STATUS_DISCONNECT_TCP=4,
	WIFI_STATUS_DISCONNECT_AP=5,
}
wifi_status_enum_t;


typedef __packed struct
{
	int link_id;
	char type[10];
	char remote_ip[20];
	int remote_port;
	int local_port;
	int tetype;
}
wifi_socket_status_t;


#define WIFI_RST_PORT	GPIOE
#define WIFI_RST_PIN	GPIO_Pin_12

#define WIFI_EN_PORT	GPIOC
#define WIFI_EN_PIN		GPIO_Pin_9

#define WIFI_UART_RTS_PORT	GPIOE
#define WIFI_UART_RTS_PIN	GPIO_Pin_7

#define WIFI_UART_CTS_PORT	GPIOE
#define WIFI_UART_CTS_PIN	GPIO_Pin_6





int wifi_init(void);
int wifi_deinit(void);

wifi_status_enum_t wifi_get_status(void);

int wifi_tcp_connect(int socket_id, char *remote_ip, int remote_port, int keepalive_time_second);
int wifi_tcp_send(int socket_id, uint8_t *data, int len);
int wifi_tcp_read(int socket_id, uint8_t *read_data, int expect_len, int timeout_ms, int *read_len);
int wifi_tcp_close(int socket_id);
int wifi_tcp_get_buffered_data_len(int socket_id, int *buffered_len);

int wifi_disconnect_ap(void);
int wifi_connect_ap(char *ssid, char *password);
int wifi_list_ap_void(wifi_ap_info_t *info);
int wifi_scan_ap(wifi_ap_info_t *info, int cnt);


int wifi_suspend_task(void);
int wifi_resume_task(void);

int wifi_tcp_is_available(void);







#endif
