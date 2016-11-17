#ifndef _AUDIO_COMM_H_
#define _AUDIO_COMM_H_

#include "common.h"
#include "audio.h"

//	ALSA Mixer Commnad
#define AMIXER_CGET(format, args...)	sprintf(format, "amixer -c %d cget numid=%d ", ##args)		//	Get Control Contents For One Control
#define AMIXER_CSET(format, args...)	sprintf(format, "amixer -c %d cset numid=%d %d", ##args)	//	Set Control Contents For One Control



u8 fnAudioRead(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);
u8 fnAudioWrite(char* bus, u8 addr, u8 reg, u8 *buf, u8 len);

#endif	//	_AUDIO_COMM_H_
