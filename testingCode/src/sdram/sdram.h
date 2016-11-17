#ifndef _SDRAM_H_
#define _SDRAM_H_

#include "common.h"

//	Stack Size
#define	SDRAM_STACK_SIZE		0x100000

//	Sleep
#define SDRAM_MILLISEC_BASE		(1000)
#define SDRAM_MICROSEC_BASE		(1000000)
#define	SDRAM_SLEEP_TIME		(10 * SDRAM_MILLISEC_BASE)		//	10ms


//	SDRAM Access Function Pointer
typedef struct
{
	u8 (*pInit)			(u8 mode);								//	Initial
	u8 (*pVerify)		(u64 start, u64 end);					//	Self Test
	u8 (*pFunRead)		(u64 start, u32 *data);					//	Read Function
	u8 (*pFunWrite)		(u64 start, u32 data);					//	Write Function

}	SDRAM_FUNC_T;

//	SDRAM Device Handle
typedef struct
{
	SDRAM_FUNC_T 	*sdramFunc;									//	SDRAM Access Function
	//SDRAM_QUQEUE_T	audio;									//	Audio Queue

}	SDRAM_DEVICE_HANDLE;

typedef enum
{
	DC_START= 0x01,
	DC_DBG,														//	Debug
	DC_RETURN,
	
}	SDRAM_COMMAND_T;

//	SDRAM Debug Command
typedef enum
{
	DD_FUNC_READ = 0x01,
	DD_FUNC_WRITE,
	DD_RETURN,
	
}	SDRAM_DEBUG_COMMAND_T;

//	Verify Mode
typedef enum
{
	VM_QUICK = 0,				//	Quick Test
	VM_ALTERNATIVE,				//	Alternative Test

} VERIFY_MODE_T;

//	Function
void fnSDRAMInit(void);
u8 	 fnSDRAMProcess(u8 cmd);

#endif	//	_SDRAM_H_
