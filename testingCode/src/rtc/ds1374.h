#ifndef _DS1374_H_
#define _DS1374_H_

#include "rtcQueue.h"
#include "rtc.h"
#include "rtcComm.h"

#define	DS1374_I2C_BUS			"/dev/i2c-0"		//	DS1374 I2C Bus
#define DS1374_I2C_ADDRESS		0x30				//	DS1374 Slave Address

//	DS1374 Register
#define DR_TOD_0				0x00				//	Time-of-Day Counter Byte 0
#define DR_TOD_1                0x01    			//	Time-of-Day Counter Byte 1
#define DR_TOD_2                0x02    			//	Time-of-Day Counter Byte 2
#define DR_TOD_3                0x03    			//	Time-of-Day Counter Byte 3
#define DR_WD_0					0x04    			//	Watchdog/Alarm Counter Byte 0
#define DR_WD_1                	0x05    			//	Watchdog/Alarm Counter Byte 1
#define DR_WD_2                	0x06    			//	Watchdog/Alarm Counter Byte 2
#define DR_CONTORL				0x07    			//	Control Register
#define DR_STATUS				0x08    			//	Status Register
#define DR_TC					0x09    			//	Trickle Charger

#define LEAPS_THRU_END_OF(y) 	((y)/4 - (y)/100 + (y)/400)
#define IS_LEAP_YEAR(y)      	(((y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0))) ? 1 : 0)

#define DS1374_BUF_LEN			4

extern RTC_FUNC_T ds1374;

#endif	//	_DS1374_H_
