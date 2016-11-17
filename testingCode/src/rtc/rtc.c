#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <linux/rtc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "common.h"
#include "system.h"
#include "rtc.h"
#include "rtcComm.h"
#include "ds1374.h"
#include "m41t82.h"
#include "rtcQueue.h"
#include "systemQueue.h"

//	Variable
RTC_DEVICE_HANDLE rtcHandle;

//	Function
void fnRTCThread(void *arg);
void fnRTCReplytoSystem(u8 reply);

/*
 * date: 2016/09/22
 * modify : hk
 * description: Create RTC Control Thread
 */
void fnRTCThreadCreate(void)
{
	pthread_t 		rtc_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	RTC Thread
	pthread_attr_setstacksize(&attr, RTC_STACK_SIZE);

	if(pthread_create(&rtc_thread, &attr, (void*)&fnRTCThread, NULL))
		printf("\n  ### hk_dbg, %s : rtc create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/22
 * modify : hk
 * description: Create RTC Control Thraed
 */
void fnRTCThread(void *arg)
{
	u8			 reply;
	RTC_QUQEUE_T rtc;

	//	Initial RTC Queue
	fnRTCQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnRTCQueuePop(&rtc) == HK_SUCCESS)
		{
			if(rtc.cmd == RQ_AUTOTEST)
			{
				//	Process RTC Event
				reply = fnRTCProcess(RC_START);

				//	Reply To System
				fnRTCReplytoSystem(reply);
			}
		}

		usleep(RTC_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Reply to System Module
 */
void fnRTCReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_RTC;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/06/21
 * modify : hk
 * description: RTC Initial
 */
void fnRTCInit(void)
{
	//	Create RTC Thread
	fnRTCThreadCreate();

	//	Point RTC Access Function to Device(ds1374, m41t82...)
	rtcHandle.rtcFunc = &ds1374;

	//	Initial RTC
	{
		time_t 		rawtime;
		struct tm 	*timeinfo;

		//	Update System Time Zone to UTC
		system("timedatectl set-timezone UTC");

		//	Get System Time
		time(&rawtime);
		timeinfo = localtime(&rawtime);

    	rtcHandle.rtc.year =  timeinfo->tm_year + 1900;  
    	rtcHandle.rtc.mon  =  timeinfo->tm_mon + 1;
    	rtcHandle.rtc.day  =  timeinfo->tm_mday;
    	rtcHandle.rtc.wday =  timeinfo->tm_wday;
    	rtcHandle.rtc.hour =  timeinfo->tm_hour;
    	rtcHandle.rtc.min  =  timeinfo->tm_min;
    	rtcHandle.rtc.sec  =  timeinfo->tm_sec;

    	printf("\n  ### hk_dbg: %s, write rtc : %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, rtcHandle.rtc.year, rtcHandle.rtc.mon, rtcHandle.rtc.day, rtcHandle.rtc.hour, rtcHandle.rtc.min, rtcHandle.rtc.sec, DOW(rtcHandle.rtc.wday));

    	//	Set RTC
		rtcHandle.rtcFunc->pRTCSet(&rtcHandle.rtc);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/06
 * modify : hk
 * description: RTC Start To Test
 */
u8 fnRTCStartTest(void)
{
	time_t 		rawtime;
	struct tm 	*timeinfo;

	//	Read RTC
	if(rtcHandle.rtcFunc->pRTCGet(&rtcHandle.rtc) == HK_FAIL)
		return HK_FAIL;

	printf("\n  ### hk_dbg: %s, read rtc: %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, rtcHandle.rtc.year, rtcHandle.rtc.mon, rtcHandle.rtc.day, rtcHandle.rtc.hour, rtcHandle.rtc.min, rtcHandle.rtc.sec, DOW(rtcHandle.rtc.wday));

	//	Update System Time Zone to UTC
	system("timedatectl set-timezone UTC");

	//	Get System Time
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	printf("\n  ### hk_dbg: %s, read UTC(NTP): %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, DOW(timeinfo->tm_wday));

	timeinfo->tm_year = timeinfo->tm_year + 1900;
	timeinfo->tm_mon = timeinfo->tm_mon + 1;

	//	Compare RTC & System Time
	if(rtcHandle.rtc.year == timeinfo->tm_year)
	{
		if(rtcHandle.rtc.mon == timeinfo->tm_mon)
		{
			if(rtcHandle.rtc.day == timeinfo->tm_mday)
			{
				if(rtcHandle.rtc.hour == timeinfo->tm_hour)
				{
					if(rtcHandle.rtc.min == timeinfo->tm_min)
					{
						u8 dev;

						if(rtcHandle.rtc.sec <= timeinfo->tm_sec)
							dev = timeinfo->tm_sec - rtcHandle.rtc.sec;
						else
							dev = rtcHandle.rtc.sec - timeinfo->tm_sec;

						if(dev <= 1)
						{
							printf("\n  ### hk_dbg, %s : RTC PASS!!!\n", __FUNCTION__);

							return HK_SUCCESS;
						}
					}
				}
			}
		}
	}

	printf("\n  ### hk_dbg, %s : RTC FAIL!!!\n", __FUNCTION__);

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/07
 * modify : hk
 * description: RTC Debug Set
 */
void fnRTCDebugSet(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ========================  Write RTC ========================\r\n\n");
    	printf("  %d. Snychronzie with UTC(NTP)									  \r\n\n", idx++);
    	printf("  %d. Manual Input        										  \r\n\n", idx++);
    	printf("  %d. Return													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
			case RS_NTP:
			{
				time_t 		rawtime;
				struct tm 	*timeinfo;

				//	Update System Time Zone to UTC
				system("timedatectl set-timezone UTC");

				//	Get System Time
				time(&rawtime);
				timeinfo = localtime(&rawtime);

    			rtcHandle.rtc.year =  timeinfo->tm_year + 1900;  
    			rtcHandle.rtc.mon  =  timeinfo->tm_mon + 1;
    			rtcHandle.rtc.day  =  timeinfo->tm_mday;
    			rtcHandle.rtc.wday =  timeinfo->tm_wday;
    			rtcHandle.rtc.hour =  timeinfo->tm_hour;
    			rtcHandle.rtc.min  =  timeinfo->tm_min;
    			rtcHandle.rtc.sec  =  timeinfo->tm_sec;

    			printf("\n  ### hk_dbg: %s, write rtc : %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, rtcHandle.rtc.year, rtcHandle.rtc.mon, rtcHandle.rtc.day, rtcHandle.rtc.hour, rtcHandle.rtc.min, rtcHandle.rtc.sec, DOW(rtcHandle.rtc.wday));

    			//	Set RTC
				rtcHandle.rtcFunc->pRTCSet(&rtcHandle.rtc);

				usleep(RTC_SLEEP_TIME);

				//	Get RTC
				rtcHandle.rtcFunc->pRTCGet(&rtcHandle.rtc);

				printf("\n  ### hk_dbg: %s, read rtc : %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, rtcHandle.rtc.year, rtcHandle.rtc.mon, rtcHandle.rtc.day, rtcHandle.rtc.hour, rtcHandle.rtc.min, rtcHandle.rtc.sec, DOW(rtcHandle.rtc.wday));
			}
			break;

			case RS_MANUAL:
			{
				char	str[64];

				printf("\n  Please Inpu Date & Time(Format:yyyymmddhhmmssww): ");

				scanf("\t %s", str);

				if(strlen(str) == RTC_TOTAL_LEN)	//	y(4)+m(2)+d(2)+h(2)+n(2)+s(2)+w(2)
				{
					rtcHandle.rtc.year = fnStrToINT(str, RTC_YEAR_STR, RTC_YEAR_LEN);
					rtcHandle.rtc.mon  = fnStrToINT(str, RTC_MONTH_STR, RTC_MONTH_LEN);
					rtcHandle.rtc.day  = fnStrToINT(str, RTC_DAY_STR, RTC_DAY_LEN);
					rtcHandle.rtc.hour = fnStrToINT(str, RTC_HOUR_STR, RTC_HOUR_LEN);
					rtcHandle.rtc.min  = fnStrToINT(str, RTC_MINUTE_STR, RTC_MINUTE_LEN);
					rtcHandle.rtc.sec  = fnStrToINT(str, RTC_SECOND_STR, RTC_SECOND_LEN);
					rtcHandle.rtc.wday = fnStrToINT(str, RTC_WDAY_STR, RTC_WDAY_LEN);

					hk_dbg("\n  ### hk_dbg: %s, write rtc : %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, rtcHandle.rtc.year, rtcHandle.rtc.mon, rtcHandle.rtc.day, rtcHandle.rtc.hour, rtcHandle.rtc.min, rtcHandle.rtc.sec, DOW(rtcHandle.rtc.wday));

					//	Set RTC
					rtcHandle.rtcFunc->pRTCSet(&rtcHandle.rtc);

					//	Get RTC
					rtcHandle.rtcFunc->pRTCGet(&rtcHandle.rtc);

					hk_dbg("\n  ### hk_dbg: %s, read rtc : %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, rtcHandle.rtc.year, rtcHandle.rtc.mon, rtcHandle.rtc.day, rtcHandle.rtc.hour, rtcHandle.rtc.min, rtcHandle.rtc.sec, DOW(rtcHandle.rtc.wday));
				}
				else
					printf("### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;
			
			case RS_RETURN:
				return;
			break;

			default:
				hk_dbg("### hk_dbg, %s : Unknow command:%d, Please Try again! \n", __FUNCTION__, cmd);
			break;
    	}

    	sleep(1);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/06
 * modify : hk
 * description: RTC Debug Mode
 */
u8 fnRTCDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  =======================  RTC Debug =======================\r\n\n");
    	printf("  %d. Read RTC   												  \r\n\n", idx++);
    	printf("  %d. Write RTC													  \r\n\n", idx++);
    	printf("  %d. Read Register												  \r\n\n", idx++);
    	printf("  %d. Write Register											  \r\n\n", idx++);
    	printf("  %d. Return													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
			case RD_GET:
				rtcHandle.rtcFunc->pRTCGet(&rtcHandle.rtc);

				hk_dbg("\n  ### hk_dbg: %s, read rtc : %d-%d-%d %d:%d:%d(%s)\n", __FUNCTION__, rtcHandle.rtc.year, rtcHandle.rtc.mon, rtcHandle.rtc.day, rtcHandle.rtc.hour, rtcHandle.rtc.min, rtcHandle.rtc.sec, DOW(rtcHandle.rtc.wday));
			break;

			case RD_SET:
				fnRTCDebugSet();
			break;
			
			case RD_REG_READ:
			{
				u8 reg;
				u8 data;

				char	str[64];

				printf("\n  Please Inpu Register (Format:rr): ");

				scanf("\t %s", str);

				if(strlen(str) == RTC_REG_LEN)	//	a(2)
				{
					reg = fnStrToHEX(str, RTC_REG_STR,  RTC_REG_LEN);

					rtcHandle.rtcFunc->pRegRead(reg, &data);

					printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;
			
			case RD_REG_WRITE:
			{
				u8 reg;
				u8 data;

				char	str[64];

				printf("\n  Please Inpu Register & Data(Format:rrdd): ");

				scanf("\t %s", str);

				if(strlen(str) == (RTC_REG_LEN + RTC_DATA_LEN))	//	r(2)+d(2)
				{
					reg  = fnStrToHEX(str, RTC_REG_STR,  RTC_REG_LEN);
					data = fnStrToHEX(str, RTC_DATA_STR,  RTC_DATA_LEN);

					printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, reg, data);

					rtcHandle.rtcFunc->pRegWrite(reg, data);

					rtcHandle.rtcFunc->pRegRead(reg, &data);

					printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;
			
			case RD_RETURN:
				return HK_NA;
			break;

			default:
				hk_dbg("\n  ### hk_dbg, %s : Unknow command:%d, Please Try again! \n", __FUNCTION__, cmd);
			break;
    	}

    	sleep(1);
	}

	return HK_NA;
}
//------------------------------------------------

/*
 * date: 2016/06/21
 * modify : hk
 * description: Process RTC Event
 */
u8 fnRTCProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case RC_START:
			result = fnRTCStartTest();
		break;

		case RC_DBG:
			result = fnRTCDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow rtc command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
