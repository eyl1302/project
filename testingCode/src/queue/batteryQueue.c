#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "batteryQueue.h"

#define BATTERY_QUEUE	"BATTERY_QUEUE"

u8					btyPush = 0;	//	Push Index
u8					btyPop  = 0;	//	Pop Index
u8					btyTotal = 0;	//	How many data in the queue
BATTERY_QUQEUE_T	*batteryQueue = NULL;

/*
 * date: 2016/07/11
 * modify : hk
 * description: Battery Queue Initial
 */
void fnBatteryQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, BATTERY_QUEUE);

	//	Allocate Battery Queue
	batteryQueue = fnQueueInit(BATTERY_QUEUE_SIZE, sizeof(BATTERY_QUQEUE_T), QID_BATTERY);

	//hk_dbg("### hk_dbg, %s : batteryQueue:%p\n", __FUNCTION__, batteryQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: Battery Queue Push Data
 */
void fnBatteryQueuePush(BATTERY_QUQEUE_T battery)
{
	if(batteryQueue == NULL)
		return;

	if(btyTotal < BATTERY_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push Battery to Queue, total: %d, push to:%d!!\n", __FUNCTION__, btyTotal, btyPush);

		//	Push To Battery Queue
		fnQueuePush(&batteryQueue[btyPush], &battery, sizeof(BATTERY_QUQEUE_T));

		btyPush++;

		if(btyPush >= BATTERY_QUEUE_SIZE)
			btyPush = 0;

		btyTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : Battery Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: Battery Queue Pop Data
 */
u8   fnBatteryQueuePop(BATTERY_QUQEUE_T *battery)
{
	if(batteryQueue == NULL)
		return HK_FAIL;

	if(btyTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop Battery From Queue!!\n", __FUNCTION__);

		//	Pop From Battery Queue
		fnQueuePop(&batteryQueue[btyPop], battery, sizeof(BATTERY_QUQEUE_T));

		btyPop++;

		if(btyPop >= BATTERY_QUEUE_SIZE)
			btyPop = 0;

		btyTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : Battery Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: Free Battery Queue
 */
void fnBatteryQueueFree(void)
{
	if(batteryQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, BATTERY_QUEUE);

	//	Free Battery Queue
	fnQueueFree(batteryQueue);

	btyPush = 0;
	btyPop = 0;
	btyTotal = 0;

	return;
}
//-----------------------------------------------------
