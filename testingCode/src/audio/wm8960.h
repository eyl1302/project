#ifndef _WM8960_H_
#define _WM8960_H_

#include "common.h"
#include "audio.h"
#include "audioComm.h"

//	WM8960 Register Table
#define REG_WM8960_LIV				0x00							//	Left Input Volume
#define REG_WM8960_RIV				0x01							//	Right Input Volume
#define REG_WM8960_LOV1				0x02							//	LOut1 Volume
#define REG_WM8960_ROV1			    0x03				            //	ROut1 Volume
#define REG_WM8960_CLK1			    0x04				            //	Clocking(1)
#define REG_WM8960_CTR1			    0x05				            //	ADC & DAC Control(CRT1)
#define REG_WM8960_CTR2			    0x06				            //	ADC & DAC Control(CRT2)
#define REG_WM8960_AIF			    0x07				            //	Audio Interface Format
#define REG_WM8960_CLK2			    0x08				            //	Clocking(2)
#define REG_WM8960_AI			    0x09				            //	Audio Interface
#define REG_WM8960_LDV			    0x0A				            //	Left DAC Volume
#define REG_WM8960_RDV			    0x0B				            //	Right DAC Volume
#define REG_WM8960_RESET		    0x0F				            //	Reset
#define REG_WM8960_3D		   	 	0x10				            //	3D Control
#define REG_WM8960_ALC1		   	 	0x11				            //	ALC1
#define REG_WM8960_ALC2		   	 	0x12				            //	ALC2
#define REG_WM8960_ALC3		   	 	0x13				            //	ALC3
#define REG_WM8960_NG		   	 	0x14				            //	Noise Gate
#define REG_WM8960_LAV			    0x15				            //	Left ADC Volume
#define REG_WM8960_RAV			    0x16				            //	Right ADC Volume
#define REG_WM8960_AC1			    0x17				            //	Additional Control(1)
#define REG_WM8960_AC2			    0x18				            //	Additional Control(2)
#define REG_WM8960_PM1			    0x19				            //	Power Management(1)
#define REG_WM8960_PM2			    0x1A				            //	Power Management(2)
#define REG_WM8960_AC3			    0x1B				            //	Additional Control(3)
#define REG_WM8960_AP1				0x1C							//	Anti-Pop(1)
#define REG_WM8960_AP2              0x1D                            //	Anti-Pop(2)
#define REG_WM8960_ALSP		   	 	0x20				            //	ADCL Signal Path
#define REG_WM8960_ADSP		   	 	0x21				            //	ADCR Signal Path
#define REG_WM8960_LOM1		   	 	0x22				            //	Left Out Mix(1)
#define REG_WM8960_ROM2		   	 	0x25				            //	Right Out Mix(2)
#define REG_WM8960_MOM1		   	 	0x26				            //	Mono Out Mix(1)
#define REG_WM8960_MOM2		   	 	0x27				            //	Mono Out Mix(2)
#define REG_WM8960_LOV2				0x28							//	LOut2 Volume
#define REG_WM8960_ROV2			    0x29				            //	ROut2 Volume
#define REG_WM8960_MOV			    0x2A				            //	MONO Out Volume
#define REG_WM8960_IBM1			    0x2B				            //	Input Boost Mixer
#define REG_WM8960_IBM2				0x2C							//	Input Boost Mixer
#define REG_WM8960_BPS1			    0x2D				            //	Bypass(1)
#define REG_WM8960_BPS2				0x2E							//	Bypass(2)
#define REG_WM8960_PM3			    0x2F				            //	Power Management(3)
#define REG_WM8960_AC4			    0x30				            //	Additional Control(4)
#define REG_WM8960_CDC1			    0x31				            //	Class D Control(1)
#define REG_WM8960_CDC3			    0x33				            //	Class D Control(3)
#define REG_WM8960_PLLN			    0x34				            //	PLL N
#define REG_WM8960_PLLK1		    0x35			            	//	PLL K1
#define REG_WM8960_PLLK2		    0x36			            	//	PLL K2
#define REG_WM8960_PLLK3		    0x37				            //	PLL K3

//	WM8960 Function Table
#define WM8960_FT_HPV				9 								//	Headphone Playback Volume
#define WM8960_FT_HPZ				10                              //	Headphone Playback ZC Switch
#define WM8960_FT_SPV               11                              //	Speaker Playback Volume
#define WM8960_FT_SAV               14                              //	Speaker AC Volume
#define WM8960_FT_SDV               13                              //	Speaker DC Volume
#define WM8960_FT_SPZ               12                              //	Speaker Playback ZC Switch
#define WM8960_FT_PCP               15                              //	PCM Playback -6dB Switch
#define WM8960_FT_MOL               39                              //	Mono Output Mixer Left Switch
#define WM8960_FT_MOR               40                              //	Mono Output Mixer Right Switch
#define WM8960_FT_PBV               8                               //	Playback Volume
#define WM8960_FT_3D2               23                              //	3D Switch
#define WM8960_FT_3DL               21                              //	3D Filter Lower Cut-Off
#define WM8960_FT_3DU               20                              //	3D Filter Upper Cut-Off
#define WM8960_FT_AHP               17                              //	ADC High Pass Filter Switch
#define WM8960_FT_APT               16                              //	ADC Polarity
#define WM8960_FT_ATK               31                              //	ALC Attack
#define WM8960_FT_ADY               30                              //	ALC Decay
#define WM8960_FT_AFN               24                              //	ALC Function
#define WM8960_FT_AHT               28                              //	ALC Hold Time
#define WM8960_FT_AMG               25                              //	ALC Max Gain
#define WM8960_FT_ANG               27                              //	ALC Min Gain
#define WM8960_FT_ALC               29                              //	ALC Mode
#define WM8960_FT_ATT               26                              //	ALC Target
#define WM8960_FT_DDS               19                              //	DAC Deemphasis Switch
#define WM8960_FT_DPT               18                              //	DAC Polarity
#define WM8960_FT_LP1               54                              //	Left Boost Mixer LINPUT1 Switch
#define WM8960_FT_LP2               52                              //	Left Boost Mixer LINPUT2 Switch
#define WM8960_FT_LP3               53                              //	Left Boost Mixer LINPUT3 Switch
#define WM8960_FT_LV2               7                               //	Left Input Boost Mixer LINPUT2 Volume
#define WM8960_FT_LV3               6                               //	Left Input Boost Mixer LINPUT3 Volume
#define WM8960_FT_LMB               48                              //	Left Input Mixer Boost Switch
#define WM8960_FT_LMS               46                              //	Left Output Mixer Boost Bypass Switch
#define WM8960_FT_LBS               35                              //	Left Output Mixer Boost Bypass Volume
#define WM8960_FT_LBV               45                              //	Left Output Mixer LINPUT3 Switch
#define WM8960_FT_L3S               36                              //	Left Output Mixer LINPUT3 Volume
#define WM8960_FT_L3V               44                              //	Left Output Mixer PCM Playback Switch
#define WM8960_FT_NGS               33                              //	Noise Gate Switch
#define WM8960_FT_NGT               32                              //	Noise Gate Threshold
#define WM8960_FT_RP1               51                              //	Right Boost Mixer RINPUT1 Switch
#define WM8960_FT_RP2               49                              //	Right Boost Mixer RINPUT2 Switch
#define WM8960_FT_RP3               50                              //	Right Boost Mixer RINPUT3 Switch
#define WM8960_FT_RM2               5                               //	Right Input Boost Mixer RINPUT2 Volume
#define WM8960_FT_RM3               4                               //	Right Input Boost Mixer RINPUT3 Volume
#define WM8960_FT_RMS               47                              //	Right Input Mixer Boost Switch
#define WM8960_FT_RBS               43                              //	Right Output Mixer Boost Bypass Switch
#define WM8960_FT_RBV               37                              //	Right Output Mixer Boost Bypass Volume
#define WM8960_FT_RPS               41                              //	Right Output Mixer PCM Playback Switch
#define WM8960_FT_R3S               42                              //	Right Output Mixer RINPUT3 Switch
#define WM8960_FT_R3V               38                              //	Right Output Mixer RINPUT3 Volume

//	WM8960 Control Function Index Range
#define WM8960_CFI_START			1								//	WM8960 Control Function Start Index
#define WM8960_CFI_END				55								//	WM8960 Control Function End Index


//	WM8960G I2C format is address(7bit) + write(1bit, write only) + register address(7bit) + register data(9bit), so we must calculate such register before we use it.
#define REG_OFFSET 					1
#define REG_CONFIG(a)				((a<<1) + REG_OFFSET)

#define	WM8960_I2C_BUS				"/dev/i2c-0"					//	Audio Codec I2C Bus
#define WM8960_I2C_ADDRESS			0x1a							//	Audio Codec Slave Address

extern AUDIO_FUNC_T wm8960;

#endif	//	_WM8960_H_
