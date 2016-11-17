#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"
#include "system.h"
#include "testingCodeVersion.h"
#include "queueVersion.h"
#include "rtcVersion.h"
#include "batteryVersion.h"
#include "audioVersion.h"
#include "gsensorVersion.h"
#include "sFlashVersion.h"
#include "sdramVersion.h"
#include "eMMCVersion.h"
#include "ccmVersion.h"
#include "usbVersion.h"
#include "tpanelVersion.h"
#include "pmicVersion.h"
#include "canVersion.h"

/*
 * date: 2016/06/24
 * modify : hk
 * description: Display Module Version
 */
void fnShowVersion(void)
{
	hk_dbg("### ================================================= ###\n");
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, TESTING_CODE_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, QUEUE_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, RTC_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, BATTERY_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, AUDIO_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, GSENSOR_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, SFLASH_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, SDRAM_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, EMMC_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, USB_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, TPANEL_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, PMIC_MODULE_VER_DEFINE);
	hk_dbg("### hk_dbg, %s, %s ###\n", __FUNCTION__, CAM_MODULE_VER_DEFINE);
	hk_dbg("### ================================================= ###\n\n");

	return;
}
//------------------------------------------------

/*
 * date: 2016/07/04
 * modify : hk
 * description: SIGNAL Process
 */
void fnSignalProcess(int dunno)
{
	exit(0);

	return;
}
//------------------------------------------------

/*
 * date: 2016/06/24
 * modify : hk
 * description: Main Function
 */
int main(int argc, char **argv)
{
	{
		u8 i;

		if(argc == 0)
			fnSystemMode(0);	//	0:user mode, 1:engineer mode
		else
		{
			for(i=0;i<argc;i++)
			{
				//printf("\n  ### hk_dbg, %s : %d, %s \n", __FUNCTION__, i, argv[i]);

				//	Version Check
				if(i==1)
				{
					int mode;

					sscanf(argv[i], "%d", &mode);

					fnSystemMode(mode);
				}
    		}
		}
	}

	//	Show Version
	fnShowVersion();

	//	System Initial
	fnSystemInit();

	while(1)
	{
		if(fnMenuMain())
			exit(1);

		sleep(1);
	}

	//	Signal Interrupt Process
	signal(SIGINT, fnSignalProcess);	//	Ctrl + C
	signal(SIGSEGV, fnSignalProcess);	//	Segmentation / Violation

	return 1;
}
//------------------------------------------------
