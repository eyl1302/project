#ifndef _RTC_QUEUE_H_
#define _RTC_QUEUE_H_

#include "common.h"

#define RTC_QUEUE_SIZE	20

typedef enum
{
	RQ_AUTOTEST = 1,	//	Auto Test

} RTC_QUEUE_CMD_T;

typedef struct
{
	u8 	cmd;			//	RTC_QUEUE_CMD_T
	u16 year;			//	Year
	u8	mon;			//	Month
	u8	day;			//	Day
	u8  wday;			//	Day of Week (1:Monday, 7:Sunday)
	u8	hour;			//	Hour
	u8	min;			//	Minute
	u8	sec;			//	Second

}	RTC_QUQEUE_T;

//	Function
void fnRTCQueueInit(void);
void fnRTCQueuePush(RTC_QUQEUE_T rtc);
u8   fnRTCQueuePop(RTC_QUQEUE_T *rtc);
void fnRTCQueueFree(void);

#endif	//	_RTC_QUEUE_H_
