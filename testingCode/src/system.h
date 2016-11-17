#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "common.h"

#define SCEEN_STR_LEN		1024

#define FUNCTION_VERIFY(a) \
		((a==HK_FAIL)?"FAIL":\
		((a==HK_SUCCESS)?"PASS":\
		((a==HK_NA)?"N/A":"Un-know")))

#define WORKING_MODE_CHECK(i)	\
				((i==0)?"USER-MODE":\
				((i==1)?"ENGR-MODE": "UNKNOW"))

#define DECIMAL2BCD(dec)		(((dec / 10) << 4) + (dec % 10)) 
#define BCD2DECIMAL(hex)	 	(((hex & 0xF0) >> 4) * 10 + (hex & 0x0F))

#define TXT_SFILE					"src.txt"
#define TXT_CFILE					"cmp.txt"

//	System Command
typedef enum
{
	SC_RTC = 0x01,
	SC_BATTERY,
	SC_AUDIO,
	SC_GSENSOR,
	SC_SFLASH,
	SC_SDRAM,
	SC_EMMC,
	SC_USB,
	SC_TPANEL,
	SC_PMIC,
	SC_CAN,
	SC_ALL,
	SC_EXIT,

}	SYSTEM_COMMAND_T;

//	Function Verify
typedef struct
{
	u8 rtc;
	u8 battery;
	u8 audio;
	u8 gsensor;
	u8 sFlash;
	u8 sdram;
	u8 emmc;
	u8 usb;
	u8 tpanel;
	u8 pmic;
	u8 can;
	u8 ccm;
	u8 all;

}	FUNCTION_VERIFY_T;

//	Function
void fnSystemInit(void);
void fnSystemMode(u8 mode);
u32  fnSystemInputString(void);
void fnSystemCmdProcess(u8 cmd);
u8   fnSystemCommandVerify(char *cmd);
u8   fnSystemSaveTxt(char *buf, char *bFile);
u8   fnSystemReadTxt(char *buf, char *bFile);

u8 	 fnMenuMain(void);

void fnStrToSub(const char* src, char *dest, u8 start, u8 cnt);
u32  fnStrToINT(const char *str, u8 start, u8 cnt);
u8   fnStrToBCD(const char *str, u8 start, u8 cnt);
u32  fnStrToHEX(const char *str, u8 start, u8 cnt);


#endif // _SYSTEM_H_
