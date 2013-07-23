/*
 * mbr.c
 * (C) Copyright 2012
 * Patrick H Wood, All rights reserved.
 * Heavily modified from the Allwinner file drivers/block/sun4i_nand/nfd/mbr.c.
 * (Allwinner copyright block retained below.)
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

/*
 * drivers/block/sun4i_nand/nfd/mbr.c
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mount.h> /* BLKRRPART */
#ifdef A20
# include "nand-part-a20.h"
#else
# include "nand-part.h"
#endif

#define MAX_NAME 16

static void printmbrheader(MBR *mbr)
{
	printf("mbr: version 0x%08x, magic %8.8s\n", mbr->version, mbr->magic);
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

MBR *_get_mbr(int fd, int mbr_num, int force)
{
	MBR *mbr;

	/*request mbr space*/
	mbr = malloc(sizeof(MBR));
	if(mbr == NULL)
	{
		printf("%s : request memory fail\n",__FUNCTION__);
		return NULL;
	}

	/*get mbr from nand device*/
	lseek(fd,MBR_START_ADDRESS + MBR_SIZE*mbr_num,SEEK_SET);
	if(read(fd,mbr,MBR_SIZE) == MBR_SIZE)
	{
		/*checksum*/
		printf("check partition table copy %d: ", mbr_num);
		printmbrheader(mbr);
		if (force) {
			strncpy((char *)mbr->magic, MBR_MAGIC, 8);
#ifdef MBR_VERSION
			mbr->version = MBR_VERSION;
#endif
			return mbr;
		}
		if(strncmp((char *)mbr->magic, MBR_MAGIC, 8))
		{
			printf("magic %8.8s is not %8s\n", mbr->magic, MBR_MAGIC);
			return NULL;
		}
#ifdef MBR_VERSION
		if(mbr->version != MBR_VERSION)
		{
			printf("version 0x%08x is not 0x%08x\n", mbr->version, MBR_VERSION);
			return NULL;
		}
#endif
		if(*(__u32 *)mbr == calc_crc32((__u32 *)mbr + 1,MBR_SIZE - 4))
		{
			printf("OK\n");
			return mbr;
		}
		printf("BAD!\n");
	}
	return NULL;
}

__s32 _free_mbr(MBR *mbr)
{
	if(mbr)
	{
		free(mbr);
		mbr = 0;
	}

	return 0;
}

void printmbr(MBR *mbr)
{
	unsigned int part_cnt;
	
	printmbrheader(mbr);
	for(part_cnt = 0; part_cnt < mbr->PartCount && part_cnt < MAX_PART_COUNT; part_cnt++)
	{
		if(1 || (mbr->array[part_cnt].user_type == 2) || (mbr->array[part_cnt].user_type == 0))
		{
			printf("partition %2d: class = %12s, name = %12s, partition start = %8d, partition size = %8d user_type=%d\n",
						part_cnt,
						mbr->array[part_cnt].classname,
						mbr->array[part_cnt].name,
						mbr->array[part_cnt].addrlo,
						mbr->array[part_cnt].lenlo,
						mbr->array[part_cnt].user_type);
		}
	}
}
void checkmbrs(int fd)
{
	unsigned int part_cnt = 0;
	int i;
	MBR *mbrs[MBR_COPY_NUM];
	MBR *mbr = NULL;

	memset((void *) mbrs, 0, sizeof(mbrs));
	for (i = 0; i < MBR_COPY_NUM; i++) {
		mbrs[i] = _get_mbr(fd, i, 0);
		if (mbrs[i])
			mbr = mbrs[i];
	}
	if (!mbr) {
		printf("all partition tables are bad!\n");
		for (i = 0; i < MBR_COPY_NUM; i++) {
			if (mbrs[i])
				_free_mbr(mbrs[i]);
		}
		return;
	}

	printmbr(mbr);
	for (i = 0; i < MBR_COPY_NUM; i++) {
		if (mbrs[i])
			_free_mbr(mbrs[i]);
	}
	printf("%d partitions\n", part_cnt);
}

int writembrs(int fd, char names[][MAX_NAME], __u32 start, __u32 *lens, unsigned int *user_types, int nparts, int force)
{
	unsigned int part_cnt = 0;
	int i;
	char yn = 'n';
	MBR *mbrs[MBR_COPY_NUM];
	MBR *mbr = NULL;
	FILE *backup;

	memset((void *) mbrs, 0, sizeof(mbrs));
	for (i = 0; i < MBR_COPY_NUM; i++) {
		mbrs[i] = _get_mbr(fd, i, force);
		if (mbrs[i])
			mbr = mbrs[i];
	}
	if (!mbr) {
		printf("all partition tables are bad!\n");
		for (i = 0; i < MBR_COPY_NUM; i++) {
			if (mbrs[i])
				_free_mbr(mbrs[i]);
		}
		return 0;
	}
	// back up mbr data
	backup = fopen("nand_mbr.backup", "w");
	if (!backup) {
		printf("can't open nand_mbr.backup to back up mbr data\n");
		for (i = 0; i < MBR_COPY_NUM; i++) {
			if (mbrs[i])
				_free_mbr(mbrs[i]);
		}
		return 0;
	}

	fprintf(backup, "%d ", mbr->array[0].addrlo);
	for(part_cnt = 0; part_cnt < mbr->PartCount && part_cnt < MAX_PART_COUNT; part_cnt++)
	{
		fprintf(backup, "'%s %d %d' ", mbr->array[part_cnt].name,
		                  mbr->array[part_cnt].lenlo, mbr->array[part_cnt].user_type);
	}
	fprintf(backup, "\n");
	fclose(backup);

	mbr->PartCount = nparts;
	for(i = 0; i < nparts; i++) {
		strcpy((char *)mbr->array[i].name, names[i]);
		strcpy((char *)mbr->array[i].classname, "DISK");
		memset((void *) mbr->array[i].res, 0, sizeof(mbr->array[i].res));
		mbr->array[i].user_type = user_types[i];
		mbr->array[i].ro = 0;
		mbr->array[i].addrhi = 0;
		mbr->array[i].lenhi = 0;
		mbr->array[i].addrlo = start;
		mbr->array[i].lenlo = lens[i];
		start += lens[i];
	}

	printf("\nready to write new partition tables:\n");
	printmbr(mbr);
	for (i = 0; i < MBR_COPY_NUM; i++) {
		if (mbrs[i])
			_free_mbr(mbrs[i]);
	}
	printf("%d partitions\n", part_cnt);
	printf("\nwrite new partition tables? (Y/N)\n");
	read(0, &yn, 1);
	if (yn != 'Y' && yn != 'y') {
		printf("aborting\n");
		return 0;
	}

	for (i = 0; i < MBR_COPY_NUM; i++) {
		mbr->index = i;
		// calculate new checksum
		*(__u32 *)mbr = calc_crc32((__u32 *)mbr + 1,MBR_SIZE - 4);
		lseek(fd,MBR_START_ADDRESS + MBR_SIZE*i,SEEK_SET);
		write(fd,mbr,MBR_SIZE);
	}

	if (ioctl(fd, BLKRRPART, NULL))
		perror("Failed rereading partition table");

	return 1;
}

int main (int argc, char **argv)
{
	int fd;
	int force = 0;		// force write even if magics and CRCs don't match
	int i;
	char *nand = "/dev/nand";
	char *cmd = argv[0];
	char names[MAX_PART_COUNT][MAX_NAME];
	__u32 lens[MAX_PART_COUNT];
	unsigned int user_types[MAX_PART_COUNT];
	__u32 start;

	argc--;
	argv++;

	if (argc > 0) {
		if (!strcmp(argv[0], "-f")) {
			force++;
			argc--;
			argv++;
		}
	}

	if (argc > 0) {
		nand = argv[0];
		argc--;
		argv++;
	}
	fd = open(nand, O_RDWR);
	if (fd < 0) {
		printf("usage: %s nand-device start1 'name1 len1 [usertype1]' ['name2 len2 [usertype2]'] ...\n", cmd);
		return -1;
	}

	// parse name/len arguments
	memset((void *) user_types, 0, sizeof(user_types));
	if (argc > 0) {
		if (sscanf(argv[0], "%u", &start) != 1) {
			printf("bad start argument\n");
			printf("usage: %s nand-device start1 'name1 len1 [usertype1]' ['name2 len2' [usertype2]] ...\n", cmd);
			close(fd);
			return -1;
		}
		argc--;
		argv++;
		for (i = 0; i < argc; i++) {
			if (sscanf(argv[i], "%s %d %d", names[i], &lens[i], &user_types[i]) < 2) {
				printf("bad 'name len' argument\n");
				printf("usage: %s nand-device start1 'name1 len1 [usertype1]' ['name2 len2' [usertype2]] ...\n", cmd);
				close(fd);
				return -3;
			}
		}
	}

	checkmbrs(fd);

	if (argc > MAX_PART_COUNT) {
		printf("too many partitions specified (MAX 14)\n");
		printf("usage: %s nand-device start1 'name1 len1 [usertype1]' ['name2 len2 [usertype2]'] ...\n", cmd);
		close(fd);
		return -2;
	}


	if (argc > 0) {
		if (writembrs(fd, names, start, lens, user_types, argc, force)) {
			printf("\nverifying new partition tables:\n");
			checkmbrs(fd);
			printf("rereading partition table... returned %d\n", ioctl(fd, BLKRRPART, 0));
		}
	}
	close(fd);

	return 0;
}
