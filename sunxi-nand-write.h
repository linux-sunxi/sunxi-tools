/*
 * sunxi-tools/nand-write.h
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

#ifndef __SUNXI_NAND_WRITE_H__
#define __SUNXI_NAND_WRITE_H__

void mbr_initialize(SUNXI_NAND *ni, void **mbr);
void mbr_part_count_set(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int count);
void mbr_part_count_set_all(SUNXI_NAND *ni, void **mbr, unsigned int count);
void mbr_part_set(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int part_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype);
void mbr_part_set_all(SUNXI_NAND *ni, void **mbr, unsigned int part_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype);
void mbr_part_add(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype);
void mbr_part_add_all(SUNXI_NAND *ni, void **mbr, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype);
ssize_t mbr_write_fd(int fd, SUNXI_NAND *ni, void **mbr, unsigned int mbr_id);
ssize_t mbr_write_fd_all(int fd, SUNXI_NAND *ni, void **mbr);
ssize_t mbr_write_file(char *file, SUNXI_NAND *ni, void **mbr, unsigned int mbr_id);
ssize_t mbr_write_file_all(char *file, SUNXI_NAND *ni, void **mbr);

#endif    //__SUNXI_NAND_WRITE_H__
