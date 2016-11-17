#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "system.h"
#include "rtcQueue.h"
#include "rtc.h"
#include "battery.h"
#include "audio.h"
#include "gsensor.h"
#include "sFlash.h"
#include "sdram.h"
#include "eMMC.h"
#include "ccm.h"
#include "usb.h"
#include "tpanel.h"
#include "pmic.h"
#include "can.h"

#include "testingCodeVersion.h"
#include "systemQueue.h"
#include "rtcQueue.h"
#include "batteryQueue.h"
#include "audioQueue.h"
#include "gsensorQueue.h"
#include "sflashQueue.h"
#include "sdramQueue.h"
#include "emmcQueue.h"
#include "usbQueue.h"
#include "tpanelQueue.h"
#include "pmicQueue.h"
#include "canQueue.h"

//	Variable
u8	autoTest = 0;				//	0: Finish, 1:Start
u8	autoTestIdx = 0;			//	SYSTEM_COMMAND_T
u8 	sysCmd;
u8 	sysMode;					//	0:user mode, 1:engineer mode

//	Function
void fnSystemTimerCreate(void);
void fnSystemTimerISR(int a);

//	Variable
FUNCTION_VERIFY_T func;

//	Function
void fnMenuRTC(void);
void fnMenuBattery(void);
void fnMenuAudio(void);
void fnMenuGSensor(void);
void fnMenuSFlash(void);
void fnMenuSDRAM(void);
void fnMenuEMMC(void);
void fnMenuUSB(void);
void fnMenuTPanel(void);
void fnMenuPMIC(void);
void fnMenuCAN(void);
void fnMenuCCM(void);
void fnMenuTestAll(void);

/*
 * date: 2016/07/04
 * modify : hk
 * description: System Timer Create
 */
void fnSystemTimerCreate(void)
{
	struct itimerval t;

	t.it_interval.tv_usec = 500;	//	Waiting for 500us, Trigger Timer
	t.it_interval.tv_sec = 0;
	t.it_value.tv_usec = 500;
	t.it_value.tv_sec = 0;		//	Trigger Timer Every 1s

	if(setitimer(ITIMER_REAL, &t, NULL) < 0)
	{
		hk_dbg("### hk_dbg, %s : Set System Timer Error\n", __FUNCTION__);

		return;
	}

	signal(SIGALRM, fnSystemTimerISR);

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/04
 * modify : hk
 * description: System Timer Interrupt Service Routine
 */
void fnSystemTimerISR(int a)
{
	SYSTEM_QUQEUE_T sys;

	//	System Command
	if(fnSystemQueuePop(&sys) == HK_SUCCESS)
	{
		sysCmd = sys.cmd;

		switch(sysCmd)
		{
			case SC_AUTOTEST:
			{
				autoTestIdx = SC_RTC;

				{
					RTC_QUQEUE_T rtc;

					rtc.cmd = RQ_AUTOTEST;

					fnRTCQueuePush(rtc);
				}
			}
			break;

			case SC_REPLY:				//	Process Reply Command
			{
				if(autoTest == 1)
				{
					//	Check Reply
					switch(sys.func)
					{
						case SC_RTC:
							func.rtc = sys.reply;
							autoTestIdx++;
						break;

						case SC_BATTERY:
							func.battery = sys.reply;
							autoTestIdx++;
						break;

						case SC_AUDIO:
							func.audio = sys.reply;
							autoTestIdx++;
						break;

						case SC_GSENSOR:
							func.gsensor = sys.reply;
							autoTestIdx++;
						break;

						case SC_SFLASH:
							func.sFlash = sys.reply;
							autoTestIdx++;
						break;

						case SC_SDRAM:
							func.sdram = sys.reply;
							autoTestIdx++;
						break;

						case SC_EMMC:
							func.emmc = sys.reply;
							autoTestIdx++;
						break;

						case SC_USB:
							func.usb = sys.reply;
							autoTestIdx++;
						break;

						case SC_TPANEL:
							func.tpanel = sys.reply;
							autoTestIdx++;
						break;

						case SC_PMIC:
							func.pmic = sys.reply;
							autoTestIdx++;
						break;

						case SC_CAN:
							func.can = sys.reply;
							autoTestIdx++;
						break;

						default:
							printf("\n  ### hk_dbg, %s : unknow function:%d!!\n", __FUNCTION__, sys.func);
							return;
					}

					//	Launch Next Function Test
					switch(autoTestIdx)
					{
						case SC_RTC:
						{
							RTC_QUQEUE_T rtc;

							rtc.cmd = RQ_AUTOTEST;

							fnRTCQueuePush(rtc);
						}
						break;

						case SC_BATTERY:
						{
							BATTERY_QUQEUE_T bty;

							bty.cmd = BQ_AUTOTEST;

							fnBatteryQueuePush(bty);
						}
						break;

						case SC_AUDIO:
						{
							AUDIO_QUQEUE_T audio;

							audio.cmd = AQ_AUTOTEST;

							fnAudioQueuePush(audio);
						}
						break;

						case SC_GSENSOR:
						{
							GSENSOR_QUQEUE_T gsensor;

							gsensor.cmd = GQ_AUTOTEST;

							fnGSensorQueuePush(gsensor);
						}
						break;

						case SC_SFLASH:
						{
							SFLASH_QUQEUE_T sflash;

							sflash.cmd = SQ_AUTOTEST;

							fnSFlashQueuePush(sflash);
						}
						break;

						case SC_SDRAM:
						{
							SDRAM_QUQEUE_T sdram;

							sdram.cmd = DQ_AUTOTEST;

							fnSDRAMQueuePush(sdram);
						}
						break;

						case SC_EMMC:
						{
							EMMC_QUQEUE_T emmc;

							emmc.cmd = EQ_AUTOTEST;

							fnEMMCQueuePush(emmc);
						}
						break;

						case SC_USB:
						{
							USB_QUQEUE_T usb;

							usb.cmd = UQ_AUTOTEST;

							fnUSBQueuePush(usb);
						}
						break;

						case SC_TPANEL:
						{
							TPANEL_QUQEUE_T tpanel;

							tpanel.cmd = TQ_AUTOTEST;

							fnTPanelQueuePush(tpanel);
						}
						break;

						case SC_PMIC:
						{
							PMIC_QUQEUE_T pmic;

							pmic.cmd = PQ_AUTOTEST;

							fnPMICQueuePush(pmic);
						}
						break;

						case SC_CAN:
						{
							CAN_QUQEUE_T can;

							can.cmd = CQ_AUTOTEST;

							fnCANQueuePush(can);
						}
						break;

						default:
							autoTest = 0;
							autoTestIdx = 0;

							printf("\n  ### hk_dbg, %s : Autotest Finish!!!\n", __FUNCTION__);
						break;
					}
				}
			}
			break;
		}
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/06/24
 * modify : hk
 * description: System Initial
 */
void fnSystemInit(void)
{
	//	Initial System Queue
	fnSystemQueueInit();

	//	RTC Init
	fnRTCInit();

	//	Battery Init
	fnBatteryInit();

	//	Audio Init
	fnAudioInit();

	//	Gsensor Init
	fnGSensorInit();

	//	SPI Flash Init
	fnSFlashInit();

	//	SDRAM Init
	fnSDRAMInit();

	//	eMMC Init
	fnEMMCInit();

	// 	USB Init
	fnUSBInit();

	//	Touch Panel Init
	fnTPanelInit();

	//	PMIC Init
	fnPMICInit();

	//	CAN Init
	fnCANInit();

	//	System Timer
	fnSystemTimerCreate();

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/21
 * modify : hk
 * description: System Mode
 */
void fnSystemMode(u8 mode)
{
	sysMode = mode;

	printf("\n  ### hk_dbg, %s : working mode:%s\n", __FUNCTION__, WORKING_MODE_CHECK(sysMode));

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/04
 * modify : hk
 * description: Get Interger
 */
u32 fnSystemInputString(void)
{
	u16 len = 0;
	u32 val = 0;
	char str[SCEEN_STR_LEN];

	//	Get String
	len = scanf("\t %s", str);

	//printf("### hk_dbg, %s : len:%d\n", __FUNCTION__, len);

	len = strlen(str);

	//	Convert to Interger
	if(len <= SCEEN_STR_LEN)
		sscanf(str, "%d", &val);

	return val;
}
//-----------------------------------------------------

/*
 * date: 2016/07/04
 * modify : hk
 * description: System Command Process
 */
void fnSystemCmdProcess(u8 cmd)
{
	switch(cmd)
	{
		case SC_RTC:
			fnMenuRTC();
		break;
		
		case SC_BATTERY:
			fnMenuBattery();
		break;

		case SC_AUDIO:
			fnMenuAudio();
		break;

		case SC_GSENSOR:
			fnMenuGSensor();
		break;

		case SC_SFLASH:
			fnMenuSFlash();
		break;

		case SC_SDRAM:
			fnMenuSDRAM();
		break;

		case SC_EMMC:
			fnMenuEMMC();
		break;

		case SC_USB:
			fnMenuUSB();
		break;

		case SC_TPANEL:
			fnMenuTPanel();
		break;

		case SC_PMIC:
			fnMenuPMIC();
		break;

		case SC_CAN:
			fnMenuCAN();
		break;

		case SC_ALL:
			fnMenuTestAll();
		break;

		default:
			hk_dbg("\n  ### hk_dbg, %s : unknow system command:%d, Please try again!\n", __FUNCTION__, cmd);
	}

	return;
}
//-----------------------------------------------------

/*
 * date: 2016/07/04
 * modify : hk
 * description: Display RTC Menu
 */
void fnMenuRTC(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  =======================  RTC Testing =======================\r\n\n");
    	printf("  %d. Start to Test RTC											  \r\n\n", idx++);
   		printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == RC_RETURN)
    		return;

    	func.rtc = fnRTCProcess(cmd);

    	sleep(1);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/11
 * modify : hk
 * description: Display Battery Menu
 */
void fnMenuBattery(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  =====================  Battery Testing =====================\r\n\n");
    	printf("  %d. Start to Test Battery										  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == BC_RETURN)
    		return;

    	func.battery = fnBatteryProcess(cmd);

    	sleep(1);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Display Audio Menu
 */
void fnMenuAudio(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================  Audio Testing ======================\r\n\n");
		printf("  %d. Select Audio Card     									  \r\n\n", idx++);
    	printf("  %d. Start to Test Audio										  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == AC_RETURN)
    		return;

    	func.audio = fnAudioProcess(cmd);

    	sleep(1);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/15
 * modify : hk
 * description: Display GSensor Menu
 */
void fnMenuGSensor(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  =====================  GSensor Testing =====================\r\n\n");
    	printf("  %d. Start to Test GSensor										  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == GC_RETURN)
    		return;
        
    	func.gsensor = fnGSensorProcess(cmd);

    	sleep(1);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/22
 * modify : hk
 * description: Display Serial Flash Menu
 */
void fnMenuSFlash(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ===================== SPI Flash Testing ====================\r\n\n");
    	printf("  %d. Start to Test SPI Flash	     							  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == SC_RETURN)
    		return;
        
    	func.sFlash = fnSFlashProcess(cmd);

    	sleep(1);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/08/24
 * modify : hk
 * description: Display DDR Menu
 */
void fnMenuSDRAM(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================= SDRAM Testing ======================\r\n\n");
    	printf("  %d. Start to Test SDRAM        								  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == DC_RETURN)
    		return;
        
    	func.sdram = fnSDRAMProcess(cmd);

    	sleep(1);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/13
 * modify : hk
 * description: Display eMMC Menu
 */
void fnMenuEMMC(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================= eMMC Testing ======================\r\n\n");
    	printf("  %d. Start to Test eMMC        								  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == EC_RETURN)
    		return;
        
    	func.emmc = fnEMMCProcess(cmd);

    	sleep(1);
	}

	return;
}
//------------------------------------------------

/*
 * date: 2016/09/20
 * modify : hk
 * description: Display CCM Menu
 */
void fnMenuCCM(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================= Camera Testing =====================\r\n\n");
    	printf("  %d. Start to Test Camera       								  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == CC_RETURN)
    		return;
        
    	//func.ccm = fnCCMProcess(cmd);

    	sleep(1);
	}
}
//------------------------------------------------

/*
 * date: 2016/09/23
 * modify : hk
 * description: Display USB Menu
 */
void fnMenuUSB(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================== USB Testing =======================\r\n\n");
    	printf("  %d. Start to Test USB    								  		  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == UC_RETURN)
    		return;
        
    	func.usb = fnUSBProcess(cmd);

    	sleep(1);
	}
}
//------------------------------------------------

/*
 * date: 2016/10/04
 * modify : hk
 * description: Display Touch Panel Menu
 */
void fnMenuTPanel(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ==================== Touch Panel Testing ===================\r\n\n");
    	printf("  %d. Start to Test Touch Panel							  		  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == TC_RETURN)
    		return;
        
    	func.tpanel = fnTPanelProcess(cmd);

    	sleep(1);
	}
}
//------------------------------------------------

/*
 * date: 2016/10/04
 * modify : hk
 * description: Display PMIC Menu
 */
void fnMenuPMIC(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================= PMIC Testing =======================\r\n\n");
    	printf("  %d. Start to Test PMIC	    						  		  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == PC_RETURN)
    		return;
        
    	func.pmic = fnPMICProcess(cmd);

    	sleep(1);
	}
}
//------------------------------------------------

/*
 * date: 2016/10/20
 * modify : hk
 * description: Display Test All Menu
 */
void fnMenuCAN(void)
{
	u8	idx;
	u32 cmd;

	while(1)
	{
		idx = 1;

		printf("\r\n  ======================== CAN Testing =======================\r\n\n");
    	printf("  %d. Start to Test CAN	        						  		  \r\n\n", idx++);
    	printf("  %d. Debug Mode												  \r\n\n", idx++);
    	printf("  %d. Return  													  \r\n\n", idx);
    	printf("  Please Enter Command(1 ~ %d): ", idx);

    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd:%d\n", __FUNCTION__, cmd);

    	if(cmd == NC_RETURN)
    		return;
        
    	func.can = fnCANProcess(cmd);

    	sleep(1);
	}
}
//------------------------------------------------

/*
 * date: 2016/07/12
 * modify : hk
 * description: Display Test All Menu
 */
void fnMenuTestAll(void)
{
	//	push to queue
	SYSTEM_QUQEUE_T sys;

	sys.cmd = SC_AUTOTEST;

	fnSystemQueuePush(sys);

	//	Start to Autotest
	autoTest = 1;

	//	Wait for Autotest Finish
	while(autoTest)
		sleep(1);

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/04
 * modify : hk
 * description: Display Main Menu
 */
u8 fnMenuMain(void)
{
	u8	idx;
	u32 cmd;

	//	Set Verify Function to Default
	memset(&func, 0xff, sizeof(FUNCTION_VERIFY_T));

	while(1)
	{
		idx = 1;

		func.all = func.rtc & func.battery & func.audio & func.gsensor & func.sFlash & func.sdram & func.emmc & func.usb & func.tpanel & func.pmic & func.can;

		printf("  =====================================================================\r\n");
		printf("  =                           %s                 =\r\n",TESTING_CODE_MODULE_VER_DEFINE);
		printf("  =====================================================================\r\n\n");
   		printf("  %d. RTC ----------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.rtc));
    	printf("  %d. Battery ------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.battery));
    	printf("  %d. Audio --------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.audio));
    	printf("  %d. GSensor ------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.gsensor));
    	printf("  %d. SPI Flash ----------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.sFlash));
    	printf("  %d. SDRAM --------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.sdram));
    	printf("  %d. eMMC ---------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.emmc));
    	printf("  %d. USB ----------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.usb));
    	printf("  %d. Touch Panel --------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.tpanel));
    	printf("  %d. PMIC ---------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.pmic));
    	printf("  %d. CAN ----------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.can));
    	printf("  %d. All ----------------------------------------------------- %s\r\n\n", idx++, FUNCTION_VERIFY(func.all));
    	printf("  %d. Exit   											  \r\n\n", idx);
    	printf("  Please Select Number(1 ~ %d) and Press Enter Key to Test: ", idx);
        
    	cmd = fnSystemInputString();

    	//hk_dbg("### hk_dbg, %s : cmd::%d\n", __FUNCTION__, cmd);
        
    	if(cmd == SC_EXIT)
    	{
    		printf("\n  Thank You, Goodbye!! \n\n");
        
    		return 1;
    	}

		//	Process Command
    	fnSystemCmdProcess(cmd);

    	sleep(1);
	}

	return 0;
}
//-----------------------------------------------------

/*
 * date: 2016/07/07
 * modify : hk
 * description: RTC Get Sub-String
 */
void fnStrToSub(const char* src, char *dest, u8 start, u8 cnt)
{
	strncpy(dest, src + start, cnt);

  	dest[cnt] = 0;

  	return;
}
//------------------------------------------------

/*
 * date: 2016/07/07
 * modify : hk
 * description: RTC Get Sub-String and conver to Interger
 */
u32  fnStrToINT(const char *str, u8 start, u8 cnt)
{
	u32 	data;
	char	sub[8];

	//	Get Substring	
	fnStrToSub(str, sub, start, cnt);

	//hk_dbg("### hk_dbg, %s : substring:%s\n", __FUNCTION__, sub);

	//	Convert String to Interger
	sscanf(sub, "%d", &data);

	//hk_dbg("### hk_dbg, %s : data:%d!\n", __FUNCTION__, data);

  	return data;
}
//------------------------------------------------

/*
 * date: 2016/07/07
 * modify : hk
 * description: RTC Get Sub-String and conver to BCD
 */
u8  fnStrToBCD(const char *str, u8 start, u8 cnt)
{
	u32 data;

	//	Conver String to Interger
	data = fnStrToINT(str, start, cnt);

	//	Convert Integer to BCD
	data = DECIMAL2BCD(data);

	//hk_dbg("### hk_dbg, %s : data:%02x\n", __FUNCTION__, data);

	return data;
}
//------------------------------------------------

/*
 * date: 2016/07/07
 * modify : hk
 * description: RTC Get Sub-String and conver to HEX
 */
u32  fnStrToHEX(const char *str, u8 start, u8 cnt)
{
	u32 	data;
	char	sub[8];

	//	Get Substring	
	fnStrToSub(str, sub, start, cnt);

	//hk_dbg("### hk_dbg, %s : substring:%s\n", __FUNCTION__, sub);

	//	Convert String to Interger
	sscanf(sub, "%x", &data);

	//hk_dbg("### hk_dbg, %s : data:%d!\n", __FUNCTION__, data);

  	return data;
}
//------------------------------------------------

/*
 * date: 2016/09/20
 * modify : hk
 * description: Return System Commnad Result
 */
u8 fnSystemCommandVerify(char *cmd)
{
	s32 flag;

	flag = system(cmd);

	if(flag < 0)
	{
	    printf("\n  ### hk_dbg, %s : cmd:'%s'!! \n", __FUNCTION__, cmd);

	    return HK_FAIL;
	}
    
	if(!WIFEXITED(flag))
	{
		printf("\n  ### hk_dbg, %s : cmd:'%s' abnormal termination!! \n", __FUNCTION__, cmd);
		
		return HK_FAIL;
	}	

	if(WEXITSTATUS(flag))
	{
		printf("\n  ### hk_dbg, %s : cmd:'%s' not succeed(%d)!! \n", __FUNCTION__, cmd, WEXITSTATUS(flag));
		
		return HK_FAIL;
	}

	//printf("\n  ### hk_dbg, %s : cmd: '%s' normal termination(%d)!! \n", __FUNCTION__, cmd, WEXITSTATUS(flag));

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/20
 * modify : hk
 * description: Create Text File and Verify eMMC Access
 */
u8 fnSystemSaveTxt(char *buf, char *bFile)
{
	u8		len;
	FILE	*file;

	len =  strlen(buf);

	if(!len)
		return HK_FAIL;

	file = fopen(bFile, "w+");

	if(file == NULL)
	{
		printf("\n  ### hk_dbg, %s : Open file:%s Fail~\n", __FUNCTION__, bFile);

		return HK_FAIL;
	}

	if(fwrite(buf, sizeof(u8), len, file) != len)
	{
		printf("\n  ### hk_dbg, %s : Save to %s Fail~ \n", __FUNCTION__, bFile);

		fclose(file);

		return HK_FAIL;
	}

	fclose(file);

	return HK_SUCCESS;
}
//-----------------------------------------------------

/*
 * date: 2016/09/21
 * modify : hk
 * description: Read from Text File
 */
u8 fnSystemReadTxt(char *buf, char *bFile)
{
	u16		len;
	FILE	*file;

	file = fopen(bFile, "r");

	if(file == NULL)
	{
		printf("\n  ### hk_dbg, %s : line:%d,  Read Fil Fail~~\n", __FUNCTION__, __LINE__);

		return HK_FAIL;
	}

	//	Sets the position to the end of the stream
	fseek(file, 0, SEEK_END);   // non-portable

	//	Get File Size
    len = ftell(file);

    //	Set the positionto the beginning of the stream
    fseek(file, 0, SEEK_SET);

    //printf("\n  ### hk_dbg, %s : file size :%d!!\n", __FUNCTION__, len);

	if(fread(buf, 1, len, file) != len)
	{
		printf("\n  ### hk_dbg, %s : Empty Data~~\n", __FUNCTION__);

		fclose(file);

		return HK_FAIL;
	}

	fclose(file);

	return HK_SUCCESS;
}
//-----------------------------------------------------
