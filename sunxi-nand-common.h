/*
 * sunxi-tools/sunxi-nand-common.h
 *
 * (C) Copyright 2015
 * Eddy Beaupre <eddy@beaupre.biz>
 *
 * Derived from work made by Patrick H Wood
 *
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

#ifndef __SUNXI_NAND_COMMON_H__
#define __SUNXI_NAND_COMMON_H__

#define NAND_TYPE_A10	10
#define	NAND_TYPE_A20	20

typedef struct _SUNXI_NAND {
    unsigned char	*mbr_type;			// MBR Type
    unsigned char	*mbr_magic;			// magic number
    unsigned int	 mbr_version;			// version
    unsigned int	 mbr_max_partitions;		// max part count
    unsigned int	 mbr_name_len;			// max length of device name
    unsigned int	 mbr_copy;			// mbr backup count
    unsigned int	 mbr_start;			// mbr start address
    unsigned int	 mbr_size;			// mbr size
    unsigned int	 mbr_reserved;			// mbr reserved space
    int		(*mbr_check)(void *, int);		// function to validate mbr
    void	*(*mbr_get)(void *, int);		// function to retreive mbr data
    void	(*mbr_set)(void *, int, void *); 	// function to modify mbr data
    void	*(*part_get)(void *, int, int);		// function to retreive partition info
    void	(*part_set)(void *, int, int, void *);	// function to modify partition info
    void	(*part_copy)(void *, int, int);		// function to copy a partition to another slot
    void	*next;					// link to the next definition
} SUNXI_NAND;

typedef struct _SUNXI_PARTS {
    int			cmd;
    long 		id;
    unsigned char	*classname;
    unsigned char 	*name;
    long 		start;
    long 		len;
    long 		type;
    long		ro;
    void		*prev;
    void 		*next;
} SUNXI_PARTS;

typedef struct _SUNXI_CRC32 {
    unsigned int CRC;
    unsigned int CRC_32_Tbl[256];
} SUNXI_CRC32;

#define	MBR_CHECK_MAGIC		0x01
#define MBR_CHECK_VERSION	0x02
#define MBR_CHECK_CRC32		0x03

#define SUNXI_MBR_CRC32		0x01
#define SUNXI_MBR_VERSION	0x02
#define SUNXI_MBR_MAGIC		0x03
#define	SUNXI_MBR_COPY		0x04
#define SUNXI_MBR_INDEX		0x05
#define	SUNXI_MBR_PARTCOUNT	0x06
#define	SUNXI_MBR_PARTINFO	0x07
#define SUNXI_MBR_RESERVED	0x08
#define SUNXI_MBR_STAMP		0x09

#define SUNXI_PART_ADDRHI	0x01
#define SUNXI_PART_ADDRLO	0x02
#define SUNXI_PART_LENHI	0x03
#define SUNXI_PART_LENLO	0x04
#define SUNXI_PART_CLASSNAME	0x05
#define SUNXI_PART_NAME		0x06
#define SUNXI_PART_USERTYPE	0x07
#define SUNXI_PART_KEYDATA	0x08
#define SUNXI_PART_RO		0x09
#define SUNXI_PART_RESERVED	0x10

void *voidfree(void **ptr);
char *strfree(char **ptr);
void *malloc_clean(size_t size);
char *strduplicate(char **dest, const char *src);
void underline(char *s, int c);
void printf_underline(int c, const char *format, ...);
char *sprintf_alloc(char **str, const char *format, ...);

SUNXI_NAND *sunxi_nand_add(SUNXI_NAND **ni, char *type, char *magic, int version, int partitions, int copy, int len, int start, int size, void *mbr_check, void *mbr_get, void *mbr_set, void *part_get, void *part_set, void *part_copy, int reserved);
SUNXI_NAND *sunxi_nand_free(SUNXI_NAND **ni);
void printNandInfo(SUNXI_NAND *ni);

SUNXI_PARTS *sunxi_parts_add(SUNXI_PARTS **sp, int cmd, long id, unsigned char *classname, unsigned char *name, long start, long len, long type, long ro);
SUNXI_PARTS *sunxi_parts_free(SUNXI_PARTS **sp);

void usage(char *program);
void **mbr_alloc(SUNXI_NAND *ni);
void mbr_free(SUNXI_NAND *ni, void **mbr);
int mbr_compare(SUNXI_NAND *ni, void **mbr1, void **mbr2);

void init_crc32(void);
unsigned int calc_crc32(void *buffer, unsigned int length);

#endif    //__SUNXI_NAND_COMMON_H__
