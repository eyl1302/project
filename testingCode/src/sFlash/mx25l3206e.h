#ifndef _MX25L3206E_H_
#define _MX25L3206E_H_

#include "common.h"
#include "sFlash.h"
#include "sFlashComm.h"

#define MX25L3206E_UCDEV		"/dev/mtd0"			//	Character Device for User
#define MX25L3206E_FCDEV		"/dev/mtd1"			//	Character Device for Factory
#define MX25L3206E_UBDEV		"/dev/mtdblock0"	//	Block Device for User
#define MX25L3206E_FBDEV		"/dev/mtdblock1"	//	Block Device for Factory

//	Block Divide, SPI Flahs was divided into two equal partition, block number 0 ~ 63
#define UCDEV_START_BLCK		0					//	User Partition Used
#define FCDEV_START_BLCK		32					//	Factory Partition Used

#define WSF_IMG					"wsfImg.img"		//	Source file wirte to flash
#define RSF_IMG					"rsfImg.img"		//	Destination file read from flash

#define MTD_ACC_LEN				256					//	Flash Access Length

//	Memory Organization
//	1 Blocks = 16 Secotrs
//	1 Secotr = 4096 Bytes
//	64 Blcoks = 64 * 16  * 4096 = 4M Bytes(4194304 Bytes, 33554432 Bits)
//	|----------------------------------------|
//	|  Block  |  Sector  |   Address Range   |
//	|----------------------------------------|
//	|         |   1023   | 3FF000h | 3FFFFFh |
//	|         -------------------------------|
//	|    63   |   ...    |   ...   |   ...   |
//	|         -------------------------------|
//	|         |   1008   | 3F0000h | 3F0FFFh |
//	|----------------------------------------|
//	|         |   1007   | 3EF000h | 3EFFFFh |
//	|         -------------------------------|
//	|    62   |   ...    |   ...   |   ...   |
//	|         -------------------------------|
//	|         |   992    | 3E0000h | 3E0FFFh |
//	|----------------------------------------|
//	|         |   ...    |   ...   |   ...   |
//	|         -------------------------------|
//	|   ...   |   ...    |   ...   |   ...   |
//	|         -------------------------------|
//	|         |   ...    |   ...   |   ...   |
//	|----------------------------------------|
//	|         |    15    | 00F000h | 00FFFFh |
//	|         -------------------------------|
//	|         |   ...    |   ...   |   ...   |
//	|         -------------------------------|
//	|         |    3     | 003000h | 003FFFh |
//	|    0    -------------------------------|
//	|         |    2     | 002000h | 002FFFh |
//	|         -------------------------------|
//	|         |    1     | 001000h | 001FFFh |
//	|         -------------------------------|
//	|         |    0     | 000000h | 000FFFh |
//	|----------------------------------------|

#define MX25L3206E_BLOCK_TOTAL			64
#define MX25L3206E_SETCOR_TOTAL			1024

#define MX25L3206E_BLOCK_MIN			0
#define MX25L3206E_BLOCK_MAX			63
#define MX25L3206E_SETCOTR_MIN			0
#define MX25L3206E_SETCOTR_MAX			1023
#define MX25L3206E_ADDR_START			0x000000
#define MX25L3206E_ADDR_END				0x3FFFFF

#define MX25L3206E_SECTOR_PER_BLOCK		16
#define MX25L3206E_SECTOR_SIZE			4096
#define MX25L3206E_BLOCK_SIZE			(MX25L3206E_SECTOR_PER_BLOCK * MX25L3206E_SECTOR_SIZE)


//	Command Definition
#define CMD_WREN						0x06						//	Write Enable
#define CMD_WRDI						0x04						//	Write Disable
#define CMD_RDSR						0x05						//	Read Status Register
#define CMD_WRSR						0x01						//	Write Status Register
#define CMD_READ						0x03						//	Read Data
#define CMD_FREAD						0x0B						//	Fast Read Data
#define CMD_DREAD						0x3B						//	Double Output Mode Command
#define CMD_SE							0x20						//	Sector Erase
#define CMD_BE							0x52						//	Block Erase
#define CMD_CE							0x60						//	Chip Erase
#define CMD_PP							0x02						//	Page Program
#define CMD_DP							0xB9						//	Deep Power Down
#define CMD_RDP							0xAB						//	Release From Deep Power Down
#define CMD_RES							0xAB						//	Read Electronic ID
#define CMD_RDID						0x9F						//	Read Identification
#define CMD_REMS						0x90						//	Read Electronic Manufacturer & Device ID
#define CMD_ENSO						0xB1						//	Enter Secured OTP
#define CMD_EXSO						0xC1						//	Exit Secured OTP
#define CMD_RDSCUR						0x2B						//	Read Security Register
#define CMD_WRSCUR						0x2F						//	Write Security Register
#define CMD_RDDMC						0x5A						//	Read DMC

//	Read Status Register
#define BIT_WIP							(1<<0)						//	The Write in Progress bit.
#define BIT_WEL							(1<<1)						//	The Write Enable Latch bit.
#define BIT_BP							(0x0F<<2)					//	The Block Protect bits(BP3-BP0).
#define BIT_SRWD						(1<<7)						//	The Status Register Write Disable bit.

extern SFLASH_FUNC_T	mx25l3206e;

#endif	//_MX25L3206E_H_
