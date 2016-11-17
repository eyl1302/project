#ifndef _RTC_COMMON_H_
#define _RTC_COMMON_H_

#include "rtcQueue.h"

#define LEAPS_THRU_END_OF(y) 	((y)/4 - (y)/100 + (y)/400)
#define IS_LEAP_YEAR(y)      	(((y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0))) ? 1 : 0)

u8   fnRTCWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);
u8   fnRTCRead(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);
void fnRTCtoLtime(RTC_QUQEUE_T *rtc, u32 *ltime);
void fnRTCtoTime(u32 ltime, RTC_QUQEUE_T *rtc);



#endif	//	_RTC_COMMON_H_
