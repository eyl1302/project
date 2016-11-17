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
#include "sdram.h"
#include "mt41kxx.h"
#include "sdramQueue.h"
#include "systemQueue.h"

//	Variable
u8 vfyMode;							//	Verify Mode
SDRAM_DEVICE_HANDLE sdramHandle;

//	Function
void fnSDRAMThread(void *arg);
void fnSDRAMReplytoSystem(u8 reply);

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create SDRAM Control Thread
 */
void fnSDRAMThreadCreate(void) 
{
	pthread_t 		sdram_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	SDRAM Thread
	pthread_attr_setstacksize(&attr, SDRAM_STACK_SIZE);

	if(pthread_create(&sdram_thread, &attr, (void*)&fnSDRAMThread, NULL))
		printf("\n  ### hk_dbg, %s : SDRAM create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create SDRAM Control Thraed
 */
void fnSDRAMThread(void *arg)
{
	u8			 	reply;
	SDRAM_QUQEUE_T 	sdram;

	//	Initial SDRAM Queue
	fnSDRAMQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnSDRAMQueuePop(&sdram) == HK_SUCCESS)
		{
			if(sdram.cmd == DQ_AUTOTEST)
			{
				//	Process SDRAM Event
				reply = fnSDRAMProcess(DC_START);

				//	Reply To System
				fnSDRAMReplytoSystem(reply);
			}
		}

		usleep(SDRAM_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/22
 * modify : hk
 * description: Reply to System Module
 */
void fnSDRAMReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_SDRAM;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/08/24
 * modify : hk
 * description: SDRAM Initial
 */
void fnSDRAMInit(void)
{
	//	Create SDRAM Thread
	fnSDRAMThreadCreate();

	//	Verify Mode
	vfyMode = VM_QUICK;//VM_ALTERNATIVE;//

	//	Point Audio Access Function to Device(MT41Kx)
	sdramHandle.sdramFunc = &mt41kxx;

	//	Initial SDRAM
	sdramHandle.sdramFunc->pInit(vfyMode);

	return;
}
//------------------------------------------------

/*
 * date: 2016/08/24
 * modify : hk
 * description: SDRAM Start Test
 */
u8 fnSDRAMStartTest(void)
{
	u8 result;

	result = sdramHandle.sdramFunc->pVerify(0, 0);

	if(result)
		printf("\n  ### hk_dbg, %s : DDR PASS!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : DDR FAIL!!\n", __FUNCTION__);

	return result;
}
//------------------------------------------------

/*
 * date: 2016/08/24
 * modify : hk
 * description: SDRAM Debug
 */
u8 fnSDRAMDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================= SDRAM Debug ========================\r\n\n");
    	printf("  %d. Read Function										  		  \r\n\n", idx++);
    	printf("  %d. Write Function											  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
			case DD_FUNC_READ:
			{
				u16 	len;
				u32 	data;
				u64 	addr;
				char	str[64];

				printf("\n  Please Inpu Memory Address(ex: 123 -> 0x123): ");

				scanf("\t %s", str);

				len = strlen(str);

				//printf("### hk_dbg, %s : str len:%d!!\n", __FUNCTION__, len);

				addr = fnStrToHEX(str, 0, len);

				sdramHandle.sdramFunc->pFunRead(addr, &data);

				printf("\n  ### hk_dbg, %s, addr:0x%08lx, data:0x%08x!! \n", __FUNCTION__, addr, data);
			}
			break;
			
			case DD_FUNC_WRITE:
			{
				u16 	len;
				u32 	data;
				u64 	addr;
				char	str[64];

				printf("\n  Please Inpu Memory Address(ex: 123 -> 0x123): ");

				scanf("\t %s", str);

				len = strlen(str);

				//printf("### hk_dbg, %s : str len:%d!!\n", __FUNCTION__, len);

				addr = fnStrToHEX(str, 0, len);

				printf("\n  Please Inpu Data(ex: 456 -> 0x456): ");

				scanf("\t %s", str);

				len = strlen(str);

				//printf("### hk_dbg, %s : str len:%d!!\n", __FUNCTION__, len);

				data = fnStrToHEX(str, 0, len);

				sdramHandle.sdramFunc->pFunWrite(addr, data);
			}
			break;

			case DD_RETURN:
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
 * date: 2016/08/24
 * modify : hk
 * description: Process SDRAM Event
 */
u8 fnSDRAMProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case DC_START:
			result = fnSDRAMStartTest();
		break;

		case DC_DBG:
			result = fnSDRAMDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow sdram command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
