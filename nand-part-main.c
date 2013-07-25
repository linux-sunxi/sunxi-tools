/*
 * (C) Copyright 2013
 * Patrick H Wood, All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include "nand-common.h"

void usage(const char *cmd)
{
	printf("usage: %s [-f a10|a20] nand-device\n", cmd);
	printf("       %s nand-device 'name2 len2 [usertype2]' ['name3 len3 [usertype3]'] ...\n", cmd);
	printf("       %s [-f a10|a20] nand-device start1 'name1 len1 [usertype1]' ['name2 len2 [usertype2]'] ...\n", cmd);
}

typedef struct tag_CRC32_DATA
{
	__u32 CRC;				//int的大小是32位
	__u32 CRC_32_Tbl[256];	//用来保存码表
}CRC32_DATA_t;

__u32 calc_crc32(void * buffer, __u32 length)
{
	__u32 i, j;
	CRC32_DATA_t crc32;		//
	__u32 CRC32 = 0xffffffff; //设置初始值
	crc32.CRC = 0;

	for( i = 0; i < 256; ++i)//用++i以提高效率
	{
		crc32.CRC = i;
		for( j = 0; j < 8 ; ++j)
		{
			//这个循环实际上就是用"计算法"来求取CRC的校验码
			if(crc32.CRC & 1)
				crc32.CRC = (crc32.CRC >> 1) ^ 0xEDB88320;
			else //0xEDB88320就是CRC-32多项表达式的值
				crc32.CRC >>= 1;
		}
		crc32.CRC_32_Tbl[i] = crc32.CRC;
	}

	CRC32 = 0xffffffff; //设置初始值
	for( i = 0; i < length; ++i)
	{
		CRC32 = crc32.CRC_32_Tbl[(CRC32^((unsigned char*)buffer)[i]) & 0xff] ^ (CRC32>>8);
	}
	//return CRC32;
	return CRC32^0xffffffff;
}

int main (int argc, char **argv)
{
	char *nand = "/dev/nand";
	const char *cmd = argv[0];
	int fd;
	int force = 0;		// force write even if magics and CRCs don't match

	argc--;
	argv++;

	if (argc > 1) {
		if (!strcmp(argv[0], "-f")) {
			if (!strcasecmp(argv[1], "a10"))
				force = 10;
			else if (!strcasecmp(argv[1], "a20"))
				force = 20;
			else {
				usage(cmd);
				return -1;
			}
			argc -= 2;
			argv += 2;
		}
	}

	if (argc > 0) {
		nand = argv[0];
		argc--;
		argv++;
	}
	fd = open(nand, O_RDWR);
	if (fd < 0) {
		usage(cmd);
		return -2;
	}
	if (force == 10)
		return nand_part_a10 (argc, argv, cmd, fd, force);
	if (force == 20)
		return nand_part_a20 (argc, argv, cmd, fd, force);

	if (checkmbrs_a10(fd))
		return nand_part_a10 (argc, argv, cmd, fd, force);
	if (checkmbrs_a20(fd))
		return nand_part_a20 (argc, argv, cmd, fd, force);
}
