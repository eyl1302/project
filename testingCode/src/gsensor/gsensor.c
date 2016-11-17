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
#include "gsensor.h"
#include "mma8452.h"
#include "gsensorQueue.h"
#include "systemQueue.h"

//	Variable
GSENSOR_DEVICE_HANDLE	gsensor;

//	Function
void fnGSensorThread(void *arg);
void fnGSensorReplytoSystem(u8 reply);

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create GSensor Control Thread
 */
void fnGSensorThreadCreate(void)
{
	pthread_t 		gsensor_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	GSensor Thread
	pthread_attr_setstacksize(&attr, GSENSOR_STACK_SIZE);

	if(pthread_create(&gsensor_thread, &attr, (void*)&fnGSensorThread, NULL))
		printf("\n  ### hk_dbg, %s : gsensor create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create GSensor Control Thraed
 */
void fnGSensorThread(void *arg)
{
	u8			 		reply;
	GSENSOR_QUQEUE_T 	gsensor;

	//	Initial GSensor Queue
	fnGSensorQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnGSensorQueuePop(&gsensor) == HK_SUCCESS)
		{
			if(gsensor.cmd == GQ_AUTOTEST)
			{
				//	Process GSensor Event
				reply = fnGSensorProcess(GC_START);

				//	Reply To System
				fnGSensorReplytoSystem(reply);
			}
		}

		usleep(GSENSOR_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Reply to System Module
 */
void fnGSensorReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_GSENSOR;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/15
 * modify : hk
 * description: GSensor Initial
 */
void fnGSensorInit(void)
{
	//	Create GSensor Thread
	fnGSensorThreadCreate();

	//	Point GSensor Access Function to Device(mma8452...)
	gsensor.gsensorFunc = &mma8452;

	//	Initial GSensor
	gsensor.gsensorFunc->pInit(0);	//	0:Motion Detection, 1:Free Fall Detection

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/18
 * modify : hk
 * description: GSensor Start Test
 */
u8 fnGSensorStartTest(void)
{
	GSENSOR_OUTPUT gsr;

	//	Read X, Y, Z Out
	if(gsensor.gsensorFunc->pReadOut(&gsr) == HK_SUCCESS)
	{
		printf("\n");
		printf("\n  ### hk_dbg, %s : x out:%1.4fg!!\n", __FUNCTION__, gsr.x);
		printf("\n  ### hk_dbg, %s : y out:%1.4fg!!\n", __FUNCTION__, gsr.y);
		printf("\n  ### hk_dbg, %s : z out:%1.4fg!!\n", __FUNCTION__, gsr.z);

		printf("\n  ### hk_dbg, %s : GSensor PASS!!!\n", __FUNCTION__);

		return HK_SUCCESS;
	}
	else
		printf("\n  ### hk_dbg, %s : GSensor FAIL!!!\n", __FUNCTION__);

	return HK_FAIL;
}
//------------------------------------------------

/*
 * date: 2016/07/18
 * modify : hk
 * description: GSensor Debug
 */
u8 fnGSensorDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================  GSensor Debug ======================\r\n\n");
		printf("  %d. GSensor Calibration								  		  \r\n\n", idx++);
		printf("  %d. Read Register										  		  \r\n\n", idx++);
		printf("  %d. Write Register									  		  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
    		case GD_CALIBRATION:
				gsensor.gsensorFunc->pCalibration(1);
    		break;

			case GD_REG_READ:
			{
				u8 reg;
				u8 data;

				char	str[32];

				printf("\n  Please Input Register Address (Format:aa): ");

				scanf("\t %s", str);

				if(strlen(str) == GSENSOR_REG_LEN)	//	a(2)
				{
					reg = fnStrToHEX(str, GSENSOR_REG_STR,  GSENSOR_REG_LEN);

					gsensor.gsensorFunc->pRegRead(reg, &data);

					printf("\n  ### hk_dbg, %s : read reg:0x%02x, data:0x%02x!! \n", __FUNCTION__, reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;

			case GD_REG_WRITE:
			{
				u8 reg;
				u8 data;

				char	str[64];

				printf("\n  Please Inpu Register & Data(Format:iidd): ");

				scanf("\t %s", str);

				if(strlen(str) == (GSENSOR_REG_LEN + GSENSOR_DATA_LEN))	//	r(2)+d(2)
				{
					reg  = fnStrToHEX(str, GSENSOR_REG_STR,  GSENSOR_REG_LEN);

					data = fnStrToHEX(str, GSENSOR_DATA_STR,  GSENSOR_DATA_LEN);

					gsensor.gsensorFunc->pRegWrite(reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;

			case GD_RETURN:
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
 * date: 2016/07/15
 * modify : hk
 * description: Process GSensor Event
 */
u8 fnGSensorProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case GC_START:
			result = fnGSensorStartTest();
		break;

		case GC_DBG:
			result = fnGSensorDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow gsensor command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
