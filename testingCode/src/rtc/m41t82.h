#ifndef _M41T82_H_
#define _M41T82_H_

#include "system.h"
#include "rtcQueue.h"
#include "rtc.h"
#include "rtcComm.h"

#define	M41T82_I2C_BUS			"/dev/i2c-2"		//	M41T82 I2C Bus
#define M41T82_I2C_ADDRESS		0x68				//	M41T82 Slave Address

#define DELAY_MILLI_SECOND		1000
#define M41T82_DELAY_SET		(10*DELAY_MILLI_SECOND)

#define M41T82_BUF_LEN			7

//	M41T82 Register Map
#define	MR_SUB_SEC				0x00				//	Sub-Scond
#define	MR_SEC					0x01				//	Scond
#define	MR_MIN					0x02				//	Minute
#define	MR_HOUR					0x03				//	Hour
#define	MR_DAY					0x04				//	Day of week
#define	MR_DATE					0x05				//	Date
#define	MR_MONTH				0x06				//	Month
#define MR_YEAR					0x07				//	Year
#define MR_AL1_HOUR				0x0c				//	Alarm 1 Hour
#define MR_AC					0x12				//	Analog Calibration

#define AL1_HT					0x40				//	HT Bit

//#define DECIMAL2BCD(dec)		(((dec / 10) << 4) + (dec % 10))

//	Analog Calibratoin Command
typedef enum
{
	ACC_NORMAL = 0x00,
	ACC_SLOW,
	ACC_FAST,

}	AC_COMMAND_T;

//	Analog Calibration Value
typedef enum
{
	AC_ZERO = 0x00,		//	0 PPM
	AC_SLOW = 0x27,		//	-15 PPM(slow)
	AC_FAST = 0xC8,     //	15 PPM(fast)

}	ANALOG_CALIBRATION_T;

//	Analog Calibration
#define OSCILLATOR_FREQ(ac) \
		((ac == AC_ZERO)?"0 ppm" :\
		((ac == AC_SLOW)?"-15 ppm(slow)":\
		((ac == AC_FAST)?"+95 ppm(fast)":"unknow")))

extern RTC_FUNC_T m41t82;

#endif	//	_M41T82_H_
