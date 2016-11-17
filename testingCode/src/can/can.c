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
#include "can.h"
#include "canQueue.h"
#include "systemQueue.h"

//	Variable
CAN_DEVICE_HANDLE canHandle;

//	Function
void fnCANThread(void *arg);
void fnCANReplytoSystem(u8 reply);

/*
 * date: 2016/10/20
 * modify : hk
 * description: Create CAN Control Thread
 */
void fnCANThreadCreate(void)
{
	pthread_t 		can_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	CAN Thread
	pthread_attr_setstacksize(&attr, CAN_STACK_SIZE);

	if(pthread_create(&can_thread, &attr, (void*)&fnCANThread, NULL))
		printf("\n  ### hk_dbg, %s : can create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: Create CAN Thraed
 */
void fnCANThread(void *arg)
{
	u8			 	reply;
	CAN_QUQEUE_T 	can;

	//	Initial CAN Queue
	fnCANQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnCANQueuePop(&can) == HK_SUCCESS)
		{
			if(can.cmd == CQ_AUTOTEST)
			{
				//	Process Touch Panel Event
				reply = fnCANProcess(NC_START);

				//	Reply To System
				fnCANReplytoSystem(reply);
			}
		}

		usleep(CAN_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: Reply to System Module
 */
void fnCANReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_CAN;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: CAN Initial
 */
void fnCANInit(void)
{
	//	Create CAN Thread
	fnCANThreadCreate();

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: CAN Start Test
 */
u8 fnCANStartTest(void)
{
	u8 result;

	result = HK_SUCCESS;

	if(result == HK_SUCCESS)
		printf("\n  ### hk_dbg, %s : CAN OK!!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : CAN FAIL!!!\n", __FUNCTION__);

	return result;
}
//------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: CAN Debug
 */
u8 fnCANDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ========================== CAN Debug =======================\r\n\n");
    	printf("  %d. Read 	        									  		  \r\n\n", idx++);
    	printf("  %d. Write         											  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
    		case ND_READ:
    		{
    			u8 	buf[32];
    			u16 len;

    			len = 10;

    			if(fnCANRead(buf, len) == HK_SUCCESS)
	    			printf("\n  ### hk_dbg, %s : Read Data:%s\n", __FUNCTION__, buf);
	    		else
	    			printf("\n  ### hk_dbg, %s : Read Data Fail\n", __FUNCTION__);
    		}
    		break;

    		case ND_WRITE:
			{
				u8 	buf[32];
    			u16 len;

    			len = 10;

    			if(fnCANWrite(buf, len) == HK_SUCCESS)
	    			printf("\n  ### hk_dbg, %s : Write Data Success!!\n", __FUNCTION__);
	    		else
	    			printf("\n  ### hk_dbg, %s : Write Data Fail\n", __FUNCTION__);
			}
    		break;

			case ND_RETURN:
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
 * date: 2016/10/05
 * modify : hk
 * description: Process CAN Event
 */
u8 fnCANProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case NC_START:
			result = fnCANStartTest();
		break;

		case NC_DBG:
			result = fnCANDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow pmic command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
