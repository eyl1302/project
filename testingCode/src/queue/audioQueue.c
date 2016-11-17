#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "audioQueue.h"

#define AUDIO_QUEUE	"AUDIO_QUEUE"

u8				audioPush = 0;	//	Push Index
u8				audioPop  = 0;	//	Pop Index
u8				audioTotal = 0;	//	How many data in the queue
AUDIO_QUQEUE_T	*audioQueue = NULL;

/*
 * date: 2016/07/12
 * modify : hk
 * description: Audio Queue Initial
 */
void fnAudioQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, AUDIO_QUEUE);

	//	Allocate Battery Queue
	audioQueue = fnQueueInit(AUDIO_QUEUE_SIZE, sizeof(AUDIO_QUQEUE_T), QID_AUDIO);

	//hk_dbg("### hk_dbg, %s : audioQueue:%p\n", __FUNCTION__, audioQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Audio Queue Push Data
 */
void fnAudioQueuePush(AUDIO_QUQEUE_T audio)
{
	if(audioQueue == NULL)
		return;

	if(audioTotal < AUDIO_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push Battery to Queue, total: %d, push to:%d!!\n", __FUNCTION__, audioTotal, audioPush);

		//	Push To Battery Queue
		fnQueuePush(&audioQueue[audioPush], &audio, sizeof(AUDIO_QUQEUE_T));

		audioPush++;

		if(audioPush >= AUDIO_QUEUE_SIZE)
			audioPush = 0;

		audioTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : Audio Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Audio Queue Pop Data
 */
u8   fnAudioQueuePop(AUDIO_QUQEUE_T *audio)
{
	if(audioQueue == NULL)
		return HK_FAIL;

	if(audioTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop Audio From Queue!!\n", __FUNCTION__);

		//	Pop From Battery Queue
		fnQueuePop(&audioQueue[audioPop], audio, sizeof(AUDIO_QUQEUE_T));

		audioPop++;

		if(audioPop >= AUDIO_QUEUE_SIZE)
			audioPop = 0;

		audioTotal--;
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
 * date: 2016/07/12
 * modify : hk
 * description: Free Audio Queue
 */
void fnAudioQueueFree(void)
{
	if(audioQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, AUDIO_QUEUE);

	//	Free Battery Queue
	fnQueueFree(audioQueue);

	audioPush = 0;
	audioPop = 0;
	audioTotal = 0;

	return;
}
//-----------------------------------------------------
