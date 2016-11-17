#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "system.h"
#include "sdramComm.h"

static const u64 bitpattern[] =
{
	0x00000001,			// single bit
	0x00000003,			// two adjacent bits
	0x00000007,			// three adjacent bits
	0x0000000F,			// four adjacent bits
	0x00000005,			// two non-adjacent bits
	0x00000015,			// three non-adjacent bits
	0x00000055,			// four non-adjacent bits
	0xaaaaaaaa,			// alternating 1/0
};

/*
 * date: 2016/09/01
 * modify : hk
 * description: SDRAM Alternative Test
 */
u8 fnSDRAMTestAlt(u64* buf, u64* dummy, u64 sAddr, u64 length)
{
	v64 *addr;
	u64 errs = 0;
	u64 val, readback;
	u8 	j;
	v64 offset;
	v64 test_offset;
	v64 pattern;
	v64 temp;
	v64 anti_pattern;
	v64 words;

	words = length / sizeof(u64);

	/*
	 * Data line test: write a pattern to the first
	 * location, write the 1's complement to a 'parking'
	 * address (changes the state of the data bus so a
	 * floating bus doesn't give a false OK), and then
	 * read the value back. Note that we read it back
	 * into a variable because the next time we read it,
	 * it might be right (been there, tough to explain to
	 * the quality guys why it prints a failure when the
	 * "is" and "should be" are obviously the same in the
	 * error message).
	 *
	 * Rather than exhaustively testing, we test some
	 * patterns by shifting '1' bits through a field of
	 * '0's and '0' bits through a field of '1's (i.e.
	 * pattern and ~pattern).
	 */

	addr = (v64 *)buf;

	for(j=0; j<sizeof(bitpattern)/sizeof(bitpattern[0]); j++)
	{
		val = bitpattern[j];

		for(; val!=0; val<<=1)
		{
			*addr = val;

			*dummy = ~val; /* clear the test data off the bus */

			readback = *addr;

			if(readback != val)
			{
				printf("\n  ### hk_dbg, %s : FAILURE (data line), expected %08lx, actual %08lx\n", __FUNCTION__, val, readback);

				errs++;
			}

			*addr = ~val;

			*dummy = val;

			readback = *addr;

			if(readback != ~val)
			{
				printf("\n  ### hk_dbg, %s : FAILURE (data line): ""Is %08lx, should be %08lx\n", __FUNCTION__, readback, ~val);

				errs++;
			}
		}
	}

	if(!errs)
		printf("\n  ### hk_dbg, %s : Data pin test OK!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : Data pin test FAIL!!\n", __FUNCTION__);

	errs = 0;	

	/*
	 * Based on code whose Original Author and Copyright
	 * information follows: Copyright (c) 1998 by Michael
	 * Barr. This software is placed into the public
	 * domain and may be used for any purpose. However,
	 * this notice must not be changed or removed and no
	 * warranty is either expressed or implied by its
	 * publication or distribution.
	 */

	/*
	* Address line test

	 * Description: Test the address bus wiring in a
	 *              memory region by performing a walking
	 *              1's test on the relevant bits of the
	 *              address and checking for aliasing.
	 *              This test will find single-bit
	 *              address failures such as stuck-high,
	 *              stuck-low, and shorted pins. The base
	 *              address and size of the region are
	 *              selected by the caller.

	 * Notes:	For best results, the selected base
	 *              address should have enough LSB 0's to
	 *              guarantee single address bit changes.
	 *              For example, to test a 64-Kbyte
	 *              region, select a base address on a
	 *              64-Kbyte boundary. Also, select the
	 *              region size as a power-of-two if at
	 *              all possible.
	 *
	 * Returns:     0 if the test succeeds, 1 if the test fails.
	 */

	pattern = (v64) 0xaaaaaaaa;
	anti_pattern = (v64) 0x55555555;

	/*
	 * Write the default pattern at each of the power-of-two offsets.
	 */
	for (offset=1; offset<words; offset<<=1)
		addr[offset] = pattern;

	/*
	 * Check for address bits stuck high.
	 */
	test_offset = 0;

	addr[test_offset] = anti_pattern;

	for(offset=1; offset<words; offset<<=1)
	{
		temp = addr[offset];

		if (temp != pattern)
		{
			printf("\n  ### hk_dbg, %s, FAILURE: Address bit stuck high @ 0x%.8lx:" " expected 0x%.8lx, actual 0x%.8lx\n", __FUNCTION__,  sAddr + offset*sizeof(v64), pattern, temp);

			errs++;
		}
	}

	addr[test_offset] = pattern;

	/*
	 * Check for addr bits stuck low or shorted.
	 */
	for(test_offset=1; test_offset<words; test_offset<<=1)
	{
		addr[test_offset] = anti_pattern;

		for(offset=1; offset<words; offset<<=1)
		{
			temp = addr[offset];

			if ((temp != pattern) && (offset != test_offset))
			{
				printf("\n  ### hk_dbg, %s : FAILURE: Address bit stuck low or" " shorted @ 0x%.8lx: expected 0x%.8lx," " actual 0x%.8lx\n", __FUNCTION__, sAddr + offset*sizeof(v64), pattern, temp);

				errs++;
			}
		}

		addr[test_offset] = pattern;
	}

	if(!errs)
		printf("\n  ### hk_dbg, %s : Address bus test OK!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : Address bus test FAIL!!\n", __FUNCTION__);

	errs = 0;
	
	/*
	 * Description: Test the integrity of a physical
	 *		memory device by performing an
	 *		increment/decrement test over the
	 *		entire region. In the process every
	 *		storage bit in the device is tested
	 *		as a zero and a one. The base address
	 *		and the size of the region are
	 *		selected by the caller.
	 *
	 * Returns:     0 if the test succeeds, 1 if the test fails.
	 */
	words++;

	/*
	 * Fill memory with a known pattern.
	 */
	for(pattern=1, offset=0; offset<words; pattern++, offset++)
		addr[offset] = pattern;

	/*
	 * Check each location and invert it for the second pass.
	 */
	for(pattern=1, offset=0; offset<words; pattern++, offset++)
	{
		temp = addr[offset];

		if (temp != pattern)
		{
			printf("\n  ### hk_dbg, %s, FAILURE (read/write) @ 0x%.8lx:" " expected 0x%.8lx, actual 0x%.8lx)\n", __FUNCTION__, sAddr + offset*sizeof(v64), pattern, temp);

			errs++;
		}

		anti_pattern = ~pattern;

		addr[offset] = anti_pattern;
	}

	/*
	 * Check each location for the inverted pattern and zero it.
	 */
	for (pattern=1, offset=0; offset<words; pattern++, offset++)
	{
		anti_pattern = ~pattern;

		temp = addr[offset];

		if (temp != anti_pattern)
		{
			printf("\n  ### hk_dbg, %s, FAILURE (read/write): @ 0x%.8lx:" " expected 0x%.8lx, actual 0x%.8lx)\n", __FUNCTION__, sAddr + offset*sizeof(v64), anti_pattern, temp);

			errs++;
		}

		addr[offset] = 0;
	}

	if(!errs)
		printf("\n  ### hk_dbg, %s : Physical memory test OK!!\n", __FUNCTION__);
	else
		printf("\n  ### hk_dbg, %s : Physical memory test FAIL!!\n", __FUNCTION__);

	if(errs)
		return HK_FAIL;
	else
		return HK_SUCCESS;
}
//------------------------------------------------

/*
 * date: 2016/09/01
 * modify : hk
 * description: SDRAM Quick Test
 */
u8 fnSDRAMTestQuick(u64 *buf, u64 sAddr, u64 length, u64 pattern, s32 iteration)
{
	v64 *end;
	v64 *addr;
	u64 errs = 0;
	u64 incr;
	u64 val, readback;

	/* Alternate the pattern */
	incr = 1;

	if (iteration & 1)
	{
		incr = -incr;
		/*
		 * Flip the pattern each time to make lots of zeros and
		 * then, the next time, lots of ones.  We decrement
		 * the "negative" patterns and increment the "positive"
		 * patterns to preserve this feature.
		 */
		if (pattern & 0x80000000)
			pattern = -pattern;	/* complement & increment */
		else
			pattern = ~pattern;
	}

	end = buf + length;
	
	printf("\n  ### hk_dbg, %s, Pattern %08lX  Writing..." "%12s" "\b\b\b\b\b\b\b\b\b\b\n", __FUNCTION__, pattern, "");

	//	Pointer to buffer address
	addr = (v64 *)buf;

	for(val = pattern; addr < end; addr++)
	{
		*addr = val;

		val += incr;
	}

	printf("\n  ### hk_dbg, %s, Reading...\n", __FUNCTION__);

	//	Pointer to buffer address
    addr = (v64 *)buf;
    
	for (val = pattern; addr < end; addr++)
	{
		readback = *addr;

		if (readback != val)
		{
			u64 offset = addr - buf;
			
			printf("\n  ### hk_dbg, %s, Memory Read @ 0x%08X: " "found %08lX, expected %08lX\n", __FUNCTION__, (u32)(u64)(sAddr + offset * sizeof(v64)), readback, val);

			errs++;
		}
		
		val += incr;
	}

	if(errs)
		return HK_FAIL;
	else
		return HK_SUCCESS;
}
//------------------------------------------------
