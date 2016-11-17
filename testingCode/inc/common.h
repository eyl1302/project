#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdarg.h>

typedef	unsigned char 			u8;
typedef unsigned short			u16;
typedef	unsigned int			u32;
typedef unsigned long			u64;

typedef	signed char 			s8;
typedef signed short			s16;
typedef	signed int				s32;
typedef signed long				s64;

typedef volatile unsigned long	v64;
typedef volatile unsigned short v16;
typedef volatile unsigned char	vu8;

typedef unsigned long int 		ul64;

#define DBG

#ifdef DBG
#define hk_dbg(format, args...)	printf(format, ##args)
#else
#define hk_dbg(args...)
#endif

typedef enum
{
	HK_FAIL	   = 0x00,
	HK_SUCCESS = 0x01,
	HK_NA	   = 0xFF,

}	HK_TEST_T;

#endif //	__COMMON_H__
