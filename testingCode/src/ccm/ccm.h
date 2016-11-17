#ifndef _CCM_H_	//	CMOS Camera Module
#define _CCM_H_

#include "common.h"

//	 CCM Access Function Pointer
typedef struct
{
	u8 (*pInit)			(u8 uid);								//	Initial
	u8 (*pRegRead)		(u8 reg, u8 *data);						//	Read Regiser
	u8 (*pRegWrite)		(u8 reg, u8 data);						//	Write Register

}	CCM_FUNC_T;

//	CCM Device Handle
typedef struct
{
	CCM_FUNC_T 		*ccmFunc;									//	CCM Access Function

}	CCM_DEVICE_HANDLE;

//	CCM Command
typedef enum
{
	CC_START,
	CC_DBG,														//	Debug
	CC_RETURN,
	
}	CCM_COMMAND_T;

//	CCM Debug Command
typedef enum
{
	CD_REG_READ,
	CD_REG_WRITE,
	CD_RETURN,
	
}	CCM_DEBUG_COMMAND_T;

//	Function
void fnCCMInit(void);
u8 	 fnCCMProcess(u8 cmd);

#endif	//	_CCM_H_
