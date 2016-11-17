#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"


/*
 * date: 2016/06/21
 * modify : hk
 * description: Queue Initial
 */
void* fnQueueInit(u8 len, u8 size, u8 id)
{
	void* queue;

	//	Allocate Memory
	queue = malloc(len * size);

	if(queue == NULL)
	{
		hk_dbg("### hk_dbg, %s : Create %s Queue Fail!!\n", __FUNCTION__, QUEUE_STR(id));

		return NULL;
	}

    //hk_dbg("### hk_dbg, %s : rtcQueue:%p\n", __FUNCTION__, queue);

	return queue;
}
//-----------------------------------------------------

/*
 * date: 2016/06/21
 * modify : hk
 * description: Queue Push Data
 */
void fnQueuePush(void* dest, void* src, u8 size)
{
	if(dest == NULL)
		return;

	//	Save To Queue
	memcpy(dest, src, size);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/06/21
 * modify : hk
 * description: Queue Pop Data
 */
void fnQueuePop(void* src, void* dest, u8 size)
{
	if(src == NULL)
		return;

	//	Pop From Queue
	memcpy(dest, src, size);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/06/30
 * modify : hk
 * description: Free Queue
 */
void fnQueueFree(void* queue)
{
	// free queue
	free(queue);

	return;
}
//-----------------------------------------------------
