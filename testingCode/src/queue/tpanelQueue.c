#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "tpanelQueue.h"

#define TPANEL_QUEUE	"TPANEL_QUEUE"

u8				tpanelPush = 0;		//	Push Index
u8				tpanelPop  = 0;		//	Pop Index
u8				tpanelTotal = 0;	//	How many data in the queue
TPANEL_QUQEUE_T	*tpanelQueue = NULL;

/*
 * date: 2016/10/04
 * modify : hk
 * description: Touch Panel Queue Initial
 */
void fnTPanelQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, TPANEL_QUEUE);

	//	Allocate Touch Panel Queue
	tpanelQueue = fnQueueInit(TPANEL_QUEUE_SIZE, sizeof(TPANEL_QUQEUE_T), QID_TPANEL);

	//hk_dbg("### hk_dbg, %s : tpanelQueue:%p\n", __FUNCTION__, tpanelQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/10/04
 * modify : hk
 * description: Touch Panel Queue Push Data
 */
void fnTPanelQueuePush(TPANEL_QUQEUE_T tpanel)
{
	if(tpanelQueue == NULL)
		return;

	if(tpanelTotal < TPANEL_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push Touch Panel to Queue, total: %d, push to:%d!!\n", __FUNCTION__, tpanelTotal, tpanelPush);

		//	Push To Touch Panel Queue
		fnQueuePush(&tpanelQueue[tpanelPush], &tpanel, sizeof(TPANEL_QUQEUE_T));

		tpanelPush++;

		if(tpanelPush >= TPANEL_QUEUE_SIZE)
			tpanelPush = 0;

		tpanelTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : Touch Panel Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/10/04
 * modify : hk
 * description: Touch Panel Queue Pop Data
 */
u8 fnTPanelQueuePop(TPANEL_QUQEUE_T *tpanel)
{
	if(tpanelQueue == NULL)
		return HK_FAIL;

	if(tpanelTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop Touch Panel From Queue, total:%d, pop from :%d!!\n", __FUNCTION__, tpanelTotal, tpanelPop);

		//	Pop From Touch Panel Queue
		fnQueuePop(&tpanelQueue[tpanelPop], tpanel, sizeof(TPANEL_QUQEUE_T));

		tpanelPop++;

		if(tpanelPop >= TPANEL_QUEUE_SIZE)
			tpanelPop = 0;

		tpanelTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : Touch Panel Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/10/04
 * modify : hk
 * description: Free Touch Panel Queue
 */
void fnTPanelQueueFree(void)
{
	if(tpanelQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, TPANEL_QUEUE);

	//	Free Touch Pane Queue
	fnQueueFree(tpanelQueue);

	tpanelPush = 0;
	tpanelPop = 0;
	tpanelTotal = 0;

	return;
}
//-----------------------------------------------------
