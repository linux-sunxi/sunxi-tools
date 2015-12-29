/*
 * sunxi-tools/nand-sun7i.h
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

#ifndef __SUNXI_NAND_SUN7I_H__
#define __SUNXI_NAND_SUN7I_H__

/*
 * Sun7i (A20) Nand Definitions
 */

#define SUN7I_MBR_NAME		"SUN7I"							//mbr_name
#define SUN7I_MBR_MAGIC		"softw411"						//magic id
#define SUN7I_MBR_MAGiC_LEN	8							//magic id len
#define SUN7I_MBR_VERSION	0x200

#define SUN7I_MBR_PART_COUNT	120							//max part count
#define SUN7I_MBR_COPY_NUM	4							//mbr backup count

#define SUN7I_MBR_NAME_LEN	16							//Max length of device name

#define SUN7I_MBR_START_ADDRESS	0x0							//mbr start address
#define SUN7I_MBR_SIZE		1024*16							//mbr size
#define SUN7I_MBR_RESERVED	(SUN7I_MBR_SIZE - 32 - (SUN7I_MBR_PART_COUNT * 128))	//mbr reserved space
#define SUN7I_MBR_PART_RES	68							//mbr partition reserved space

/* SUN7I part info */
typedef struct _SUN7I_PARTITION {
    unsigned int	addrhi;				//start address high 32 bit
    unsigned int	addrlo;				//start address low 32 bit
    unsigned int	lenhi;				//size high 32 bit
    unsigned int	lenlo;				//size low 32 bit
    unsigned char	classname[SUN7I_MBR_NAME_LEN];	//major device name
    unsigned char	name[SUN7I_MBR_NAME_LEN];	//minor device name
    unsigned int	user_type;			//user type
    unsigned int	keydata;			//key data
    unsigned int	ro;				//read only flag
    unsigned char	res[SUN7I_MBR_PART_RES];	//reserved (pack to 128 bytes)
} __attribute__ ((packed))SUN7I_PARTITION;

/* SUN7I mbr info */
typedef struct _SUN7I_MBR {
    unsigned int	crc32;				//crc, from byte 4 to mbr tail
    unsigned int	version;			//version
    unsigned char	magic[SUN7I_MBR_MAGiC_LEN];	//magic number
    unsigned int	copy;				//mbr backup count
    unsigned int	index;				//current part no
    unsigned int	PartCount;			//part counter
    unsigned int	stamp[1];			//stamp
    SUN7I_PARTITION	array[SUN7I_MBR_PART_COUNT];	//part info
    unsigned char	res[SUN7I_MBR_RESERVED];	//reserved space
} __attribute__ ((packed))SUN7I_MBR;

extern int sun7i_mbr_check(void *_mbr, int test);
extern void *sun7i_mbr_get(void *_mbr, int info);
extern void sun7i_mbr_set(void *_mbr, int info, void *data);
extern void *sun7i_part_get(void *_mbr, int partition, int info);
extern void sun7i_part_set(void *_mbr, int partition, int info, void *data);
void sun7i_part_copy(void *_mbr, int dst, int src);

#endif    //__SUNXI_NAND_SUN7I_H__
