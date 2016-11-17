#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "sdramQueue.h"

#define SDRAM_QUEUE	"SDRAM_QUEUE"

u8				sdramPush = 0;	//	Push Index
u8				sdramPop  = 0;	//	Pop Index
u8				sdramTotal = 0;	//	How many data in the queue
SDRAM_QUQEUE_T	*sdramQueue = NULL;

/*
 * date: 2016/09/23
 * modify : hk
 * description: SDRAM Queue Initial
 */
void fnSDRAMQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, SDRAM_QUEUE);

	//	Allocate Battery Queue
	sdramQueue = fnQueueInit(SDRAM_QUEUE_SIZE, sizeof(SDRAM_QUQEUE_T), QID_SDRAM);

	//hk_dbg("### hk_dbg, %s : sdramQueue:%p\n", __FUNCTION__, sdramQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: SDRAM Queue Push Data
 */
void fnSDRAMQueuePush(SDRAM_QUQEUE_T sdram)
{
	if(sdramQueue == NULL)
		return;

	if(sdramTotal < SDRAM_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push SDRAM to Queue, total: %d, push to:%d!!\n", __FUNCTION__, sdramTotal, sdramPush);

		//	Push To GSensor Queue
		fnQueuePush(&sdramQueue[sdramPush], &sdram, sizeof(SDRAM_QUQEUE_T));

		sdramPush++;

		if(sdramPush >= SDRAM_QUEUE_SIZE)
			sdramPush = 0;

		sdramTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : SDRAM Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: SDRAM Queue Pop Data
 */
u8   fnSDRAMQueuePop(SDRAM_QUQEUE_T *sdram)
{
	if(sdramQueue == NULL)
		return HK_FAIL;

	if(sdramTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop SDRAM From Queue!!\n", __FUNCTION__);

		//	Pop From SDRAM Queue
		fnQueuePop(&sdramQueue[sdramPop], sdram, sizeof(SDRAM_QUQEUE_T));

		sdramPop++;

		if(sdramPop >= SDRAM_QUEUE_SIZE)
			sdramPop = 0;

		sdramTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : SDRAM Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Free SDRAM Queue
 */
void fnSDRAMQueueFree(void)
{
	if(sdramQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, SDRAM_QUEUE);

	//	Free SDRAM Queue
	fnQueueFree(sdramQueue);

	sdramPush = 0;
	sdramPop = 0;
	sdramTotal = 0;

	return;
}
//-----------------------------------------------------
