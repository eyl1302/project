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
#include "pmic.h"
#include "pf0100.h"
#include "pmicQueue.h"
#include "systemQueue.h"

//	Variable
PMIC_DEVICE_HANDLE pmicHandle;

//	Function
void fnPMICThread(void *arg);
void fnPMICReplytoSystem(u8 reply);

/*
 * date: 2016/10/05
 * modify : hk
 * description: Create PMIC Control Thread
 */
void fnPMICThreadCreate(void)
{
	pthread_t 		pmic_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	PMIC Thread
	pthread_attr_setstacksize(&attr, PMIC_STACK_SIZE);

	if(pthread_create(&pmic_thread, &attr, (void*)&fnPMICThread, NULL))
		printf("\n  ### hk_dbg, %s : pmic create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: Create PMIC Thraed
 */
void fnPMICThread(void *arg)
{
	u8			 	reply;
	PMIC_QUQEUE_T 	pmic;

	//	Initial PMIC Queue
	fnPMICQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnPMICQueuePop(&pmic) == HK_SUCCESS)
		{
			if(pmic.cmd == PQ_AUTOTEST)
			{
				//	Process Touch Panel Event
				reply = fnPMICProcess(PC_START);

				//	Reply To System
				fnPMICReplytoSystem(reply);
			}
		}

		usleep(PMIC_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: Reply to System Module
 */
void fnPMICReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_PMIC;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: PMIC Initial
 */
void fnPMICInit(void)
{
	//	Create Audio Queue
	fnPMICThreadCreate();

	//	Point PMIC Access Function to Device(pf0100...)
	pmicHandle.pmicFunc = &pf0100;

	//	Initial PMIC
	pmicHandle.pmicFunc->pInit();

	return;
}
//------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: PMIC Start Test
 */
u8 fnPMICStartTest(void)
{
	u8 result;

	//	Initial PMIC
	result = pmicHandle.pmicFunc->pTest();

	if(result == HK_SUCCESS)
		printf("\n  ### hk_dbg, %s : PMIC OK!!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : PMIC FAIL!!!\n", __FUNCTION__);

	return result;
}
//------------------------------------------------

/*
 * date: 2016/10/05
 * modify : hk
 * description: PMIC Debug
 */
u8 fnPMICDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ========================  PMIC Debug =======================\r\n\n");
		printf("  %d. List Current Setting								  		  \r\n\n", idx++);
		printf("  %d. Config Current Setting							  		  \r\n\n", idx++);
    	printf("  %d. Read Register										  		  \r\n\n", idx++);
    	printf("  %d. Write Register											  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
    		case PD_LIST:
    			pmicHandle.pmicFunc->pList();
    		break;

    		case PD_CONFIG:
    			pmicHandle.pmicFunc->pConfig();
    		break;

    		case PD_REG_READ:
    		{
    			u8 reg;
				u8 data;

				char	str[32];

				printf("\n  Please Input Register Address (Format:aa): ");

				scanf("\t %s", str);

				if(strlen(str) == PMIC_REG_LEN)	//	a(2) 
				{
					reg = fnStrToHEX(str, PMIC_REG_STR,  PMIC_REG_LEN);

					pmicHandle.pmicFunc->pRegRead(reg, &data);

					printf("\n  ### hk_dbg, %s : read reg:0x%02x, data:0x%02x!! \n", __FUNCTION__, reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
    		}
    		break;

    		case PD_REG_WRITE:
			{
				u8 reg;
				u8 data;

				char	str[64];

				printf("\n  Please Inpu Register & Data(Format:aadd): ");

				scanf("\t %s", str);

				if(strlen(str) == (PMIC_REG_LEN + PMIC_DATA_LEN))	//	a(2)+d(2)
				{
					reg  = fnStrToHEX(str, PMIC_REG_STR,  PMIC_REG_LEN);

					data = fnStrToHEX(str, PMIC_DATA_STR,  PMIC_DATA_LEN);

					pmicHandle.pmicFunc->pRegWrite(reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
    		break;

			case PD_RETURN:
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
 * description: Process PMIC Event
 */
u8 fnPMICProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case PC_START:
			result = fnPMICStartTest();
		break;

		case PC_DBG:
			result = fnPMICDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow pmic command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
