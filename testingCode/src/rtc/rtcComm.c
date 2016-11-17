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
#include "rtcComm.h"

static const unsigned char rtc_days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*
 * date: 2016/05/04
 * modify : hk
 * description: RTC Read Function
 */
u8 fnRTCRead(char* bus, u8 addr, u8 reg, u8 *buf, u8 len)
{
	s32 fd;

	//	Open RTC I2C Bus
	if((fd = open(bus, O_RDWR)) < 0)
   	{
   		perror("fnRTCRead");

      	return HK_FAIL;
   	}

	if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Could not set address to 0x%02x:!!  ###\n ", __FUNCTION__, __LINE__, addr);

   		//	Close RTC
		close(fd);

		return HK_FAIL;;
	}

	if(i2c_smbus_read_i2c_block_data(fd, reg, len, buf) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Unable to read from RTC!!  ###\n ", __FUNCTION__, __LINE__);

   		//	Close RTC
		close(fd);

		return HK_FAIL;
	}

   	//	Close RTC
	close(fd);

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/05/04
 * modify : hk
 * description: RTC Write Function
 */
u8 fnRTCWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len)
{
	s32 fd;

	//	Open RTC I2C Bus
	if((fd = open(bus, O_RDWR)) < 0)
   	{
      	hk_dbg("\n  ### hk_dbg: %s, line:%d, Failed to open i2c bus!!  ###\n ", __FUNCTION__, __LINE__);

      	return HK_FAIL;
   	}

	//	I2C Slave Address
   	if (ioctl(fd, I2C_SLAVE_FORCE, addr) < 0)
	{
		hk_dbg("\n  ### hk_dbg: %s, line:%d, Could not set address to 0x%02x:!!  ###\n ", __FUNCTION__, __LINE__, addr);

   		//	Close RTC
		close(fd);

		return HK_FAIL;;
	}

	if(i2c_smbus_write_i2c_block_data(fd, reg, len, buf) < 0)
	{
   		hk_dbg("\n  ### hk_dbg: %s, line:%d, Unable to write to RTC!!  ###\n ", __FUNCTION__, __LINE__);

   		//	Close RTC
		close(fd);

   		return HK_FAIL;
   	}

   	//	Close RTC
	close(fd);

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/07/02
 * modify : hk
 * description: The number of days in the month.
 */
int fnRTCMonthDays(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month] + (IS_LEAP_YEAR(year) && month == 1);
}
//-----------------------------------------------------

/*
 * date: date: 2016/07/02
 * modify : hk
 * description: Time to RTC
 */
void fnRTCtoLtime(RTC_QUQEUE_T *rtc, u32 *ltime)
{
	unsigned int mon;
	unsigned int year;

	mon = rtc->mon;
	year = rtc->year;

	//	1..12 -> 11,12,1..10
	if (0 >= (int) (mon -= 2))
	{
		mon += 12;												//	Puts Feb last since it has leap day
		year -= 1;
	}

	*ltime = ((((unsigned long)
		  (year/4 - year/100 + year/400 + 367*mon/12 + rtc->day) + year*365 - 719499
        )*24 + rtc->hour											//	now have hours
	  )*60 + rtc->min											// 	now have minutes
	)*60 + rtc->sec; 											// 	finally seconds

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/07/02
 * modify : hk
 * description: RTC to Time
 */
void fnRTCtoTime(u32 ltime, RTC_QUQEUE_T *rtc)
{
	unsigned int month, year;
	int days;

	days = ltime / 86400;
	ltime -= (unsigned int) days * 86400;

	/* day of the week, 1970-01-01 was a Thursday */
	rtc->wday = (days + 4) % 7; //	1~7 : Monday to Sunday

	year = 1970 + days / 365;
	days -= (year - 1970) * 365
		+ LEAPS_THRU_END_OF(year - 1)
		- LEAPS_THRU_END_OF(1970 - 1);
	if (days < 0) {
		year -= 1;
		days += 365 + IS_LEAP_YEAR(year);
	}
	rtc->year = year - 1900;
	rtc->day = days + 1;

	for (month = 0; month < 11; month++)
	{
		s32 newdays;

		newdays = days - fnRTCMonthDays(month, year);

		if (newdays < 0)
			break;
		days = newdays;
	}

	rtc->mon = month;
	rtc->day = days + 1;

	rtc->hour = ltime / 3600;
	ltime -= rtc->hour * 3600;
	rtc->min = ltime / 60;
	rtc->sec = ltime - rtc->min * 60;

	return;
}
//-----------------------------------------------------
