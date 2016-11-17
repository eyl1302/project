#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <linux/rtc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "i2c-dev.h"
#include "common.h"
#include "m41t82.h"

//	Function
u8 fnM41T82Get(RTC_QUQEUE_T *rtc);
u8 fnM41T82Set(RTC_QUQEUE_T *rtc);
u8 fnM41T82RegRead(u8 reg, u8 *data);
u8 fnM41T82RegWrite(u8 reg, u8 data);

//	M41T82 RTC Access Function
RTC_FUNC_T m41t82 =
{
	fnM41T82Get,		//	Get RTC
	fnM41T82Set,		//	Set RTC
	fnM41T82RegRead,	//	Read Register
	fnM41T82RegWrite,   //	Write Register    
};

/*
 * date: 2016/07/02
 * modify : hk
 * description: Get M41T82
 */
u8 fnM41T82Get(RTC_QUQEUE_T *rtc)
{
	u8 	buf[M41T82_BUF_LEN];

	//	Read RTC
	if(fnRTCRead(M41T82_I2C_BUS, M41T82_I2C_ADDRESS, MR_SEC, buf, M41T82_BUF_LEN) == HK_SUCCESS)
	{
		//printf("\n  ### hk_dbg, %s  get rtc : %02x/%02x/%02x - %02x:%02x:%02x, (%s)\n", __FUNCTION__, 0x2000+buf[6], buf[5], buf[4], buf[2], buf[1], buf[0], DOW(buf[3]));

		//	Year
		rtc->year = BCD2DECIMAL(buf[6]) + 2000;
		rtc->mon  = BCD2DECIMAL(buf[5]);
		rtc->day  = BCD2DECIMAL(buf[4]);
		rtc->wday = BCD2DECIMAL(buf[3]);
		rtc->hour = BCD2DECIMAL(buf[2]);
		rtc->min  = BCD2DECIMAL(buf[1]);
		rtc->sec  = BCD2DECIMAL(buf[0]);

		return HK_SUCCESS;
	}

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/02
 * modify : hk
 * description: Set M41T82
 */
u8 fnM41T82Set(RTC_QUQEUE_T *rtc)
{
	u8 	buf[M41T82_BUF_LEN];

	rtc->year = rtc->year - 2000;

	buf[6] = DECIMAL2BCD(rtc->year);
	buf[5] = DECIMAL2BCD(rtc->mon);
	buf[4] = DECIMAL2BCD(rtc->day);
	buf[3] = DECIMAL2BCD(rtc->wday);
	buf[2] = DECIMAL2BCD(rtc->hour);
	buf[1] = DECIMAL2BCD(rtc->min);
	buf[0] = DECIMAL2BCD(rtc->sec);

	//printf("\n  ### hk_dbg, %s  set rtc : %02x/%02x/%02x - %02x:%02x:%02x, (%s)\n", __FUNCTION__, 0x2000+buf[6], buf[5], buf[4], buf[2], buf[1], buf[0], DOW(buf[3]));

	//	Set Time
	fnRTCWrite(M41T82_I2C_BUS, M41T82_I2C_ADDRESS, MR_SEC, buf, M41T82_BUF_LEN);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/07
 * modify : hk
 * description: Read Register
 */
u8 fnM41T82RegRead(u8 reg, u8 *data)
{
	u8 buf[M41T82_BUF_LEN];

	if(fnRTCRead(M41T82_I2C_BUS, M41T82_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
	{
		*data = buf[0];

		return HK_SUCCESS;
	}

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/07
 * modify : hk
 * description: Write Register
 */
u8 fnM41T82RegWrite(u8 reg, u8 data)
{
	u8 buf[M41T82_BUF_LEN];

	buf[0] = data;

	if(fnRTCWrite(M41T82_I2C_BUS, M41T82_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
		return HK_SUCCESS;
	else
		return HK_FAIL;
}
//------------------------------------------------
