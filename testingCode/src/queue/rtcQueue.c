#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "rtcQueue.h"

#define RTC_QUEUE	"RTC_QUEUE"

u8				rtcPush = 0;	//	Push Index
u8				rtcPop  = 0;	//	Pop Index
u8				rtcTotal = 0;	//	How many data in the queue
RTC_QUQEUE_T	*rtcQueue = NULL;

/*
 * date: 2016/06/21
 * modify : hk
 * description: RTC Queue Initial
 */
void fnRTCQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, RTC_QUEUE);

	//	Allocate RTC Queue
	rtcQueue = fnQueueInit(RTC_QUEUE_SIZE, sizeof(RTC_QUQEUE_T), QID_RTC);

	//hk_dbg("### hk_dbg, %s : rtcQueue:%p\n", __FUNCTION__, rtcQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/06/21
 * modify : hk
 * description: RTC Queue Push Data
 */
void fnRTCQueuePush(RTC_QUQEUE_T rtc)
{
	if(rtcQueue == NULL)
		return;

	if(rtcTotal < RTC_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push RTC to Queue, total: %d, push to:%d!!\n", __FUNCTION__, rtcTotal, rtcPush);

		//	Push To RTC Queue
		fnQueuePush(&rtcQueue[rtcPush], &rtc, sizeof(RTC_QUQEUE_T));

		rtcPush++;

		if(rtcPush >= RTC_QUEUE_SIZE)
			rtcPush = 0;

		rtcTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : RTC Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/06/21
 * modify : hk
 * description: RTC Queue Pop Data
 */
u8 fnRTCQueuePop(RTC_QUQEUE_T *rtc)
{
	if(rtcQueue == NULL)
		return HK_FAIL;

	if(rtcTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop RTC From Queue, total:%d, pop from :%d!!\n", __FUNCTION__, rtcTotal, rtcPop);

		//	Pop From RTC Queue
		fnQueuePop(&rtcQueue[rtcPop], rtc, sizeof(RTC_QUQEUE_T));

		rtcPop++;

		if(rtcPop >= RTC_QUEUE_SIZE)
			rtcPop = 0;

		rtcTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : RTC Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/06/30
 * modify : hk
 * description: Free RTC Queue
 */
void fnRTCQueueFree(void)
{
	if(rtcQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, RTC_QUEUE);

	//	Free RTC Queue
	fnQueueFree(rtcQueue);

	rtcPush = 0;
	rtcPop = 0;
	rtcTotal = 0;

	return;
}
//-----------------------------------------------------
