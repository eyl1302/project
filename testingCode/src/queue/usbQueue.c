#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "usbQueue.h"

#define USB_QUEUE	"USB_QUEUE"

u8				usbPush = 0;	//	Push Index
u8				usbPop  = 0;	//	Pop Index
u8				usbTotal = 0;	//	How many data in the queue
USB_QUQEUE_T	*usbQueue = NULL;

/*
 * date: 2016/09/26
 * modify : hk
 * description: USB Queue Initial
 */
void fnUSBQueueInit(void)
{
	hk_dbg("### hk_dbg, %s : Create %s\n", __FUNCTION__, USB_QUEUE);

	//	Allocate USB Queue
	usbQueue = fnQueueInit(USB_QUEUE_SIZE, sizeof(USB_QUQEUE_T), QID_USB);

	//hk_dbg("### hk_dbg, %s : usbQueue:%p\n", __FUNCTION__, usbQueue);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/26
 * modify : hk
 * description: USB Queue Push Data
 */
void fnUSBQueuePush(USB_QUQEUE_T usb)
{
	if(usbQueue == NULL)
		return;

	if(usbTotal < USB_QUEUE_SIZE)
	{
		//hk_dbg("### hk_dbg, %s : Push USB to Queue, total: %d, push to:%d!!\n", __FUNCTION__, usbTotal, usbPush);

		//	Push To USB Queue
		fnQueuePush(&usbQueue[usbPush], &usb, sizeof(USB_QUQEUE_T));

		usbPush++;

		if(usbPush >= USB_QUEUE_SIZE)
			usbPush = 0;

		usbTotal++;
	}
	else
		hk_dbg("### hk_dbg, %s : USB Queue Full!!!\n", __FUNCTION__);

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/09/26
 * modify : hk
 * description: USB Queue Pop Data
 */
u8 fnUSBQueuePop(USB_QUQEUE_T *usb)
{
	if(usbQueue == NULL)
		return HK_FAIL;

	if(usbTotal)
	{
		//hk_dbg("### hk_dbg, %s : Pop USB From Queue, total:%d, pop from :%d!!\n", __FUNCTION__, usbTotal, usbPop);

		//	Pop From USB Queue
		fnQueuePop(&usbQueue[usbPop], usb, sizeof(USB_QUQEUE_T));

		usbPop++;

		if(usbPop >= USB_QUEUE_SIZE)
			usbPop = 0;

		usbTotal--;
	}
	else
	{
		//hk_dbg("### hk_dbg, %s : USB Queue Empty!!!\n", __FUNCTION__);

		return HK_FAIL;
	}

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/26
 * modify : hk
 * description: Free USB Queue
 */
void fnUSBQueueFree(void)
{
	if(usbQueue == NULL)
		return;

	hk_dbg("### hk_dbg, %s : Free %s\n", __FUNCTION__, USB_QUEUE);

	//	Free USB Queue
	fnQueueFree(usbQueue);

	usbPush = 0;
	usbPop = 0;
	usbTotal = 0;

	return;
}
//-----------------------------------------------------
