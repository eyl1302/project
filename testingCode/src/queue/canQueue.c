#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "canQueue.h"

#define CAN_QUEUE	"CAN_QUEUE"

u8				canPush = 0;		//	Push Index
u8				canPop  = 0;		//	Pop Index
u8				canTotal = 0;		//	How many data in the queue
CAN_QUQEUE_T	*canQueue = NULL;

/*
 * date: 2016/10/20
 * modify : hk
 * description: CAN Queue Initial
 */
void fnCANQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, CAN_QUEUE);

	//	Allocate CAN Queue
	canQueue = fnQueueInit(CAN_QUEUE_SIZE, sizeof(CAN_QUQEUE_T), QID_CAN);

	//hk_dbg("### hk_dbg, %s : canQueue:%p\n", __FUNCTION__, canQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: CAN Queue Push Data
 */
void fnCANQueuePush(CAN_QUQEUE_T can)
{
	if(canQueue == NULL)
		return;

	if(canTotal < CAN_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push CAN to Queue, total: %d, push to:%d!!\n", __FUNCTION__, canTotal, canPush);

		//	Push To CAN Queue
		fnQueuePush(&canQueue[canPush], &can, sizeof(CAN_QUQEUE_T));

		canPush++;

		if(canPush >= CAN_QUEUE_SIZE)
			canPush = 0;

		canTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : CAN Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: CAN Queue Pop Data
 */
u8 fnCANQueuePop(CAN_QUQEUE_T *can)
{
	if(canQueue == NULL)
		return HK_FAIL;

	if(canTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop CAN From Queue, total:%d, pop from :%d!!\n", __FUNCTION__, canTotal, canPop);

		//	Pop From CAN Queue
		fnQueuePop(&canQueue[canPop], can, sizeof(CAN_QUQEUE_T));

		canPop++;

		if(canPop >= CAN_QUEUE_SIZE)
			canPop = 0;

		canTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : CAN Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: Free CAN Queue
 */
void fnCANQueueFree(void)
{
	if(canQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, CAN_QUEUE);

	//	Free CAN Queue
	fnQueueFree(canQueue);

	canPush = 0;
	canPop = 0;
	canTotal = 0;

	return;
}
//-----------------------------------------------------
