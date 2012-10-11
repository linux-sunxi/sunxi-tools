/*
 * Boot header to work around broken Allwinner A1x boot loaders
 *
 *  Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 *
 * This file is a workaround to broken bootloaders on Allwinner A1x
 * platform who do not provide correct machid or atags address
 *
 * Usage:
 *   load the header at 0x40007000 and change the entry point of your
 *   boot process to 0x40007000
 *
 * Detailed memory map:
 *   0x40000100  atags
 *   0x40007000  boot_head  (entry point)
 *   0x40008000  kernel
 *   0x43000000  script.bin
 * If you have a ramdisk then load it at some higher address
 */

_start:
        ldr     r0, =0
	ldr	r1, =MACHID
	ldr	r2, =0x40000100
	ldr	lr, =0x40008000
	bx	lr
