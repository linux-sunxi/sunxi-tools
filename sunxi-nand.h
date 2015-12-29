/*
 * sunxi-tools/sunxi-nand.h
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

#ifndef __SUNXI_NAND_H__
#define __SUNXI_NAND_H__

#define MAX_PART_COUNT	120
#define MAX_NAME	16

#define CMD_BACKUP	0x0001
#define CMD_RESTORE	0x0002
#define CMD_3		0x0004
#define CMD_4		0x0008
#define CMD_CLEAN	0x0010
#define CMD_FORCE	0x0020
#define CMD_PART	0x0040
#define CMD_HELP	0x0080

#define	OPT_START	0
#define OPT_LEN		1
#define OPT_CLASS	2
#define OPT_NAME	3
#define OPT_TYPE	4
#define OPT_RO		5
#define OPT_ADD		6
#define OPT_DELETE	7
#define OPT_INSERT	8

mode_t file_type(char *file);
void sync_part(char *file);
unsigned long file_size(char *file);
unsigned long device_size(char *file);

#endif    //__SUNXI_NAND_H__
