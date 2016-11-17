#ifndef _RTC_H_
#define _RTC_H_

#include "common.h"
#include "rtcQueue.h"

#define DELAY_MILLI_SECOND		1000
#define RTC_DELAY_SET			(300 * DELAY_MILLI_SECOND)
#define RTC_DELAY_GET			(700 * DELAY_MILLI_SECOND)

#define RTC_BUF_LEN				4

//	I2C Access Mode
//#define I2C_ACS_BYTE											//	Byte
#define I2C_ACE_BLOCK											//	Block(More than 1 byte)

//	Stack Size
#define	RTC_STACK_SIZE			0x100000

//	Sleep
#define RTC_MILLISEC_BASE		(1000)
#define RTC_MICROSEC_BASE		(1000000)
#define	RTC_SLEEP_TIME			(10 * RTC_MILLISEC_BASE)		//	10ms


/********** RTC Manual Format : yyyymmdd-hhmmss-ww **********/
//	RTC Length
#define RTC_YEAR_LEN			4								//	Year
#define RTC_MONTH_LEN			2								//	Month
#define RTC_DAY_LEN				2                               //	Day
#define RTC_HOUR_LEN			2                               //	Hour
#define RTC_MINUTE_LEN			2                               //	Minute
#define RTC_SECOND_LEN			2                               //	Second
#define RTC_WDAY_LEN			2                               //	Day of Week
#define RTC_TOTAL_LEN			(RTC_YEAR_LEN + RTC_MONTH_LEN + RTC_DAY_LEN + RTC_HOUR_LEN + RTC_MINUTE_LEN + RTC_SECOND_LEN + RTC_WDAY_LEN)

//	RTC Start Address
#define	RTC_YEAR_STR			0
#define RTC_MONTH_STR			(RTC_YEAR_STR + RTC_YEAR_LEN)
#define RTC_DAY_STR				(RTC_MONTH_STR + RTC_MONTH_LEN)
#define RTC_HOUR_STR			(RTC_DAY_STR + RTC_DAY_LEN)
#define RTC_MINUTE_STR			(RTC_HOUR_STR + RTC_HOUR_LEN)
#define RTC_SECOND_STR			(RTC_MINUTE_STR + RTC_MINUTE_LEN)
#define RTC_WDAY_STR			(RTC_SECOND_STR + RTC_SECOND_LEN)

/********** End of RTC Manual Format **********/

/********** RTC Register Format : aadd **********/
//	RTC Reigster Length
#define	RTC_REG_LEN				2
#define RTC_DATA_LEN			2

//	RTC Register Start Address
#define	RTC_REG_STR				0
#define	RTC_DATA_STR			(RTC_REG_STR + RTC_REG_LEN)

/********** End of RTC Register Format **********/

//	Day of Week
#define DOW(d)	\
		((d == 0)?"Sun.":\
		((d == 1)?"Mon.":\
		((d == 2)?"Tue.":\
		((d == 3)?"Wed.":\
		((d == 4)?"Thu.":\
		((d == 5)?"Fri.":\
		((d == 6)?"Sat.":\
		((d == 7)?"Sun.": "unknow"))))))))


//	RTC Reigster Access Command
typedef enum
{
	RRA_READ = 0,												//	Register Read
	RRA_WRITE,													//	Register Write

}	RTC_REG_ACCESS_T;

//	RTC Access Function Pointer
typedef struct
{
	u8 (*pRTCGet)		(RTC_QUQEUE_T *rtc);					//	Get RTC
	u8 (*pRTCSet)		(RTC_QUQEUE_T *rtc);					//	Set RTC
	u8 (*pRegRead)		(u8 reg, u8 *data);						//	Read Register
	u8 (*pRegWrite)		(u8 reg, u8 data);						//	Write Register

}	RTC_FUNC_T;

//	RTC Device Handle
typedef struct
{
	RTC_FUNC_T 		*rtcFunc;									//	RTC Access Function
	RTC_QUQEUE_T	rtc;										//	RTC Queue

}	RTC_DEVICE_HANDLE;

//	RTC Command
typedef enum
{
	RC_START = 0x01,
	RC_DBG,
	RC_RETURN,
	
}	RTC_COMMAND_T;

//	RTC Debug Command
typedef enum
{
	RD_GET = 0x01,
	RD_SET,
	RD_REG_READ,
	RD_REG_WRITE,
	RD_RETURN,

}	RTC_DEBUG_COMMAND_T;

//	RTC Debug Set Command
typedef enum
{
	RS_NTP = 0x01,
	RS_MANUAL,
	RS_RETURN,

}	RTC_DEBUG_SET_COMMAND_T;

//	Function
void fnRTCInit(void);
u8 	 fnRTCProcess(u8 cmd);

#endif	//	_RTC_H_
