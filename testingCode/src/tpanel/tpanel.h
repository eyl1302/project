#ifndef _TPANEL_H_
#define _TPANEL_H_

#include "common.h"

/********** Touch Panel Register Format : aadd **********/
//	TPL Reigster Length
#define	TPL_REG_LEN				2
#define TPL_DATA_LEN			2

//	TPL Register Start Address
#define	TPL_REG_STR				0
#define	TPL_DATA_STR			(TPL_REG_STR + TPL_REG_LEN)

/********** End of GSensor Register Format **********/

//	Stack Size
#define	TPANEL_STACK_SIZE		0x100000

//	Sleep
#define TPANEL_MILLISEC_BASE	(1000)
#define TPANEL_MICROSEC_BASE	(1000000)
#define	TPANEL_SLEEP_TIME		(10 * TPANEL_MILLISEC_BASE)		//	10ms

//	Touch Panel Access Function Pointer
typedef struct
{
	u8 (*pInit)			(void);									//	Initial
	u8 (*pTest)			(void);									//	Auto Test
	u8 (*pRegRead)		(u8 reg, u8 *data);						//	Read Regiser
	u8 (*pRegWrite)		(u8 reg, u8 data);						//	Write Register

}	TPANEL_FUNC_T;

//	Touch Panel Device Handle
typedef struct
{
	TPANEL_FUNC_T 	*tpanelFunc;								//	Touch Panel Access Function

}	TPANEL_DEVICE_HANDLE;

//	Touch Panel Command
typedef enum
{
	TC_START = 0x01,
	TC_DBG,														//	Debug
	TC_RETURN,
	
}	TPANEL_COMMAND_T;

//	Touch Panel Command
typedef enum
{
	TD_REG_READ = 0x01,
	//TD_REG_WRITE,
	TD_RETURN,
	
}	TPANEL_DEBUG_COMMAND_T;

//	Function
void fnTPanelInit(void);
u8 	 fnTPanelProcess(u8 cmd);

#endif	//	_AUDIO_H_
