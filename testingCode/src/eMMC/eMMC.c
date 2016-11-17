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
#include "eMMC.h"
#include "emmc04gxx.h"
#include "emmcQueue.h"
#include "systemQueue.h"

//	Variable
EMMC_DEVICE_HANDLE eMMCHandle;

//	Function
void fnEMMCThread(void *arg);
void fnEMMCReplytoSystem(u8 reply);

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create EMMC Control Thread
 */
void fnEMMCThreadCreate(void)
{
	pthread_t 		emmc_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	EMMC Thread
	pthread_attr_setstacksize(&attr, EMMC_STACK_SIZE);

	if(pthread_create(&emmc_thread, &attr, (void*)&fnEMMCThread, NULL))
		printf("\n  ### hk_dbg, %s : EMMC create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create EMMC Control Thraed
 */
void fnEMMCThread(void *arg)
{
	u8			 	reply;
	EMMC_QUQEUE_T 	emmc;

	//	Initial EMMC Queue
	fnEMMCQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnEMMCQueuePop(&emmc) == HK_SUCCESS)
		{
			if(emmc.cmd == EQ_AUTOTEST)
			{
				//	Process RTC Event
				reply = fnEMMCProcess(EC_START);

				//	Reply To System
				fnEMMCReplytoSystem(reply);
			}
		}

		usleep(EMMC_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Reply to System Module
 */
void fnEMMCReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_EMMC;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------


/*
 * date: 2016/09/13
 * modify : hk
 * description: eMMC Initial
 */
void fnEMMCInit(void)
{
	//	Create EMMC Thread
	fnEMMCThreadCreate();

	//	Point Audio Access Function to Device(emmc04gxx..)
	eMMCHandle.eMMCFunc = &emmc04gxx;

	//	Initial eMMC
	eMMCHandle.eMMCFunc->pInit();

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/13
 * modify : hk
 * description: eMMC Start Test
 */
u8 fnEMMCStartTest(void)
{
	u8 result;

	result = eMMCHandle.eMMCFunc->pVerify(0);	//	0:Quick, 1:Low Level

	if(result)
		printf("\n  ### hk_dbg, %s : eMMC PASS!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : eMMC FAIL!!\n", __FUNCTION__);

	return result;
}
//------------------------------------------------

/*
 * date: 2016/09/13
 * modify : hk
 * description: eMMC Debug
 */
u8 fnEMMCDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

    	printf("\r\n  ======================= eMMC Debug =========================\r\n\n");
    	printf("  %d. Read Function										  		  \r\n\n", idx++);
    	printf("  %d. Write Function											  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
			case ED_FUNC_READ:
			{
				char str[128];

				//	Read String from eMMC
				if(eMMCHandle.eMMCFunc->pFunRead(0, str) == HK_SUCCESS)
					printf("\n  ### hk_dbg, %s : Input Text: %s\n", __FUNCTION__, str);
			}
			break;
			
			case ED_FUNC_WRITE:
			{
				char str[128];

				printf("\n ### hk_dbg, %s : Please Input Text: ", __FUNCTION__);

				//	Scan Input String
				scanf("\t %s", str);

				//	Write String to eMMC
				eMMCHandle.eMMCFunc->pFunWrite(0, str);
			}
			break;

			case ED_RETURN:
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
 * date: 2016/09/13
 * modify : hk
 * description: Process eMMC Event
 */
u8 fnEMMCProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case EC_START:
			result = fnEMMCStartTest();
		break;

		case EC_DBG:
			result = fnEMMCDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow eMMC command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
