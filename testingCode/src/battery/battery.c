#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include "common.h"
#include "system.h"
#include "battery.h"
#include "bq27541.h"
#include "batteryQueue.h"
#include "systemQueue.h"

//	Varibale
BATTERY_DEVICE_HANDLE batteryHandle;

//	Internal Function
void fnBatteryThread(void *arg);
void fnBatteryReplytoSystem(u8 reply);
//void fnGGaugeProcess(BATTERY_DEVICE_HANDLE *batteryHandle);

/*
 * date: 2016/09/22
 * modify : hk
 * description: Create Battery Control Thread
 */
void fnBatteryThreadCreate(void) 
{
	pthread_t 		battery_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	Battery Thread
	pthread_attr_setstacksize(&attr, BATTERY_STACK_SIZE);

	if(pthread_create(&battery_thread, &attr, (void*)&fnBatteryThread, NULL))
		printf("\n  ### hk_dbg, %s : battery create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/22
 * modify : hk
 * description: Create Battery Control Thraed
 */
void fnBatteryThread(void *arg)
{
	u8			 		reply;
	BATTERY_QUQEUE_T 	bty;

	//	Initial Battery Queue
	fnBatteryQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnBatteryQueuePop(&bty) == HK_SUCCESS)
		{
			if(bty.cmd == BQ_AUTOTEST)
			{
				//	Process RTC Event
				reply = fnBatteryProcess(BC_START);

				//	Reply To System
				fnBatteryReplytoSystem(reply);
			}
		}

		usleep(BATTERY_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/22
 * modify : hk
 * description: Reply to System Module
 */
void fnBatteryReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_BATTERY;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: Initial Battery Control
 */
void fnBatteryInit(void)
{
	//	Craete Battery Thread
	fnBatteryThreadCreate();

	//	Point Battery Access Function to Device(bq27541...)
	batteryHandle.batteryFunc = &bq27541;

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Debug Battery
 */
u8 fnBatteryDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================  Battery Debug ======================\r\n\n");
    	printf("  %d. Read Register										  		  \r\n\n", idx++);
    	printf("  %d. Write Register											  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
			case BD_REG_READ:
			{
				u8 reg;
				u8 data;

				char	str[32];

				printf("\n  Please Inpu Register (Format:rr): ");

				scanf("\t %s", str);

				if(strlen(str) == BATTERY_REG_LEN)	//	a(2)
				{
					reg = fnStrToHEX(str, BATTERY_REG_STR,  BATTERY_REG_LEN);

					batteryHandle.batteryFunc->pRegRead(reg, &data);

					printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;
			
			case BD_REG_WRITE:
			{
				u8 reg;
				u8 data;

				char	str[64];

				printf("\n  Please Inpu Register & Data(Format:rrdd): ");

				scanf("\t %s", str);

				if(strlen(str) == (BATTERY_REG_LEN + BATTERY_DATA_LEN))	//	r(2)+d(2)
				{
					reg  = fnStrToHEX(str, BATTERY_REG_STR,  BATTERY_REG_LEN);
					data = fnStrToHEX(str, BATTERY_DATA_STR,  BATTERY_DATA_LEN);

					printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, reg, data);

					batteryHandle.batteryFunc->pRegWrite(reg, data);

					batteryHandle.batteryFunc->pRegRead(reg, &data);

					printf("\n  ### hk_dbg, %s : reg:0x%02x, data:0x%02x\n", __FUNCTION__, reg, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;
			
			case BD_RETURN:
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
 * date: 2016/06/21
 * modify : hk
 * description: Process Battery Event
 */
u8 fnBatteryProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case BC_START:
		{
			result = batteryHandle.batteryFunc->pBatteryGet(&batteryHandle.battery);

			printf("\n  ");
			system("date");	//	Print date & time for read gas gauge
			printf("\n  ### hk_dbg, %s, Capacity    : %d!!\n", __FUNCTION__, batteryHandle.battery.cap);
			printf("\n  ### hk_dbg, %s, Temperature : %d!!\n", __FUNCTION__, batteryHandle.battery.temp);
			printf("\n  ### hk_dbg, %s, Current     : %d!!\n", __FUNCTION__, batteryHandle.battery.cur);
			printf("\n  ### hk_dbg, %s, Voltage     : %d!!\n", __FUNCTION__, batteryHandle.battery.vol);
			printf("\n  ### hk_dbg, %s, Status      : %s!!\n", __FUNCTION__, batteryHandle.battery.status);

			if(result == HK_SUCCESS)
			{
				if(batteryHandle.battery.cap > CAPCITY_MIN)									//	Capacity
				{
					if(batteryHandle.battery.temp < TEMPERATURE_MAX)						//	Temperature
					{
						if(batteryHandle.battery.cur == CURRENT_MIN)						//	Current
						{
							if(batteryHandle.battery.vol >  VOLTAGE_MIN)					//	Voltage
							{
								printf("\n  ### hk_dbg, %s : Battery Pass!! \n", __FUNCTION__);

								return HK_SUCCESS;
							}
						}
					}
				}
			}

			printf("\n  ### hk_dbg, %s : Battery Fail!! \n", __FUNCTION__);

			return HK_FAIL;
		}
		break;

		case BC_DBG:
			result = fnBatteryDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow battery command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------