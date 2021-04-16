

#ifndef SDK_H
#define SDK_H


#include "stdint.h"



typedef struct {
unsigned short year; // 2000 ~ 2049
unsigned short mon; // 1 - 12
unsigned short day; // 1 - 31
unsigned short hour; // 0 - 23
unsigned short min; // 0 - 59
unsigned short sec; // 0 - 59
unsigned short wday; // 0 - 6
}tsSysTime;

#define MAX_CFG 1

typedef struct
{
	char OsVer[32]; //OS版本
	char BootVer[32]; //BOOT版本
	char Pid[32]; //机器型号
	char CfgVer[32]; //配置文件版本
	unsigned int FDebug; //固件调试
	unsigned int ADebug; //软件调试
	unsigned int DevicCfg[MAX_CFG]; //硬件配置
}ST_SP_INFO;

typedef struct {
	unsigned int fColor;
	unsigned int bColor;
	unsigned char Mode;
	unsigned char Type;
	unsigned char Size;
}tsFont;

typedef struct {
	short Width;
	short Height;
} tsScrInfo;

typedef struct {
	short Left; // 横坐标
	short Top; // 纵坐标
	short Width; // 宽度
	short Height;// 高度
}tsRect;

typedef struct {
	int gprsState; //1 可以进行拨号。0 不能进行拨号。
	int roaming;
	int mcc;
	int mnc;
	int mnc_num;
	int lac;
	int cid;
}tsNetWorkInfo;


typedef enum
{
	DEV_ID_GPRS=0,
	DEV_ID_WIFI=1,
	DEV_ID_NULL=2,
}
comm_device_id_enum;

typedef struct
{
	char name[16+1];//文件名
	unsigned int length;//文件大小
} FILE_INFO;




//system api
unsigned int GetSysTickCount(void);
void DelayMs(unsigned int ms);
int SetSysLocalTime(tsSysTime *time);
int GetSysLocalTime(tsSysTime *time);
void SetSysSleepEnabled(int Level);
void SetSysSleepTime(int tsec);
int GetRandom(unsigned int len, unsigned char* rand);
int GetSN(unsigned char type, char* sn);
int GetSysbatteryState(void);
void app_printf(char *fmt,...);
int GetSysStatus(void);
void s_GetSpInfo(ST_SP_INFO* pstInfo);
void SysPowerOff(int flag);
int SysGetMainAppVersion(uint8_t *pVersion);
void SetSysDeepSleepTime(int tmin);
int GetSysbatteryValue(void);
int GetSysInfo(char *info, int flag);


//lcd
void ScrCls(void);
void ScrGotoXY(unsigned short X, unsigned short Y);
void ScrTextOut (unsigned char *ptext);
void ScrSetFont (tsFont *pFont);
void ScrGetFont (tsFont *pFont);
void ScrBitMap (tsRect *pRect, unsigned char* pBitmap);
void ScrGetStatus (tsScrInfo *pScrInfo);
void ScrSetBackLight(unsigned short level);
void ScrRectClear(tsRect *prect);
void ScrRefresh(void);
void ScrSetPixel(unsigned short color);
void ScrDrawRect(tsRect *rect, short penw, short color);
void ScrFillRect(tsRect *rect, short color);
void ScrPrintf(char *fmt, ...);
int ScrShowQrCode(tsRect *rect, char *paystr);
int ScrShowLed(char *digit_str);
int CLScLedTurnOn(unsigned char mask);
int CLScLedTurnOff(unsigned char mask);



//keyboard
int KbHit(void);
void KbFlush(void);
int KbGetKey(void);


//scan qr code
int ScanBarStart(void);
int ScanBarGetData(unsigned char *pdata, int buf_sz, int tov);
int ScanBarStop(void);
int ScanBarLightSet(unsigned int conf_param, unsigned int mask);
int ScanRefundCodeBarStart(void);



//net

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
comm_wifi_scan_t;


int CommDeviceOpen(int id);
int CommDeviceClose(int id);
int CommSetDeviceInfo(int id, int type, void *info);
int CommGetDeviceInfo(int id, int type, void *info, int size);
int CommDeviceConnect(int id, char *user_name, char *psw);
int CommDeviceDisconnect(int id);
int CommDeviceCheck(int id, unsigned int *Status);
int CommSocketConnect(int id, int type, char *remote_addr, unsigned short port);
int CommSocketClear(int socket);
int CommSocketCheck(int socket, unsigned int *signal);
int CommSocketSend(int socket, unsigned char *sendbuf, int len);
int CommSocketRecv(int socket, unsigned char *recvbuf, int len);
int CommSocketClose(int socket);

int local_net_resolve_ipv4(const char *domain_name, unsigned int *ipv4_array,
                          unsigned int array_size, unsigned int *ret_num, unsigned int timeout_ms);
                          
int local_get_bs_info(	char *mcc, unsigned int mcc_buf_len, 
								char *mnc, unsigned int mnc_buf_len,
                     			char *lac, unsigned int lac_buf_len,
                     			char *cellid, unsigned int cellid_buf_len,
                     			char *rss, unsigned int rss_buf_len);


int local_get_network_type(void);
int local_get_wifi_signal_level(char *level, unsigned level_buf_len);


//audio
void SpeakerPlayText(char *fmt,...);
int SpeakerSetVolume(int level);
int SpeakerBeepPlay(uint32_t freq, uint8_t time);
int SpeakerSetParams(int speed, int pitch);
int SpeakerGetParams(int *speed, int *pitch);
int SetSystemPromptTone(int type, int value);
int SpeakerPinkNoise(void);
int Speaker0DB(void);
int SpeakerPlayIsBusy(void);
int SpeakerPlayNumber(const double number);

//file system

//define for FsSeek origin
#define SEEKSET 0
#define SEEKCUR 1
#define SEEKEND 2

//define for FsOpen mode
#define O_CREATE 	1
#define O_RDWR		2



//thread
void *local_thread_create(unsigned short stack_size, void *(*fn)(void *), void *arg);
int local_thread_destroy(void *thread);
void *local_thread_self(void);

//sem
void *local_sem_create(unsigned int n);
void local_sem_destroy(void *sem);
void local_sem_post(void *sem);
int local_sem_wait(void *sem, unsigned int timeout_ms);


//mutex
void *local_mutex_create(void);
void local_mutex_destroy(void *mutex);
void local_mutex_lock(void *mutex);
void local_mutex_unlock(void *mutex);
int local_mutex_try_lock(void *mutex);


//se
int upos_se_trigger_get(unsigned char *tri, unsigned int *tri_len);
/**
	*	return
	*		0x17: serial key not downloaded
	*		0x16: already downloaded
	*/
int upos_se_snkey_status(void);







#endif



