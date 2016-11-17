#ifndef _PMIC_H_
#define _PMIC_H_

#include "common.h"
#include "pmicComm.h"

/********** Power IC Register Format : aadd **********/
//	PMIC Reigster Length
#define	PMIC_REG_LEN			2
#define PMIC_DATA_LEN			2

//	PMIC Register Start Address
#define	PMIC_REG_STR			0
#define	PMIC_DATA_STR			(PMIC_REG_STR + PMIC_REG_LEN)

/********** End of GSensor Register Format **********/

//	Stack Size
#define	PMIC_STACK_SIZE		0x100000

//	Sleep
#define PMIC_MILLISEC_BASE	(1000)
#define PMIC_MICROSEC_BASE	(1000000)
#define	PMIC_SLEEP_TIME		(10 * PMIC_MILLISEC_BASE)		//	10ms

//	PMIC Access Function Pointer
typedef struct
{
	u8 (*pInit)			(void);									//	Initial
	u8 (*pList)			(void);									//	List Current Setting
	u8 (*pConfig)		(void);									//	Config Current Setting
	u8 (*pTest)			(void);									//	Auto Test
	u8 (*pRegRead)		(u8 reg, u8 *data);						//	Read Regiser
	u8 (*pRegWrite)		(u8 reg, u8 data);						//	Write Register

}	PMIC_FUNC_T;

//	PMIC Device Handle
typedef struct
{
	PMIC_FUNC_T 	*pmicFunc;									//	PMIC Access Function

}	PMIC_DEVICE_HANDLE;

//	PMIC Command
typedef enum
{
	PC_START = 0x01,
	PC_DBG,														//	Debug
	PC_RETURN,
	
}	PMIC_COMMAND_T;

//	PMIC Command
typedef enum
{
	PD_LIST = 0x01,
	PD_CONFIG,
	PD_REG_READ,
	PD_REG_WRITE,
	PD_RETURN,
	
}	PMIC_DEBUG_COMMAND_T;

//	Function
void fnPMICInit(void);
u8 	 fnPMICProcess(u8 cmd);

#endif	//	_PMIC_H_
