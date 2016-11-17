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
#include "tpanel.h"
#include "eeti.h"
#include "tpanelQueue.h"
#include "systemQueue.h"

//	Variable
TPANEL_DEVICE_HANDLE tpanelHandle;

//	Function
void fnTPanelThread(void *arg);
void fnTPanelReplytoSystem(u8 reply);

/*
 * date: 2016/10/04
 * modify : hk
 * description: Create Touch Panel Control Thread
 */
void fnTPanelThreadCreate(void)
{
	pthread_t 		tpanel_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	Touch Panel Thread
	pthread_attr_setstacksize(&attr, TPANEL_STACK_SIZE);

	if(pthread_create(&tpanel_thread, &attr, (void*)&fnTPanelThread, NULL))
		printf("\n  ### hk_dbg, %s : touch panel create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/04
 * modify : hk
 * description: Create Touch Panel Thraed
 */
void fnTPanelThread(void *arg)
{
	u8			 		reply;
	TPANEL_QUQEUE_T 	tpanel;

	//	Initial Touch Panel Queue
	fnTPanelQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnTPanelQueuePop(&tpanel) == HK_SUCCESS)
		{
			if(tpanel.cmd == TQ_AUTOTEST)
			{
				//	Process Touch Panel Event
				reply = fnTPanelProcess(TC_START);

				//	Reply To System
				fnTPanelReplytoSystem(reply);
			}
		}

		usleep(TPANEL_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/04
 * modify : hk
 * description: Reply to System Module
 */
void fnTPanelReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_TPANEL;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/04
 * modify : hk
 * description: Touch Panel Initial
 */
void fnTPanelInit(void)
{
	//	Create Audio Queue
	fnTPanelThreadCreate();

	//	Point Touch Panel Access Function to Device(eeti...)
	tpanelHandle.tpanelFunc = &eeti;

	//	Initial
	tpanelHandle.tpanelFunc->pInit();

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/14
 * modify : hk
 * description: Touch Panel Start Test
 */
u8 fnTPanelStartTest(void)
{
	u8 result;

	result = tpanelHandle.tpanelFunc->pTest();

	if(result == HK_SUCCESS)
		printf("\n  ### hk_dbg, %s : Touch Panel OK!!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : Touch Panel FAIL!!!\n", __FUNCTION__);

	return result;
}
//------------------------------------------------

/*
 * date: 2016/10/14
 * modify : hk
 * description: Touch Panel Debug
 */
u8 fnTPanelDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  =====================  Touch Panel Debug ===================\r\n\n");
    	printf("  %d. Read Register										  		  \r\n\n", idx++);
    	//printf("  %d. Write Register											  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
    		case TD_REG_READ:
    		{
    			u8 reg;
				u8 data;

				char	str[32];

				printf("\n  Please Input Register Address (Format:aa): ");

				scanf("\t %s", str);

				if(strlen(str) == TPL_REG_LEN)	//	a(2) 
				{
					reg = fnStrToHEX(str, TPL_REG_STR,  TPL_REG_LEN);

					tpanelHandle.tpanelFunc->pRegRead(reg, &data);

					printf("\n  ### hk_dbg, %s : read reg:0x%02x, data:0x%02x!! \n", __FUNCTION__, reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
    		}
    		break;

    		//case TD_REG_WRITE:
			//{
			//	u8 reg;
			//	u8 data;
           //
			//	char	str[64]; 
           //
			//	printf("\n  Please Inpu Register & Data(Format:aadd): ");
           //
			//	scanf("\t %s", str);
           //
			//	if(strlen(str) == (TPL_REG_LEN + TPL_DATA_LEN))	//	a(2)+d(2)
			//	{
			//		reg  = fnStrToHEX(str, TPL_REG_STR,  TPL_REG_LEN);
           //
			//		data = fnStrToHEX(str, TPL_DATA_STR,  TPL_DATA_LEN);
           //
			//		tpanelHandle.tpanelFunc->pRegWrite(reg, data);
			//	}
			//	else
			//		printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			//}
    		//break;

			case TD_RETURN:
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
 * date: 2016/10/04
 * modify : hk
 * description: Process Touch Panel Event
 */
u8 fnTPanelProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case TC_START:
			result = fnTPanelStartTest();
		break;

		case TC_DBG:
			result = fnTPanelDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow touch panel command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
