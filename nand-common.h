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

#include "types.h"

extern int nand_part_a10 (int argc, char **argv, const char *cmd, int fd, int force);
extern int nand_part_a20 (int argc, char **argv, const char *cmd, int fd, int force);
extern int checkmbrs_a10 (int fd);
extern int checkmbrs_a20 (int fd);
extern void usage (const char *cmd);
extern __u32 calc_crc32(void * buffer, __u32 length);
