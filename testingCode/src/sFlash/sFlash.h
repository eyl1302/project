#ifndef _SFLASH_H_
#define _SFLASH_H_

#include "common.h"

//	Stack Size
#define	SFLASH_STACK_SIZE		0x100000

//	Sleep
#define SFLASH_MILLISEC_BASE	(1000)
#define SFLASH_MICROSEC_BASE	(1000000)
#define	SFLASH_SLEEP_TIME		(10 * SFLASH_MILLISEC_BASE)		//	10ms

//	Serial Flash Command
typedef enum
{
	SC_START = 0x01,
	SC_DBG,
	SC_RETURN,
	
}	SERIAL_FLASH_COMMAND_T;

//	Serial Flash Debug Command
typedef enum
{
	SD_INFO = 0x1,
	SD_READ,
	SD_WRITE,
	SD_ERASE,
	SD_RETURN,

}	SERIAL_FLASH_DEBUG_COMMAND_T;

//	Serila Flash Access Function Pointer
typedef struct
{
	u8 (*pInfo)		(void);							//	Display Information
	u8 (*pVerify)	(void);							//	Verify Function
	u8 (*pRead)		(void);							//	Read Function
	u8 (*pWrite)	(void);							//	Write Function
	u8 (*pErase)	(void);							//	Erase Function

}	SFLASH_FUNC_T;

// Serial Flash Handler
typedef struct
{
	SFLASH_FUNC_T *sflashFunc;

}	SFLASH_DEVICE_HANDLE;

void fnSFlashInit(void);
u8 	 fnSFlashProcess(u8 cmd);

#endif	//_SFLASH_H_
