#ifndef _USB_QUEUE_H_
#define _USB_QUEUE_H_

#include "common.h"

#define USB_QUEUE_SIZE	20

typedef enum
{
	UQ_AUTOTEST = 1,	//	Auto Test

} USB_QUEUE_CMD_T;

typedef struct
{
	u8 	cmd;			//	USB_QUEUE_CMD_T

}	USB_QUQEUE_T;

//	Function
void fnUSBQueueInit(void);
void fnUSBQueuePush(USB_QUQEUE_T usb);
u8   fnUSBQueuePop(USB_QUQEUE_T *usb);
void fnUSBQueueFree(void);

#endif	//	_USB_QUEUE_H_
