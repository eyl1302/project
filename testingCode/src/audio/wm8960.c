
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
#include "wm8960.h"

//	Variable
u8	wm8960Init = 0; // 1:Initial
u8  wm8960Play = 0; // 0:Stop, 1:Play

//	Function
u8	fnWM8960Init(u8 cid);
u8	fnWM8960PlayAudio(void); 
u8	fnWM8960FunList(void);
u8  fnWM8960FunRead(u8 cid, u8 fid, u8 *data);
u8  fnWM8960FunWrite(u8 cid, u8 fid, u8 data);

//	WM8960 RTC Access Function
AUDIO_FUNC_T wm8960 =
{
	fnWM8960Init,		//	Initial	
	fnWM8960FunList,	//	List Function
	fnWM8960PlayAudio,	//	Play Audio
	fnWM8960FunRead,	//	Read Function
	fnWM8960FunWrite,	//	Write Function
	NULL,
	NULL,
};

/*
 * date: 2016/08/12
 * modify : hk
 * description: Initial WM8960 Function
 */
u8	fnWM8960Init(u8 cid)
{
	char cmd[64];

	if(wm8960Init)
		return HK_SUCCESS;

	//	Enable MAINIKIN-5V
	sprintf(cmd, "echo 1 >/sys/class/power_supply/MANIKIN_5V/online");

	system(cmd);

	//	Enable MAINIKIN-5V
	sprintf(cmd, "echo 1 >/sys/class/power_supply/MANIKIN_12V/online");

	system(cmd);

	fnWM8960FunWrite(cid, WM8960_FT_HPV, 124);		//  Headphone Playback Volume
	fnWM8960FunWrite(cid, WM8960_FT_HPZ, 1);		//  Headphone Playback ZC Switch
	fnWM8960FunWrite(cid, WM8960_FT_SPV, 124);		//  Speaker Playback Volume
	fnWM8960FunWrite(cid, WM8960_FT_SAV, 100);		//  Speaker AC Volume
	fnWM8960FunWrite(cid, WM8960_FT_SDV, 100);		//  Speaker DC Volume
	fnWM8960FunWrite(cid, WM8960_FT_SPZ, 0);		//  Speaker Playback ZC Switch
	fnWM8960FunWrite(cid, WM8960_FT_PCP, 0);		//  PCM Playback -6dB Switch
	fnWM8960FunWrite(cid, WM8960_FT_MOL, 1);		//  Mono Output Mixer Left Switch
	fnWM8960FunWrite(cid, WM8960_FT_MOR, 1);		//  Mono Output Mixer Right Switch
	fnWM8960FunWrite(cid, WM8960_FT_PBV, 192);		//  Playback Volume
	fnWM8960FunWrite(cid, WM8960_FT_3D2, 0);		//  3D Switch
	fnWM8960FunWrite(cid, WM8960_FT_3DL, 0);		//  3D Filter Lower Cut-Off
	fnWM8960FunWrite(cid, WM8960_FT_3DU, 0);		//  3D Filter Upper Cut-Off
	fnWM8960FunWrite(cid, WM8960_FT_AHP, 0);		//  ADC High Pass Filter Switch
	fnWM8960FunWrite(cid, WM8960_FT_APT, 0);		//  ADC Polarity
	fnWM8960FunWrite(cid, WM8960_FT_ATK, 0);		//  ALC Attack
	fnWM8960FunWrite(cid, WM8960_FT_ADY, 0);		//  ALC Decay
	fnWM8960FunWrite(cid, WM8960_FT_AFN, 0);		//  ALC Function
	fnWM8960FunWrite(cid, WM8960_FT_AHT, 0);		//  ALC Hold Time
	fnWM8960FunWrite(cid, WM8960_FT_AMG, 0);		//  ALC Max Gain
	fnWM8960FunWrite(cid, WM8960_FT_ANG, 0);		//  ALC Min Gain
	fnWM8960FunWrite(cid, WM8960_FT_ALC, 0);		//  ALC Mode
	fnWM8960FunWrite(cid, WM8960_FT_ATT, 0);		//  ALC Target
	fnWM8960FunWrite(cid, WM8960_FT_DDS, 0);		//  DAC Deemphasis Switch
	fnWM8960FunWrite(cid, WM8960_FT_DPT, 1);		//  DAC Polarity
	fnWM8960FunWrite(cid, WM8960_FT_LP1, 0);		//  Left Boost Mixer LINPUT1 Switch
	fnWM8960FunWrite(cid, WM8960_FT_LP2, 0);		//  Left Boost Mixer LINPUT2 Switch
	fnWM8960FunWrite(cid, WM8960_FT_LP3, 1);		//  Left Boost Mixer LINPUT3 Switch
	fnWM8960FunWrite(cid, WM8960_FT_LV2, 0);		//  Left Input Boost Mixer LINPUT2 Volume
	fnWM8960FunWrite(cid, WM8960_FT_LV3, 0);		//  Left Input Boost Mixer LINPUT3 Volume
	fnWM8960FunWrite(cid, WM8960_FT_LMB, 0);		//  Left Input Mixer Boost Switch
	fnWM8960FunWrite(cid, WM8960_FT_LMS, 0);		//  Left Output Mixer Boost Bypass Switch
	fnWM8960FunWrite(cid, WM8960_FT_LBS, 0);		//  Left Output Mixer Boost Bypass Volume
	fnWM8960FunWrite(cid, WM8960_FT_LBV, 0);		//  Left Output Mixer LINPUT3 Switch
	fnWM8960FunWrite(cid, WM8960_FT_L3S, 0);		//  Left Output Mixer LINPUT3 Volume
	fnWM8960FunWrite(cid, WM8960_FT_L3V, 0);		//  Left Output Mixer PCM Playback Switch
	fnWM8960FunWrite(cid, WM8960_FT_NGS, 0);		//  Noise Gate Switch
	fnWM8960FunWrite(cid, WM8960_FT_NGT, 21);		//  Noise Gate Threshold
	fnWM8960FunWrite(cid, WM8960_FT_RP1, 1);		//  Right Boost Mixer RINPUT1 Switch
	fnWM8960FunWrite(cid, WM8960_FT_RP2, 0);		//  Right Boost Mixer RINPUT2 Switch
	fnWM8960FunWrite(cid, WM8960_FT_RP3, 0);		//  Right Boost Mixer RINPUT3 Switch
	fnWM8960FunWrite(cid, WM8960_FT_RM2, 0);		//  Right Input Boost Mixer RINPUT2 Volume
	fnWM8960FunWrite(cid, WM8960_FT_RM3, 100);		//  Right Input Boost Mixer RINPUT3 Volume
	fnWM8960FunWrite(cid, WM8960_FT_RMS, 0);		//  Right Input Mixer Boost Switch
	fnWM8960FunWrite(cid, WM8960_FT_RBS, 0);		//  Right Output Mixer Boost Bypass Switch
	fnWM8960FunWrite(cid, WM8960_FT_RBV, 0);		//  Right Output Mixer Boost Bypass Volume
	fnWM8960FunWrite(cid, WM8960_FT_RPS, 1);		//  Right Output Mixer PCM Playback Switch
	fnWM8960FunWrite(cid, WM8960_FT_R3S, 0);		//  Right Output Mixer RINPUT3 Switch
	fnWM8960FunWrite(cid, WM8960_FT_R3V, 0);		//  Right Output Mixer RINPUT3 Volume

	wm8960Init = 1;

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/15
 * modify : hk
 * description: List WM8960 Function
 */
u8 fnWM8960FunList(void)
{
	printf("\n");
	printf(  "|====|========================================|========|=======|\n");
	printf(  "|                      WM8960 Function List                    |\n");
	printf(  "|====|========================================|========|=======|\n");
	printf(  "| ID |                Name                    | Access | value |\n");
	printf(  "|====|========================================|========|=======|\n");
	printf(  "| 01 | Capture Volume                         |   rw   | 0~63  |\n");
	printf(  "| 02 | Capture Volume ZC Switch               |   rw   | 0~1   |\n");
	printf(  "| 03 | Capture Switch                         |   rw   | 0/1   |\n");
	printf(  "| 04 | Right Input Boost Mixer RINPUT3 Volume |   rw   | 0~7   |\n");
	printf(  "| 05 | Right Input Boost Mixer RINPUT2 Volume |   rw   | 0~7   |\n");
	printf(  "| 06 | Left Input Boost Mixer LINPUT3 Volume  |   rw   | 0~7   |\n");
	printf(  "| 07 | Left Input Boost Mixer LINPUT2 Volume  |   rw   | 0~7   |\n");
	printf(  "| 08 | Playback Volume                        |   rw   | 0~255 |\n");
	printf(  "| 09 | Headphone Playback Volume              |   rw   | 0~127 |\n");
	printf(  "| 10 | Headphone Playback ZC Switch           |   rw   | 0/1   |\n");
	printf(  "| 11 | Speaker Playback Volume                |   rw   | 0~127 |\n");
	printf(  "| 12 | Speaker Playback ZC Switch             |   rw   | 0~1   |\n");
	printf(  "| 13 | Speaker DC Volume                      |   rw   | 0~5   |\n");
	printf(  "| 14 | Speaker AC Volume                      |   rw   | 0~5   |\n");
	printf(  "| 15 | PCM Playback -6dB Switch               |   rw   | 0/1   |\n");
	printf(  "| 16 | ADC Polarity                           |   rw   | 0~3   |\n");
	printf(  "| 17 | ADC High Pass Filter Switch            |   rw   | 0/1   |\n");
	printf(  "| 18 | DAC Polarity                           |   rw   | 0/1   |\n");
	printf(  "| 19 | DAC Deemphasis Switch                  |   rw   | 0/1   |\n");
	printf(  "| 20 | 3D Filter Upper Cut-Off                |   rw   | 0/1   |\n");
	printf(  "| 21 | 3D Filter Lower Cut-Off                |   rw   | 0/1   |\n");
	printf(  "| 22 | 3D Volume                              |   rw   | 0~15  |\n");
	printf(  "| 23 | 3D Switch                              |   rw   | 0/1   |\n");
	printf(  "| 24 | ALC Function                           |   rw   | 0~3   |\n");
	printf(  "| 25 | ALC Max Gain                           |   rw   | 0~7   |\n");
	printf(  "| 26 | ALC Target                             |   rw   | 0~15  |\n");
	printf(  "| 27 | ALC Min Gain                           |   rw   | 0~7   |\n");
	printf(  "| 28 | ALC Hold Time                          |   rw   | 0~15  |\n");
	printf(  "| 29 | ALC Mode                               |   rw   | 0~1   |\n");
	printf(  "| 30 | ALC Decay                              |   rw   | 0~15  |\n");
	printf(  "| 31 | ALC Attack                             |   rw   | 0~15  |\n");
	printf(  "| 32 | Noise Gate Threshold                   |   rw   | 0~31  |\n");
	printf(  "| 33 | Noise Gate Switch                      |   rw   | 0/1   |\n");
	printf(  "| 34 | ADC PCM Capture Volume                 |   rw   | 0~255 |\n");
	printf(  "| 35 | Left Output Mixer Boost Bypass Volume  |   rw   | 0~7   |\n");
	printf(  "| 36 | Left Output Mixer LINPUT3 Volume       |   rw   | 0~7   |\n");
	printf(  "| 37 | Right Output Mixer Boost Bypass Volume |   rw   | 0~7   |\n");
	printf(  "| 38 | Right Output Mixer RINPUT3 Volume      |   rw   | 0~7   |\n");
	printf(  "| 39 | Mono Output Mixer Left Switch          |   rw   | 0/1   |\n");
	printf(  "| 40 | Mono Output Mixer Right Switch         |   rw   | 0/1   |\n");
	printf(  "| 41 | Right Output Mixer PCM Playback Switch |   rw   | 0/1   |\n");
	printf(  "| 42 | Right Output Mixer RINPUT3 Switch      |   rw   | 0/1   |\n");
	printf(  "| 43 | Right Output Mixer Boost Bypass Switch |   rw   | 0/1   |\n");
	printf(  "| 44 | Left Output Mixer PCM Playback Switch  |   rw   | 0/1   |\n");
	printf(  "| 45 | Left Output Mixer LINPUT3 Switch       |   rw   | 0/1   |\n");
	printf(  "| 46 | Left Output Mixer Boost Bypass Switch  |   rw   | 0/1   |\n");
	printf(  "| 47 | Right Input Mixer Boost Switch         |   rw   | 0/1   |\n");
	printf(  "| 48 | Left Input Mixer Boost Switch          |   rw   | 0/1   |\n");
	printf(  "| 49 | Right Boost Mixer RINPUT2 Switch       |   rw   | 0/1   |\n");
	printf(  "| 50 | Right Boost Mixer RINPUT3 Switch       |   rw   | 0/1   |\n");
	printf(  "| 51 | Right Boost Mixer RINPUT1 Switch       |   rw   | 0/1   |\n");
	printf(  "| 52 | Left Boost Mixer LINPUT2 Switch        |   rw   | 0/1   |\n");
	printf(  "| 53 | Left Boost Mixer LINPUT3 Switch        |   rw   | 0/1   |\n");
	printf(  "| 54 | Left Boost Mixer LINPUT1 Switch        |   rw   | 0/1   |\n");
	printf(  "| 55 | Headphone Jack'                        |   r    | 0/1   |\n");
	printf(  "|====|========================================|========|=======|\n");

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/15
 * modify : hk
 * description: Play Audio Function
 */
u8 fnWM8960PlayAudio(void)
{
	char cmd[64];

	wm8960Play = ~wm8960Play;

	sprintf(cmd, "aplay -D hw:%d,0 audio.wav &", AC_WM8960);

	//if(wm8960Play)
	//	sprintf(cmd, "aplay -D hw:%d,0 audio.wav &", AC_WM8960);
	//else
	//	sprintf(cmd, "killall aplay");

	printf("\n");

	system(cmd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/15
 * modify : hk
 * description: Read Function
 */
u8 fnWM8960FunRead(u8 cid, u8 fid, u8 *data)
{
	char cmd[32];

	if((fid < WM8960_CFI_START)||(fid > WM8960_CFI_END))
	{
		printf("\n  ### hk_dbg, %s : Out off range, Please try agagin!\n", __FUNCTION__);

		return HK_FAIL;
	}

	printf("\n");

	AMIXER_CGET(cmd, cid, fid);

	system(cmd);

	printf("\n ### hk_dbg, %s , line:%d, cmd :%s\n", __FUNCTION__, __LINE__, cmd);

	return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/07/15
 * modify : hk
 * description: Write Function
 */
u8 fnWM8960FunWrite(u8 cid, u8 fid, u8 data)
{
	char cmd[32];

	if((fid < WM8960_CFI_START)||(fid > WM8960_CFI_END))
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
