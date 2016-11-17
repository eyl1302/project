#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "common.h"
#include "audioQueue.h"

/********** Audio Register Format : aadd **********/
//	Audio Reigster Length
#define AUDIO_FUN_LEN			2
#define	AUDIO_REG_LEN			2
#define AUDIO_FDATA_LEN			3
#define AUDIO_RDATA_LEN			2

//	Audio Register Start Address
#define AUDIO_FUN_STR			0
#define	AUDIO_REG_STR			0
#define	AUDIO_DATA_STR			(AUDIO_REG_STR + AUDIO_REG_LEN)

/********** End of RTC Register Format **********/

//	Stack Size
#define	AUDIO_STACK_SIZE			0x100000

//	Sleep
#define AUDIO_MILLISEC_BASE		(1000)
#define AUDIO_MICROSEC_BASE		(1000000)
#define	AUDIO_SLEEP_TIME		(10 * AUDIO_MILLISEC_BASE)		//	10ms

//	Audio Access Function Pointer
typedef struct
{
	u8 (*pInit)			(u8 uid);								//	Initial
	u8 (*pFunList)		(void);									//	List Function
	u8 (*pPlayAudio)	(void);									//	Play Audio
	u8 (*pFunRead)		(u8 cid, u8 fid, u8 *data);				//	Read Function
	u8 (*pFunWrite)		(u8 cid, u8 fid, u8 data);				//	Write Function
	u8 (*pRegRead)		(u8 reg, u8 *data);						//	Read Regiser
	u8 (*pRegWrite)		(u8 reg, u8 data);						//	Write Register

}	AUDIO_FUNC_T;

//	Audio Device Handle
typedef struct
{
	AUDIO_FUNC_T 	*audioFunc;									//	Audio Access Function
	AUDIO_QUQEUE_T	audio;										//	Audio Queue

}	AUDIO_DEVICE_HANDLE;

//	Audio Command
typedef enum
{
	AC_CARD = 0x01,												//	Select Audio Card
	AC_START,
	AC_DBG,														//	Debug
	AC_RETURN,
	
}	AUDIO_COMMAND_T;

//	Audio Debug Command
typedef enum
{
	AD_FUNC_LIST = 0x01,
	AD_PLAY_AUDIO,
	AD_FUNC_READ,
	AD_FUNC_WRITE,
	//AD_REG_READ,
	//AD_REG_WRITE,
	AD_RETURN,
	
}	AUDIO_DEBUG_COMMAND_T;

//	Audio Code ID
typedef enum
{
	AC_PCM1681 = 0,
	AC_WM8960,

}	AUDIO_CARD_T;

//	Audio Card ID
#define CARD_STR(a)	\
		((a==AC_PCM1681)?"PCM1681":\
		((a==AC_WM8960)?"WM8960":"unknow audio card"))

//	Function
void fnAudioInit(void);
u8 	 fnAudioProcess(u8 cmd);

#endif	//	_AUDIO_H_
