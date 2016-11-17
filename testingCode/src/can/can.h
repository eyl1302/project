#ifndef _CAN_H_	//	CAN (Controller Area Network)
#define _CAN_H_

#include "common.h"
#include "canComm.h"

//	Stack Size
#define	CAN_STACK_SIZE		0x100000

//	Sleep
#define CAN_MILLISEC_BASE	(1000)
#define CAN_MICROSEC_BASE	(1000000)
#define	CAN_SLEEP_TIME		(10 * CAN_MILLISEC_BASE)			//	10ms

//	CAN Access Function Pointer
typedef struct
{
	u8 (*pInit)			(void);									//	Initial
	u8 (*pTest)			(void);									//	Auto Test
	u8 (*pRead)			(u8 *buf);								//	Read
	u8 (*pWrite)		(u8 *buf);								//	Write

}	CAN_FUNC_T;

//	CAN Device Handle
typedef struct
{
	CAN_FUNC_T 	*canFunc;										//	can Access Function

}	CAN_DEVICE_HANDLE;

//	CAN Command
typedef enum
{
	NC_START = 0x01,
	NC_DBG,														//	Debug
	NC_RETURN,
	
}	CAN_COMMAND_T;

//	CAN Command
typedef enum
{
	ND_READ,
	ND_WRITE,
	ND_RETURN,
	
}	CAN_DEBUG_COMMAND_T;

//	Function
void fnCANInit(void);
u8 	 fnCANProcess(u8 cmd);

#endif	//	_CAN_H_
