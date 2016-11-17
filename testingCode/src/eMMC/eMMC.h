#ifndef _EMMC_H_
#define _EMMC_H_

#include "common.h"

//	Stack Size
#define	EMMC_STACK_SIZE			0x100000

//	Sleep
#define EMMC_MILLISEC_BASE		(1000)
#define EMMC_MICROSEC_BASE		(1000000)
#define	EMMC_SLEEP_TIME			(10 * EMMC_MILLISEC_BASE)		//	10ms

//	EMMC Access Function Pointer
typedef struct
{
	u8 (*pInit)			(void);									//	Initial
	u8 (*pVerify)		(u8 mode);								//	Self Test
	u8 (*pFunRead)		(u8 mode, char *str);					//	Read Function
	u8 (*pFunWrite)		(u8 mode, char *str);					//	Write Function

}	EMMC_FUNC_T;

//	EMMC Device Handle
typedef struct
{
	EMMC_FUNC_T 	*eMMCFunc;									//	EMMC Access Function

}	EMMC_DEVICE_HANDLE;

typedef enum
{
	EC_START= 0x01,
	EC_DBG,														//	Debug
	EC_RETURN,
	
}	EMMC_COMMAND_T;

//	EMMC Debug Command
typedef enum
{
	ED_FUNC_READ = 0x01,
	ED_FUNC_WRITE,
	ED_RETURN,
	
}	EMMC_DEBUG_COMMAND_T;

//	Function
void fnEMMCInit(void);
u8 	 fnEMMCProcess(u8 cmd);

#endif	//	_EMMC_H_
