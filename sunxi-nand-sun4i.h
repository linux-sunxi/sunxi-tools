/*
 * sunxi-tools/nand-sun4i.h
 *
 * (C) Copyright 2015
 * Eddy Beaupre <eddy@beaupre.biz>
 *
 * Derived from drivers/block/sun4i_nand/nfd/mbr.h
 *
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

#ifndef __SUNXI_NAND_SUN4I_H__
#define __SUNXI_NAND_SUN4I_H__

/*
 * Sun4i (A10) Nand Definitions
 */

#define SUN4I_MBR_NAME		"SUN4I"							//mbr name
#define SUN4I_MBR_MAGIC		"softw311"						//magic id
#define SUN4I_MBR_MAGIC_LEN	8							//magic id len
#define SUN4I_MBR_VERSION	0x100

#define SUN4I_MBR_PART_COUNT	15							//max part count
#define SUN4I_MBR_COPY_NUM	4							//mbr backup count

#define SUN4I_MBR_NAME_LEN	12							//max length of device name

#define SUN4I_MBR_START_ADDRESS	0x0							//mbr start address
#define SUN4I_MBR_SIZE		1024							//mbr size
#define SUN4I_MBR_RESERVED	(SUN4I_MBR_SIZE - 20 - (SUN4I_MBR_PART_COUNT * 64))	//mbr reserved space
#define SUN4I_MBR_PART_RES	16							//mbr partition reserved space

/* Sun4i (A10) part info */
typedef struct _SUN4I_PARTITION {
    unsigned int	addrhi;				//start address high 32 bit
    unsigned int	addrlo;				//start address low 32 bit
    unsigned int	lenhi;				//size high 32 bit
    unsigned int	lenlo;				//size low 32 bit
    unsigned char	classname[SUN4I_MBR_NAME_LEN];	//major device name
    unsigned char	name[SUN4I_MBR_NAME_LEN];	//minor device name
    unsigned int	user_type;			//user type
    unsigned int	ro;				//read only flag
    unsigned char	res[SUN4I_MBR_PART_RES];	//reserved (pack to 64 bytes)
} __attribute__ ((packed))SUN4I_PARTITION;

/* Sun4i (A10) mbr info */
typedef struct _SUN4I_MBR {
    unsigned int	crc32;				//crc, from byte 4 to mbr tail
    unsigned int	version;			//version
    unsigned char	magic[SUN4I_MBR_MAGIC_LEN];	//magic number
    unsigned char	copy;				//mbr backup count
    unsigned char	index;				//current part no
    unsigned short	PartCount;			//part counter
    SUN4I_PARTITION	array[SUN4I_MBR_PART_COUNT];	//part info
    unsigned char	res[SUN4I_MBR_RESERVED];	//reserved space
} __attribute__ ((packed))SUN4I_MBR;

extern int sun4i_mbr_check(void *_mbr, int test);
extern void *sun4i_mbr_get(void *_mbr, int info);
extern void sun4i_mbr_set(void *_mbr, int info, void *data);
extern void *sun4i_part_get(void *_mbr, int partition, int info);
extern void sun4i_part_set(void *_mbr, int partition, int info, void *data);
void sun4i_part_copy(void *_mbr, int dst, int src);

#endif    //__SUNXI_NAND_SUN4I_H__
