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
#include <fcntl.h>
#include "i2c-dev.h"
#include "common.h"
#include "ds1374.h"

//	Function
u8 fnDS1374Get(RTC_QUQEUE_T *rtc);
u8 fnDS1374Set(RTC_QUQEUE_T *rtc);
u8 fnDS1374RegRead(u8 reg, u8 *data);
u8 fnDS1374RegWrite(u8 reg, u8 data);

static const unsigned char rtc_days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//	DS1374 RTC Access Function
RTC_FUNC_T ds1374 =
{
	fnDS1374Get,		//	Get RTC
	fnDS1374Set,		//	Set RTC
	fnDS1374RegRead,	//	Read Register
	fnDS1374RegWrite,	//	Write Register
};

/*
 * date: 2016/07/02
 * modify : hk
 * description: Get DS1374
 */
u8 fnDS1374Get(RTC_QUQEUE_T *rtc)
{
	s8	i;
	u8 	buf[DS1374_BUF_LEN];
	u32 ltime = 0;

	//	Read RTC
	if(fnRTCRead(DS1374_I2C_BUS, DS1374_I2C_ADDRESS, DR_TOD_0, buf, DS1374_BUF_LEN) == HK_SUCCESS)
	{
		//	Transfer to ltime
		for (i=DS1374_BUF_LEN-1;i>=0;i--)
		{
			ltime = (ltime << 8) | buf[i];

			//hk_dbg("### hk_dbg: %s, buf[%d]:0x%02x!\n", __FUNCTION__, i, buf[i]);
		}

		//	Convert from ltime
		fnRTCtoTime(ltime, rtc);

		//printf("\n  ### hk_dbg: %s, read rtc: %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, rtc->year + 1900, rtc->mon + 1, rtc->day, rtc->hour, rtc->min, rtc->sec, DOW(rtc->wday));

		//	Year
		rtc->year = rtc->year + 1900;

		//	Month
		rtc->mon = rtc->mon + 1;

		return HK_SUCCESS;
	}

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/02
 * modify : hk
 * description: Set DS1374
 */
u8 fnDS1374Set(RTC_QUQEUE_T *rtc)
{
	u8		i;
	u8  	buf[DS1374_BUF_LEN];
	u32 	ltime = 0;

	//	Conver to ltime
	fnRTCtoLtime(rtc, &ltime);

	for (i=0;i<DS1374_BUF_LEN;i++)
	{
		buf[i] = ltime & 0xff;
		
		ltime >>= 8;

		//hk_dbg("### hk_dbg: %s, buf[%d]:0x%02x!\n", __FUNCTION__, i, buf[i]);
	}

	//	Write RTC
	if(fnRTCWrite(DS1374_I2C_BUS, DS1374_I2C_ADDRESS, DR_TOD_0, buf, DS1374_BUF_LEN))
		return HK_SUCCESS;

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/06
 * modify : hk
 * description: Read Register
 */
u8 fnDS1374RegRead(u8 reg, u8 *data)
{
	u8 buf[DS1374_BUF_LEN];

	if(fnRTCRead(DS1374_I2C_BUS, DS1374_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
	{
		*data = buf[0];

		return HK_SUCCESS;
	}

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/06
 * modify : hk
 * description: Write Register
 */
u8 fnDS1374RegWrite(u8 reg, u8 data)
{
	u8 buf[DS1374_BUF_LEN];

	buf[0] = data;

	if(fnRTCWrite(DS1374_I2C_BUS, DS1374_I2C_ADDRESS, reg, buf, 1) == HK_SUCCESS)
		return HK_SUCCESS;
	else
		return HK_FAIL;
}
//------------------------------------------------
