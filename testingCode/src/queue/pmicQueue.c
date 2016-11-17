#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "pmicQueue.h"

#define PMIC_QUEUE	"PMIC_QUEUE"

u8				pmicPush = 0;		//	Push Index
u8				pmicPop  = 0;		//	Pop Index
u8				pmicTotal = 0;		//	How many data in the queue
PMIC_QUQEUE_T	*pmicQueue = NULL;

/*
 * date: 2016/10/05
 * modify : hk
 * description: PMIC Queue Initial
 */
void fnPMICQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, PMIC_QUEUE);

	//	Allocate PMIC Queue
	pmicQueue = fnQueueInit(PMIC_QUEUE_SIZE, sizeof(PMIC_QUQEUE_T), QID_PMIC);

	//hk_dbg("### hk_dbg, %s : pmicQueue:%p\n", __FUNCTION__, pmicQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: PMIC Queue Push Data
 */
void fnPMICQueuePush(PMIC_QUQEUE_T pmic)
{
	if(pmicQueue == NULL)
		return;

	if(pmicTotal < PMIC_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push PMIC to Queue, total: %d, push to:%d!!\n", __FUNCTION__, pmicTotal, pmicPush);

		//	Push To PMIC Queue
		fnQueuePush(&pmicQueue[pmicPush], &pmic, sizeof(PMIC_QUQEUE_T));

		pmicPush++;

		if(pmicPush >= PMIC_QUEUE_SIZE)
			pmicPush = 0;

		pmicTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : PMIC Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: PMIC Queue Pop Data
 */
u8 fnPMICQueuePop(PMIC_QUQEUE_T *pmic)
{
	if(pmicQueue == NULL)
		return HK_FAIL;

	if(pmicTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop PMIC From Queue, total:%d, pop from :%d!!\n", __FUNCTION__, pmicTotal, pmicPop);

		//	Pop From PMIC Queue
		fnQueuePop(&pmicQueue[pmicPop], pmic, sizeof(PMIC_QUQEUE_T));

		pmicPop++;

		if(pmicPop >= PMIC_QUEUE_SIZE)
			pmicPop = 0;

		pmicTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : PMIC Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: Free PMIC Queue
 */
void fnPMICQueueFree(void)
{
	if(pmicQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, PMIC_QUEUE);

	//	Free PMIC Queue
	fnQueueFree(pmicQueue);

	pmicPush = 0;
	pmicPop = 0;
	pmicTotal = 0;

	return;
}
//-----------------------------------------------------
