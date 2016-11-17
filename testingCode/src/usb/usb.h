#ifndef _USB_H_
#define _USB_H_

#include "common.h"
#include "usbComm.h"

//	USB Device
#define	USB_DEVICE		"/dev/sda1"


//	Stack Size
#define	USB_STACK_SIZE			0x100000

//	Sleep
#define USB_MILLISEC_BASE		(1000)
#define USB_MICROSEC_BASE		(1000000)
#define	USB_SLEEP_TIME			(10 * USB_MILLISEC_BASE)		//	10ms


//	 USB Access Function Pointer
typedef struct
{
	u8 (*pInit)			(u8 uid);								//	Initial
	u8 (*pRead)			(u8 reg, u8 *data);						//	Read Regiser
	u8 (*pWrite)		(u8 reg, u8 data);						//	Write Register

}	USB_FUNC_T;

//	USB Device Handle
typedef struct
{
	USB_FUNC_T 		*usbFunc;									//	USB Access Function

}	USB_DEVICE_HANDLE;

//	USB Command
typedef enum
{
	UC_START = 0x01,
	UC_DBG,														//	Debug
	UC_RETURN,
	
}	USB_COMMAND_T;

//	 USB Debug Command
typedef enum
{
	UD_READ = 0x01,
	UD_WRITE,
	UD_RETURN,
	
}	USB_DEBUG_COMMAND_T;

//	Function
void fnUSBInit(void);
u8 	 fnUSBProcess(u8 cmd);

#endif	//	_CCM_H_
