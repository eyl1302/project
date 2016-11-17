#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "gsensorQueue.h"

#define GSENSOR_QUEUE	"GSENSOR_QUEUE"

u8					gsensorPush = 0;	//	Push Index
u8					gsensorPop  = 0;	//	Pop Index
u8					gsensorTotal = 0;	//	How many data in the queue
GSENSOR_QUQEUE_T	*gsensorQueue = NULL;

/*
 * date: 2016/09/23
 * modify : hk
 * description: GSensor Queue Initial
 */
void fnGSensorQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, GSENSOR_QUEUE);

	//	Allocate GSensor Queue
	gsensorQueue = fnQueueInit(GSENSOR_QUEUE_SIZE, sizeof(GSENSOR_QUQEUE_T), QID_GSENSOR);

	//hk_dbg("### hk_dbg, %s : gsensorQueue:%p\n", __FUNCTION__, gsensorQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: GSensor Queue Push Data
 */
void fnGSensorQueuePush(GSENSOR_QUQEUE_T gsensor)
{
	if(gsensorQueue == NULL)
		return;

	if(gsensorTotal < GSENSOR_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push GSensor to Queue, total: %d, push to:%d!!\n", __FUNCTION__, gsensorTotal, gsensorPush);

		//	Push To GSensor Queue
		fnQueuePush(&gsensorQueue[gsensorPush], &gsensor, sizeof(GSENSOR_QUQEUE_T));

		gsensorPush++;

		if(gsensorPush >= GSENSOR_QUEUE_SIZE)
			gsensorPush = 0;

		gsensorTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : GSensor Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: GSensor Queue Pop Data
 */
u8   fnGSensorQueuePop(GSENSOR_QUQEUE_T *gsensor)
{
	if(gsensorQueue == NULL)
		return HK_FAIL;

	if(gsensorTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop GSensor From Queue!!\n", __FUNCTION__);

		//	Pop From Battery Queue
		fnQueuePop(&gsensorQueue[gsensorPop], gsensor, sizeof(GSENSOR_QUQEUE_T));

		gsensorPop++;

		if(gsensorPop >= GSENSOR_QUEUE_SIZE)
			gsensorPop = 0;

		gsensorTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : GSensor Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Free GSensor Queue
 */
void fnGSensorQueueFree(void)
{
	if(gsensorQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, GSENSOR_QUEUE);

	//	Free GSensor Queue
	fnQueueFree(gsensorQueue);

	gsensorPush = 0;
	gsensorPop = 0;
	gsensorTotal = 0;

	return;
}
//-----------------------------------------------------
