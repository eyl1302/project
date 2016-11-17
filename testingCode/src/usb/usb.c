#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include "common.h"
#include "system.h"
#include "usb.h"
#include "usbComm.h"
#include "usbQueue.h"
#include "systemQueue.h"

//	Variable

//	Function
void fnUSBThread(void *arg);
void fnUSBReplytoSystem(u8 reply);

/*
 * date: 2016/09/26
 * modify : hk
 * description: Create USB Control Thread
 */
void fnUSBThreadCreate(void)
{
	pthread_t 		usb_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	USB Thread
	pthread_attr_setstacksize(&attr, USB_STACK_SIZE);

	if(pthread_create(&usb_thread, &attr, (void*)&fnUSBThread, NULL))
		printf("\n  ### hk_dbg, %s : usb create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/26
 * modify : hk
 * description: Create USB Control Thraed
 */
void fnUSBThread(void *arg)
{
	u8			 reply;
	USB_QUQEUE_T usb;

	//	Initial USB Queue
	fnUSBQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnUSBQueuePop(&usb) == HK_SUCCESS)
		{
			if(usb.cmd == UQ_AUTOTEST)
			{
				//	Process RTC Event
				reply = fnUSBProcess(UC_START);

				//	Reply To System
				fnUSBReplytoSystem(reply);
			}
		}

		usleep(USB_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/26
 * modify : hk
 * description: Reply to System Module
 */
void fnUSBReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_USB;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: USB Initial
 */
void fnUSBInit(void)
{
	//	Create USB Thread
	fnUSBThreadCreate();

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: USB Start To Test
 */
u8 fnUSBStartTest(void)
{
	u8	result;

	result = fnUSBVerify();

	if(result == HK_SUCCESS )
		printf("\n  ### hk_dbg, %s : USB PASS!!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : USB FAIL!!!\n", __FUNCTION__);

	return result;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: USB Debug
 */
u8 fnUSBDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ====================== USB Debug =====================\r\n\n");
		printf("  %d. Read USB                                              \r\n\n", idx++);
		printf("  %d. Write USB                                             \r\n\n", idx++);
    	printf("  %d. Return                                                \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
    		case UD_READ:
    		{
    			char	str[128];

    			if(fnUSBRead(str) == HK_SUCCESS)
					printf("\n  ### hk_dbg, %s : Previous String: %s\n", __FUNCTION__, str);
    		}
    		break;
    		
			case UD_WRITE:
			{
				char 	str[128];

				printf("\n  ### hk_dbg, %s : Please Input Text: ", __FUNCTION__);

				//	Scan Input String
				scanf("\t %s", str);

				fnUSBWrite(str);
			}
			break;

			case UD_RETURN:
				return HK_NA;
			break;

			default:
				hk_dbg("\n  ### hk_dbg, %s : Unknow command:%d, Please Try again! \n", __FUNCTION__, cmd);
			break;
    	}

    	sleep(1);
	}

	return HK_NA;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Process USB Event
 */
u8 fnUSBProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case UC_START:
			result = fnUSBStartTest();
		break;

		case UC_DBG:
			result = fnUSBDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow rtc command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
