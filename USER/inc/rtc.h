#ifndef RTC_H
#define RTC_H

#include "stm32f4xx.h"

typedef struct 
{
  u8 year;
  u8 month;
  u8 day;
  u8 week;
  u8 hour;
  u8 minute;
  u8 second;	
}dateAndTime_t;
typedef	struct
{
	u8 hour;
	u8 minute;
	u8 second;
	u8 twentyMsCount;
}timeStamp_t;
extern dateAndTime_t  rtc_dev;

#define   MONDAY         1
#define   TUESDAY        2
#define   WEDNESDAY      3
#define   THURSDAY       4
#define   FRIDAY         5
#define   SATURDAY       6
#define   SUNDAY         7
u8 dectobcd(u8 dec);
u8 bcdtodec(u8 bcd);
void rtc_init(void);
u8 rtc_set(dateAndTime_t *rtc);
void rtc_get(dateAndTime_t*rtc);

#endif 


