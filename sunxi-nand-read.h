/*
 * sunxi-tools/nand-read.h
 *
 * (C) Copyright 2015
 * Eddy Beaupre <eddy@beaupre.biz>
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

#ifndef __SUNXI_NAND_READ_H__
#define __SUNXI_NAND_READ_H__

ssize_t mbr_read_fd(int fd, int start, int size, int num);
void **mbr_read_fd_all(int fd, SUNXI_NAND *ni);
void **mbr_copy_all(SUNXI_NAND *ni, void **mbr);
ssize_t mbr_read_file(char *file, SUNXI_NAND *ni, void *mbr, unsigned int mbr_id);
void **mbr_read_file_all(char *file, SUNXI_NAND *ni);
int mbr_check(SUNXI_NAND *ni, void *mbr);
int mbr_check_all(SUNXI_NAND *ni, void **mbr);
void mbr_print(SUNXI_NAND *ni, void **mbr);
SUNXI_NAND *mbr_type_fd(int fd, SUNXI_NAND *ni);
SUNXI_NAND *mbr_type_file(char * file, SUNXI_NAND *ni);
SUNXI_NAND *mbr_type(char *name, SUNXI_NAND *ni);

#endif    //__SUNXI_NAND_READ_H__