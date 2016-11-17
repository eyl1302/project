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
#include "audio.h"
#include "wm8960.h"
#include "pcm1681.h"
#include "audioQueue.h"
#include "systemQueue.h"

//	Variable
u8 	audio = 0;					//	0:Stop, 1:Play
u32 audioCard = 0;				//	Audio Code Card ID
AUDIO_DEVICE_HANDLE audioHandle;

//	Function
void fnAudioThread(void *arg);
void fnAudioReplytoSystem(u8 reply);
u8 fnAudioCardSelect(void);

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create Audio Control Thread
 */
void fnAudioThreadCreate(void)
{
	pthread_t 		audio_thread;
	pthread_attr_t 	attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//	Audio Thread
	pthread_attr_setstacksize(&attr, AUDIO_STACK_SIZE);

	if(pthread_create(&audio_thread, &attr, (void*)&fnAudioThread, NULL))
		printf("\n  ### hk_dbg, %s : audio create fail!!\n", __FUNCTION__);

	pthread_attr_destroy(&attr);

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Create Audio Control Thraed
 */
void fnAudioThread(void *arg)
{
	u8			 	reply;
	AUDIO_QUQEUE_T 	audio;

	//	Initial Audio Queue
	fnAudioQueueInit();

	while(1)
	{
		//	Pop From Queue
		if(fnAudioQueuePop(&audio) == HK_SUCCESS)
		{
			if(audio.cmd == AQ_AUTOTEST)
			{
				//	Process RTC Event
				reply = fnAudioProcess(AC_START);

				//	Reply To System
				fnAudioReplytoSystem(reply);
			}
		}

		usleep(AUDIO_SLEEP_TIME);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Reply to System Module
 */
void fnAudioReplytoSystem(u8 reply)
{
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_REPLY;
	sys.func = SC_AUDIO;
	sys.reply = reply;

	//	Push to System Queue
	fnSystemQueuePush(sys);

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Audio Initial
 */
void fnAudioInit(void)
{
	//	Create Audio Queue
	fnAudioThreadCreate();

	//	Point Audio Access Function to Device(wm8960...)
	audioHandle.audioFunc = &wm8960;

	//	Initial
	audioHandle.audioFunc->pInit(audioCard);

	return;
}
//------------------------------------------------

/*
 * date: 2016/08/15
 * modify : hk
 * description: Audio Card Select
 */
u8 fnAudioCardSelect(void)
{
	if(0)
	{
		char cmd[32];

		printf("\n");

		sprintf(cmd, "aplay -l");

		system(cmd);

	}

	printf("\n ### hk_dbg, %s : Please Select Audio Card(0:pcm1681, 1:wm8960):  ", __FUNCTION__);

	audioCard = fnSystemInputString();

	//printf("### hk_dbg, %s : card id:%d(%s)\n", __FUNCTION__, audioCard, CARD_STR(audioCard));

	//	Point Audio Access Function to Device(wm8960/pcm1681...)
	if(audioCard == AC_PCM1681)
		audioHandle.audioFunc = &pcm1681;
	else if(audioCard == AC_WM8960)
		audioHandle.audioFunc = &wm8960;
	else
		printf("\n ### hk_dbg, %s : Out of Range, Please try again!!\n", __FUNCTION__);

	//	Initial
	audioHandle.audioFunc->pInit(audioCard);

	return HK_NA;
}
//------------------------------------------------

/*
 * date: 2016/07/18
 * modify : hk
 * description: Audio Start Test
 */
u8 fnAudioStartTest(void)
{
	printf("\n  ### hk_dbg, %s : Audio OK!!!\n", __FUNCTION__);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Audio Debug
 */
u8 fnAudioDebug(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  =====================  %s Debug =====================\r\n\n", CARD_STR(audioCard));
		printf("  %d. List Function										  		  \r\n\n", idx++);
		printf("  %d. Play Audio	  								  		      \r\n\n", idx++);
    	printf("  %d. Read Function										  		  \r\n\n", idx++);
    	printf("  %d. Write Function											  \r\n\n", idx++);
    	//printf("  %d. Read Register										  		  \r\n\n", idx++);
    	//printf("  %d. Write Register											  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	switch(cmd)
    	{
    		case AD_FUNC_LIST:
				audioHandle.audioFunc->pFunList();
    		break;

    		case AD_PLAY_AUDIO:
    		{
    			audioHandle.audioFunc->pPlayAudio();
    		}
    		break;

			case AD_FUNC_READ:
			{
				u8 fid;
				u8 data;

				char	str[32];

				printf("\n  Please Input Function ID (Format:dd): ");

				scanf("\t %s", str);

				if(strlen(str) == AUDIO_FUN_LEN)	//	a(2)
				{
					fid = fnStrToINT(str, AUDIO_FUN_STR,  AUDIO_FUN_LEN);

					audioHandle.audioFunc->pFunRead(audioCard, fid, &data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;
			
			case AD_FUNC_WRITE:
			{
				u8 fid;
				u8 data;

				char	str[64];

				printf("\n  Please Inpu ID & Value(Format:iivvv, ex:14127, id:14, value:127): ");

				scanf("\t %s", str);

				if(strlen(str) == (AUDIO_FUN_LEN + AUDIO_FDATA_LEN))	//	i(2)+v(3)
				{
					fid  = fnStrToINT(str, AUDIO_FUN_STR,  AUDIO_FUN_LEN);

					data = fnStrToINT(str, AUDIO_DATA_STR,  AUDIO_FDATA_LEN);

					audioHandle.audioFunc->pFunWrite(audioCard, fid, data);
				}
				else
					printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			}
			break;

			//case AD_REG_READ:	//	Only Support PCM1681
			//{
			//	u8 reg;
			//	u8 data;
            //
			//	char	str[32];
            //
			//	if(audioCard == AC_WM8960)
            //
            //
			//	printf("\n  Please Input Register Address (Address:aa, ex:10, address:0x10): ");
            //
			//	scanf("\t %s", str);
            //
			//	if(strlen(str) == AUDIO_REG_LEN)	//	a(2)
			//	{
			//		reg = fnStrToHEX(str, AUDIO_REG_STR,  AUDIO_REG_LEN);
            //
			//		audioHandle.audioFunc->pRegRead(reg, &data);
            //
			//		printf("\n  ### hk_dbg, %s : read reg:0x%02x, data:0x%02x!\n", __FUNCTION__, reg, data);
			//	}
			//	else
			//		printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			//}
			//break;
			//
			//case AD_REG_WRITE:	//	Only Support PCM1681
			//{
			//	u8 reg;
			//	u8 data;
            //
			//	char	str[32];
            //
			//	printf("\n  Please Inpu Reigster & Data(Format:aadd, ex:1012, address:0x10, data:0x12): ");
            //
			//	scanf("\t %s", str);
            //
			//	if(strlen(str) == (AUDIO_REG_LEN + AUDIO_RDATA_LEN))	//	a(2)+d(2)
			//	{
			//		reg  = fnStrToHEX(str, AUDIO_REG_STR,  AUDIO_REG_LEN);
            //
			//		data = fnStrToHEX(str, AUDIO_DATA_STR,  AUDIO_RDATA_LEN);
            //
			//		printf("\n  ### hk_dbg, %s : write reg:0x%02x, data:0x%02x!\n", __FUNCTION__, reg, data);
            //
			//		audioHandle.audioFunc->pRegWrite(reg, data);
            //
			//		audioHandle.audioFunc->pRegRead(reg, &data);
            //
			//		printf("\n  ### hk_dbg, %s : read reg:0x%02x, data:0x%02x!\n", __FUNCTION__, reg, data);
			//	}
			//	else
			//		printf("\n  ### hk_dbg, %s : Format Error, Please try again!!\n", __FUNCTION__);
			//}
			//break;
			
			case AD_RETURN:
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
 * date: 2016/07/12
 * modify : hk
 * description: Process Audio Event
 */
u8 fnAudioProcess(u8 cmd)
{
	u8 result = HK_NA;

	switch(cmd)
	{
		case AC_CARD:
			result = fnAudioCardSelect();
		break;

		case AC_START:
			result = fnAudioStartTest();
		break;

		case AC_DBG:
			result = fnAudioDebug();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow audio command:%d, Please try again!!\n", __FUNCTION__, cmd);
	}

	return result;
}
//------------------------------------------------
