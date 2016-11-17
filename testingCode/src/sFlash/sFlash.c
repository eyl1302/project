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
#include "sFlash.h"
#include "mx25l3206e.h"
#include "sflashQueue.h"
#include "systemQueue.h"

//	Variable
SFLASH_DEVICE_HANDLE	sflash;

//	Function
void fnSFlashThread(void *arg);
void fnSFlashReplytoSystem(u8 reply);

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create SPI Flash Control Thread
 */
void fnSFlashThreadCreate(void)
{
	pthread_t 		sflash_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	SPI Flash Thread
	pthread_attr_setstacksize(&attr, SFLASH_STACK_SIZE);

	if(pthread_create(&sflash_thread, &attr, (void*)&fnSFlashThread, NULL))
		printf("\n  ### hk_dbg, %s : spi flash create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create SPI Flash Control Thraed
 */
void fnSFlashThread(void *arg)
{
	u8			 	reply;
	SFLASH_QUQEUE_T sflash;

	//	Initial SPI Flash Queue
	fnSFlashQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnSFlashQueuePop(&sflash) == HK_SUCCESS)
		{
			if(sflash.cmd == SQ_AUTOTEST)
			{
				//	Process SPI Flash Event
				reply = fnSFlashProcess(SC_START);

				//	Reply To System
				fnSFlashReplytoSystem(reply);
			}
		}

		usleep(SFLASH_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Reply to System Module
 */
void fnSFlashReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_SFLASH;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/23
 * modify : hk
 * description: SPI Flash Initial
 */
void fnSFlashInit(void)
{
	//	Create SPI Flash Thread
	fnSFlashThreadCreate();

	sflash.sflashFunc = &mx25l3206e;

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/25
 * modify : hk
 * description: Start Test SPI Flash
 */
u8 fnSFlashStartTest(void)
{
	u8 result;

	result = sflash.sflashFunc->pVerify();

	if(result)
		printf("\n  ### hk_dbg, %s : SPI Flash PASS!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : SPI Flash FAIL!!\n", __FUNCTION__);

	return result;
}
//------------------------------------------------

/*
 * date: 2016/07/25
 * modify : hk
 * description: SPI Flash Debug
 */
u8 fnSFlashDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ====================== SPI Flash Debug =====================\r\n\n");
		printf("  %d. Display Flash Organization						  		  \r\n\n", idx++);
		printf("  %d. Read Flash										  		  \r\n\n", idx++);
		printf("  %d. Write Flash(Caution: Erase Flash in advacne)  	  		  \r\n\n", idx++);
		printf("  %d. Erase Flash	 									  		  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
    		case SD_INFO:
    		{
				sflash.sflashFunc->pInfo();
    		}
    		break;
    		
			case SD_READ:
			{
				sflash.sflashFunc->pRead();
			}
			break;

			case SD_WRITE:
			{
				sflash.sflashFunc->pWrite();		
			}
			break;

			case SD_ERASE:
			{
				sflash.sflashFunc->pErase();
			}
			break;

			case SD_RETURN:
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
 * date: 2016/07/23
 * modify : hk
 * description: Process SPI Flash Event
 */
u8 fnSFlashProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case SC_START:
			result = fnSFlashStartTest();
		break;

		case SC_DBG:
			result = fnSFlashDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow spi flash command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
