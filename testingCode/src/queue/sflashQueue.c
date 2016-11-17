#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "sflashQueue.h"

#define SFLASH_QUEUE	"SFLASH_QUEUE"

u8				sflashPush = 0;	//	Push Index
u8				sflashPop  = 0;	//	Pop Index
u8				sflashTotal = 0;	//	How many data in the queue
SFLASH_QUQEUE_T	*sflashQueue = NULL;

/*
 * date: 2016/09/23
 * modify : hk
 * description: SFlash Queue Initial
 */
void fnSFlashQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, SFLASH_QUEUE);

	//	Allocate SFlash Queue
	sflashQueue = fnQueueInit(SFLASH_QUEUE_SIZE, sizeof(SFLASH_QUQEUE_T), QID_SFLASH);

	//hk_dbg("### hk_dbg, %s : sflashQueue:%p\n", __FUNCTION__, sflashQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: SFlash Queue Push Data
 */
void fnSFlashQueuePush(SFLASH_QUQEUE_T sflash)
{
	if(sflashQueue == NULL)
		return;

	if(sflashTotal < SFLASH_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push SFlash to Queue, total: %d, push to:%d!!\n", __FUNCTION__, sflashTotal, sflashPush);

		//	Push To SFlash Queue
		fnQueuePush(&sflashQueue[sflashPush], &sflash, sizeof(SFLASH_QUQEUE_T));

		sflashPush++;

		if(sflashPush >= SFLASH_QUEUE_SIZE)
			sflashPush = 0;

		sflashTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : SFlash Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: SFlash Queue Pop Data
 */
u8 fnSFlashQueuePop(SFLASH_QUQEUE_T *sflash)
{
	if(sflashQueue == NULL)
		return HK_FAIL;

	if(sflashTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop SFlash From Queue, total:%d, pop from :%d!!\n", __FUNCTION__, sflashTotal, sflashPop);

		//	Pop From SFlash Queue
		fnQueuePop(&sflashQueue[sflashPop], sflash, sizeof(SFLASH_QUQEUE_T));

		sflashPop++;

		if(sflashPop >= SFLASH_QUEUE_SIZE)
			sflashPop = 0;

		sflashTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : SFlash Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Free SFlash Queue
 */
void fnSFlashQueueFree(void)
{
	if(sflashQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, SFLASH_QUEUE);

	//	Free SFlash Queue
	fnQueueFree(sflashQueue);

	sflashPush = 0;
	sflashPop = 0;
	sflashTotal = 0;

	return;
}
//-----------------------------------------------------
