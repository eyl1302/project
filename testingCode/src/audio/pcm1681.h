#ifndef _PCM1681_H_
#define _PCM1681_H_

#include "common.h"
#include "audio.h"
#include "audioComm.h"

#define	PCM1681_I2C_BUS				"/dev/i2c-2"					//	PCM1681 I2C Bus
#define PCM1681_I2C_ADDRESS			0x4C							//	PCM1681 Codec Slave Address

//	Register Map
#define REG_PCM1681_DAC_AT1			0x01							//	Digital Attenuation Control, 0 dB to -63dB in 0.5dB Steps(Volume 0 ~ 127)
#define REG_PCM1681_DAC_AT2			0x02							//	Digital Attenuation Control, 0 dB to -63dB in 0.5dB Steps(Volume 0 ~ 127)
#define REG_PCM1681_DAC_AT3			0x03							//	Digital Attenuation Control, 0 dB to -63dB in 0.5dB Steps(Volume 0 ~ 127)
#define REG_PCM1681_DAC_AT4			0x04                            //	Digital Attenuation Control, 0 dB to -63dB in 0.5dB Steps(Volume 0 ~ 127)
#define REG_PCM1681_DAC_AT5			0x05							//	Digital Attenuation Control, 0 dB to -63dB in 0.5dB Steps(Volume 0 ~ 127)
#define REG_PCM1681_DAC_AT6			0x06                            //	Digital Attenuation Control, 0 dB to -63dB in 0.5dB Steps(Volume 0 ~ 127)
#define REG_PCM1681_SOFT_MUTE		0x07							//	Soft Mute Control
#define REG_PCM1681_DAC_CTRL		0x08							//	DAC Operation Control
#define REG_PCM1681_ADF				0x09							//	Audio Data Format Control
#define REG_PCM1681_DEEM			0x0A							//	De-emphasis All Channel Function Control
#define REG_PCM1681_OP				0x0B							//	Output Phase Select Per Channel
#define REG_PCM1681_OSRATE			0x0C							//	Oversampling Rate Control
#define REG_PCM1681_ZFC				0x0D							//	Zero Flag Combination Select
#define REG_PCM1681_ZDS				0x0E							//	Zero Detect Status
#define REG_PCM1681_DAC_AT7			0x11							//	Digital Attenuation Control, 0 dB to -63dB in 0.5dB Steps(Volume 0 ~ 127)
#define REG_PCM1681_DAC_AT8			0x12							//	Digital Attenuation Control, 0 dB to -63dB in 0.5dB Steps(Volume 0 ~ 127)

//	Audio Data Format Control Bit Control
//  |----------|--------------------------------------------------|
//	| FMT[3:0] |      Audio Data Format Selection                 |
//  |----------|--------------------------------------------------|
//	|   0000   |      Right-justified format, 24bit               |
//  |----------|--------------------------------------------------|
//	|   0001   |      Reserved                                    |
//  |----------|--------------------------------------------------|
//	|   0010   |      Reserved                                    |
//  |----------|--------------------------------------------------|
//	|   0011   |      Right-justified format, 16bit               |
//  |----------|--------------------------------------------------|
//	|   0100   |      I2S format, 16 to 24 bit                    |
//  |----------|--------------------------------------------------|
//	|   0101   |      Left-justified format, 24bit                |
//  |----------|--------------------------------------------------|
//	|   0110   |      I2S format, TDM Formt, 24 bit               |
//  |----------|--------------------------------------------------|
//	|   0111   |      Left-justified format, TDM Formt, 24bit     |
//  |----------|--------------------------------------------------|
//	|   1000   |      I2S format, DSP Formt, 24 bit               |
//  |----------|--------------------------------------------------|
//	|   1000   |      Left-justified format, DSP Formt, 24bit     |
//  |----------|--------------------------------------------------|
#define REG_ADF_FMT					(1<<0)
#define REG_ADF_FLT					(1<<5)							// Digital Filter Roll-Off Control (0 : Sharp Roll-off, 1:Slow Roll-off)

//	De-emphasis All Channel Function Control Bit Control
//  |----------|--------------------------------------------------|
//	| DMF[1:0] |      De-Emphasis Sampling Rate Selection         |
//  |----------|--------------------------------------------------|
//	|    00    |      44.1 KHz                                    |
//  |----------|--------------------------------------------------|
//	|    01    |      48 KHz                                      |
//  |----------|--------------------------------------------------|
//	|    10    |      32 KHz                                      |
//  |----------|--------------------------------------------------|
//	|    11    |      Reserved                                    |
//  |----------|--------------------------------------------------|
#define REG_ADF_DEEM_DMC			(1<<0)							//	De-emphasis Control for all channels (0:Disable, 1:Enable)
#define REG_ADF_DEEM_DMF			(1<<3)							//	De-Emphasis Sampling Rate Selection
#define REG_ADF_DEEM_DREV			(1<<5)							//	Output Phase Select (0:Normal Output, 1:Inverted Output)
#define REG_ADF_DEEM_ZREV			(1<<6)							//	Zero Flag Polarity Select (0: Low detect, 1:High detect)
#define REG_ADF_DEEM_SRST			(1<<7)							//	Reset (0:Disable, 1:Enable)

//	Oversampling Rate Control Bit Control
//  |----------|-------------------------------|-------------------------------|-------------------------------|
//	|   OVER   |     512fs, 768fs, 1152fs      |          768fs, 1152fs        |          128fs, 192fs         |
//  |----------|-------------------------------|-------------------------------|-------------------------------|
//	|    0     | x64 oversampling, narrow mode | x32 oversampling, narrow mode | x16 oversampling, narrow mode |
//  |----------|-------------------------------|-------------------------------|-------------------------------|
//	|    1     | x128 oversampling, wide mode  | x64 oversampling, wide mode   | x32 oversampling, wide mode   |
//  |----------|-------------------------------|-------------------------------|-------------------------------|
#define REG_OSRATE_OVER				(1<<7)							//	Oversampling Rate Control
#define REG_OSRATE_FLT				(1<<0)							//	Digital Filter Roll-Off Control per DATA Group, (0 : Sharp Roll-off, 1:Slow Roll-off)

//	Zero Flag Combination Select
//  |----------|--------------------------------------------------------------|
//	| AZRO[1:0]|      Zero-Flag Channel-Combination Select                    |
//  |----------|--------------------------------------------------------------|
//	|    00    |      Combination A (ZR1: DATA1 L-ch, ZR2: DATA1 R-ch)        |
//  |----------|--------------------------------------------------------------|
//	|    01    |      Combination B (ZR1: DATA1¡VDATA4, ZR2: DATA1¡VDATA4)      |
//  |----------|--------------------------------------------------------------|
//	|    10    |      CombinationC (ZR1: DATA4, ZR2: DATA1¡VDATA3)             |
//  |----------|--------------------------------------------------------------|
//	|    11    |      CombinationD (ZR1: DATA1, ZR2: DATA2¡VDATA4)             |
//  |----------|--------------------------------------------------------------|
#define REG_ZFC_AZRO				(1<<5)							//	Zero-Flag Channel-Combination Select
#define REG_ZFC_DAMS				(1<<7)							//  Digital Attenuation Mode Select (0:0.5dB/Step fro 0 to -63 dB, 1:1db/Step for 1 to -100 dB)

//	PCM1681 Control Function Index Range
#define PCM1681_CFI_START			1								//	PCM1681 Control Function Start Index
#define PCM1681_CFI_END				5								//	PCM1681 Control Function End Index

extern AUDIO_FUNC_T pcm1681;

#endif	//	_PCM1681_H_
