#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "i2c-dev.h"
#include "common.h"
#include "system.h"
#include "pcm1681.h"
#include "audioComm.h"

//	Variable
u8	pcm1681Init = 0; // 1:Initial
u8  pcm1681Play = 0; // 0:Stop, 1:Play

//	Function
u8	fnPCM1681Init(u8 cid);
u8	fnPCM1681FunList(void);
u8	fnPCM1681PlayAudio(void);
u8  fnPCM1681FunRead(u8 cid, u8 fid, u8 *data);
u8  fnPCM1681FunWrite(u8 cid, u8 fid, u8 data);
u8  fnPCM1681RegRead(u8 reg, u8 *data);
u8  fnPCM1681RegWrite(u8 reg, u8 data);

//	PCM1681 RTC Access Function
AUDIO_FUNC_T pcm1681 =
{
	fnPCM1681Init,		//	Initial
	fnPCM1681FunList,	//	List Function
	fnPCM1681PlayAudio,	//	Play Audio
	fnPCM1681FunRead,	//	Read Function
	fnPCM1681FunWrite,	//	Write Function
	fnPCM1681RegRead,	//	Read Function
	fnPCM1681RegWrite,	//	Write Function
};

/*
 * date: 2016/08/12
 * modify : hk
 * description: Initial WM8960 Function
 */
u8 fnPCM1681Init(u8 cid)
{
	char cmd[64];

	if(pcm1681Init)
		return HK_SUCCESS;

	//	Enable MAINIKIN-5V
	sprintf(cmd, "echo 1 >/sys/class/power_supply/MANIKIN_5V/online");

	system(cmd);

	//	Enable MAINIKIN-5V
	sprintf(cmd, "echo 1 >/sys/class/power_supply/MANIKIN_12V/online");

	system(cmd);

	pcm1681Init = 1;

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/01
 * modify : hk
 * description: List pcm1681 Function
 */
u8 fnPCM1681FunList(void)
{
	printf("\n");
	printf(  "|====|========================================|========|=======|\n");
	printf(  "|                     PCM1681 Function List                    |\n");
	printf(  "|====|========================================|========|=======|\n");
	printf(  "| ID |                Name                    | Access | value |\n");
	printf(  "|====|========================================|========|=======|\n");
	printf(  "| 01 | Channel 1/2 Playback Volume            |   rw   | 0~127 |\n");
	printf(  "| 02 | Channel 3/4 Playback Volume            |   rw   | 0~127 |\n");
	printf(  "| 03 | Channel 5/6 Playback Volume            |   rw   | 0~127 |\n");
	printf(  "| 04 | Channel 7/8 Playback Volume            |   rw   | 0~127 |\n");
	printf(  "| 05 | De-emphasis Switch                     |   rw   | 0/1   |\n");
	printf(  "|====|========================================|========|=======|\n");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/01
 * modify : hk
 * description: Play Audio Function
 */
u8 fnPCM1681PlayAudio(void)
{
	char cmd[64];

	pcm1681Play = ~pcm1681Play;

	sprintf(cmd, "aplay -D hw:%d,0 audio.wav &", AC_PCM1681);

	//if(pcm1681Play)
	//	sprintf(cmd, "aplay -D hw:%d,0 audio.wav &", AC_PCM1681);
	//else
	//	sprintf(cmd, "killall aplay");

	printf("\n");

	system(cmd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/01
 * modify : hk
 * description: Read Function
 */
u8 fnPCM1681FunRead(u8 cid, u8 fid, u8 *data)
{
	char cmd[32];

	if((fid < PCM1681_CFI_START)||(fid > PCM1681_CFI_END))
	{
		printf("\n  ### hk_dbg, %s : Out off range, Please try agagin!\n", __FUNCTION__);

		return HK_FAIL;
	}

	printf("\n");

	AMIXER_CGET(cmd, cid, fid);

	system(cmd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/01
 * modify : hk
 * description: Write Function
 */
u8 fnPCM1681FunWrite(u8 cid, u8 fid, u8 data)
{
	char cmd[32];

	if((fid < PCM1681_CFI_START)||(fid > PCM1681_CFI_END))
	{
		printf("\n  ### hk_dbg, %s : Out off range, Please try agagin!\n", __FUNCTION__);

		return HK_FAIL;
	}

	printf("\n");

	AMIXER_CSET(cmd, cid, fid, data);

	system(cmd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/08
 * modify : hk
 * description: Read Register Function
 */
u8  fnPCM1681RegRead(u8 reg, u8 *data)
{
	u8 buf[32];

	if(fnAudioRead(PCM1681_I2C_BUS, PCM1681_I2C_ADDRESS, reg, buf, 1) == HK_FAIL)
		return HK_FAIL;

	*data = buf[0];

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/08/08
 * modify : hk
 * description: Write Register Function
 */
u8  fnPCM1681RegWrite(u8 reg, u8 data)
{
	u8 buf[32];

	buf[0] = data;

	if(fnAudioWrite(PCM1681_I2C_BUS, PCM1681_I2C_ADDRESS, reg, buf, 1) == HK_FAIL)
		return HK_FAIL;

	return HK_SUCCESS;
}
//------------------------------------------------
