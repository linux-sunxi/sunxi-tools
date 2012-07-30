/*
 * (C) Copyright 2011 Henrik Nordstrom <henrik@henriknordstrom.net>
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
 */

/*

Build instructions:

arm-none-linux-gnueabi-gcc  -g  -Os   -fno-common -ffixed-r8 -msoft-float -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  fel-pio.c -c

arm-none-linux-gnueabi-objcopy -O binary fel-pio.o fel-pio.bin

arm-none-linux-gnueabi-nm fel-pio.o

*/

void pio_to_sram(void)
{
	unsigned long *a = (void *)0x1c20800;
	unsigned long *b = (void *)0x3000;
	int i = 0x228 >> 2;
	while (i--) {
		*b++ = *a++;
	}
}

void sram_to_pio(void)
{
	unsigned long *a = (void *)0x1c20800;
	unsigned long *b = (void *)0x3000;
	int i = 0x228 >> 2;
	while (i--) {
		*a++ = *b++;
	}
}
