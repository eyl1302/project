#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "system.h"
#include "usb.h"
#include "usbComm.h"

/*
 * date: 2016/09/26
 * modify : hk
 * description: USB Auto Test
 */
u8 fnUSBVerify(void)
{
	u8 		result;
	char 	cmd[128];
	char 	buf[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzzyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";
    
	//	Create USB Verify File
	if(fnSystemSaveTxt(buf, TXT_SFILE) == HK_FAIL)
		return HK_FAIL;
    
	//	Mount
	sprintf(cmd, "mount %s /mnt", USB_DEVICE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;
    
	//	Copy file to USB
	printf("\n  ### hk_dbg, %s : write file to usb......", __FUNCTION__);
    
	sprintf(cmd, "cp -f %s /mnt", TXT_SFILE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
	{
		printf("fail!!\n");
    
		result =  HK_FAIL;
	}
	else
	{
		printf("ok!!\n");

		result =  HK_SUCCESS;
	}	
    
	usleep(100);
    
	//	Read File
	printf("\n  ### hk_dbg, %s : copy file from usb......", __FUNCTION__);
    
	sprintf(cmd, "mv -f /mnt/%s ./%s", TXT_SFILE, TXT_CFILE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
	{
		printf("fail!!\n");
    
		result =  HK_FAIL;
	}
	else
	{
		printf("ok!!\n");

		result =  HK_SUCCESS;
	}
    
	//	Compare
	printf("\n  ### hk_dbg, %s : compare two files......", __FUNCTION__);
    
	sprintf(cmd, "diff ./%s ./%s", TXT_SFILE, TXT_CFILE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
	{
		printf("fail!!\n");
		
		result =  HK_FAIL;
	}
	else
	{
		printf("ok!!\n");
		
		result = HK_SUCCESS;
	}
    
	//	Umount
	sprintf(cmd, "umount /mnt");
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		result = HK_FAIL;
    
	//	Delete
	sprintf(cmd, "rm -f %s && rm -f %s", TXT_SFILE, TXT_CFILE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		result = HK_FAIL;

	return result;
}
//------------------------------------------------

/*
 * date: 2016/09/26
 * modify : hk
 * description: USB Single Read Test
 */
u8 fnUSBRead(char *str)
{
	u8		result;
    char	cmd[128];
    
    //	Mount
	sprintf(cmd, "mount %s /mnt", USB_DEVICE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;
    
	//	Read File
	printf("\n  ### hk_dbg, %s : copy file from usb......", __FUNCTION__);
    
	sprintf(cmd, "mv -f /mnt/%s ./", TXT_SFILE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
	{
		printf("fail!!\n");

		result = HK_FAIL;
	}	
	else
	{
		printf("ok!!\n");

		result = HK_SUCCESS;
	}	
    
	//	Read Data from File
	if(fnSystemReadTxt(str, TXT_SFILE) == HK_FAIL)
		result = HK_FAIL;
    
	//	Umount
	sprintf(cmd, "umount /mnt");
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		result = HK_FAIL;
    
	//	Remove Text File
	sprintf(cmd, "rm -f ./%s", TXT_SFILE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		result = HK_FAIL;
		
	//	Remove Text File
	sprintf(cmd, "rm -f ./%s", TXT_SFILE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		result = HK_FAIL;

	return result;
}
//------------------------------------------------

/*
 * date: 2016/09/26
 * modify : hk
 * description: USB Single Write Test
 */
u8 fnUSBWrite(char *str)
{
	u8		result;
	char 	cmd[128];
    
	//	Create USB Verify File
	if(fnSystemSaveTxt(str, TXT_SFILE) == HK_FAIL)
		return HK_FAIL;
    
	//	Mount
	sprintf(cmd, "mount %s /mnt", USB_DEVICE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		return HK_FAIL;
    
	//	Copy file to USB
	printf("\n  ### hk_dbg, %s : write file to usb......", __FUNCTION__);
    
	sprintf(cmd, "mv -f %s /mnt", TXT_SFILE);
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
	{
		printf("fail!!\n");

		result = HK_FAIL;
	}
	else
	{
		printf("ok!!\n");

		result = HK_SUCCESS;
	}
	//	Umount
	sprintf(cmd, "umount /mnt");
    
	if(fnSystemCommandVerify(cmd) == HK_FAIL)
		result = HK_FAIL;

	return result;
}
//------------------------------------------------
