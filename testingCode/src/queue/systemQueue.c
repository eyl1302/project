#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "systemQueue.h"

#define SYSTEM_QUEUE 	"SYSTEM_QUEUE"

u8				sysPush = 0;	//	Push Index
u8				sysPop  = 0;	//	Pop Index
u8				sysTotal = 0;	//	How many data in the queue
SYSTEM_QUQEUE_T	*systemQueue = NULL;

/*
 * date: 2016/07/19
 * modify : hk
 * description: System Queue Initial
 */
void fnSystemQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, SYSTEM_QUEUE);

	//	Allocate Battery Queue
	systemQueue = fnQueueInit(SYSTEM_QUEUE_SIZE, sizeof(SYSTEM_QUQEUE_T), QID_SYSTEM);

	//hk_dbg("### hk_dbg, %s : systemQueue:%p\n", __FUNCTION__, systemQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/07/19
 * modify : hk
 * description: System Queue Push Data
 */
void fnSystemQueuePush(SYSTEM_QUQEUE_T system)
{
	if(systemQueue == NULL)
		return;

	if(sysTotal < SYSTEM_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push Battery to Queue, total: %d, push to:%d!!\n", __FUNCTION__, sysTotal, sysPush);

		//	Push To Battery Queue
		fnQueuePush(&systemQueue[sysPush], &system, sizeof(SYSTEM_QUQEUE_T));

		sysPush++;

		if(sysPush >= SYSTEM_QUEUE_SIZE)
			sysPush = 0;

		sysTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : System Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: Battery Queue Pop Data
 */
u8 fnSystemQueuePop(SYSTEM_QUQEUE_T *system)
{
	if(systemQueue == NULL)
		return HK_FAIL;

	if(sysTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop Battery From Queue!!\n", __FUNCTION__);

		//	Pop From Battery Queue
		fnQueuePop(&systemQueue[sysPop], system, sizeof(SYSTEM_QUQEUE_T));

		sysPop++;

		if(sysPop >= SYSTEM_QUEUE_SIZE)
			sysPop = 0;

		sysTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : System Queue Empty!!!\n", __FUNCTION__);

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
void fnSystemQueueFree(void)
{
	if(systemQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, SYSTEM_QUEUE);

	//	Free Battery Queue
	fnQueueFree(systemQueue);

	sysPush = 0;
	sysPop = 0;
	sysTotal = 0;

	return;
}
//-----------------------------------------------------
