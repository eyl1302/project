#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "emmcQueue.h"

#define EMMC_QUEUE	"EMMC_QUEUE"

u8				emmcPush = 0;	//	Push Index
u8				emmcPop  = 0;	//	Pop Index
u8				emmcTotal = 0;	//	How many data in the queue
EMMC_QUQEUE_T	*emmcQueue = NULL;

/*
 * date: 2016/09/23
 * modify : hk
 * description: EMMC Queue Initial
 */
void fnEMMCQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, EMMC_QUEUE);

	//	Allocate EMMC Queue
	emmcQueue = fnQueueInit(EMMC_QUEUE_SIZE, sizeof(EMMC_QUQEUE_T), QID_EMMC);

	//hk_dbg("### hk_dbg, %s : emmcQueue:%p\n", __FUNCTION__, emmcQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: EMMC Queue Push Data
 */
void fnEMMCQueuePush(EMMC_QUQEUE_T emmc)
{
	if(emmcQueue == NULL)
		return;

	if(emmcTotal < EMMC_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push EMMC to Queue, total: %d, push to:%d!!\n", __FUNCTION__, emmcTotal, emmcPush);

		//	Push To EMMC Queue
		fnQueuePush(&emmcQueue[emmcPush], &emmc, sizeof(EMMC_QUQEUE_T));

		emmcPush++;

		if(emmcPush >= EMMC_QUEUE_SIZE)
			emmcPush = 0;

		emmcTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : EMMC Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: EMMC Queue Pop Data
 */
u8 fnEMMCQueuePop(EMMC_QUQEUE_T *emmc)
{
	if(emmcQueue == NULL)
		return HK_FAIL;

	if(emmcTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop EMMC From Queue, total:%d, pop from :%d!!\n", __FUNCTION__, emmcTotal, emmcPop);

		//	Pop From EMMC Queue
		fnQueuePop(&emmcQueue[emmcPop], emmc, sizeof(EMMC_QUQEUE_T));

		emmcPop++;

		if(emmcPop >= EMMC_QUEUE_SIZE)
			emmcPop = 0;

		emmcTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : EMMC Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Free EMMC Queue
 */
void fnEMMCQueueFree(void)
{
	if(emmcQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, EMMC_QUEUE);

	//	Free EMMC Queue
	fnQueueFree(emmcQueue);

	emmcPush = 0;
	emmcPop = 0;
	emmcTotal = 0;

	return;
}
//-----------------------------------------------------
