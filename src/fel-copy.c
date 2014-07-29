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

arm-none-linux-gnueabi-gcc  -g  -Os   -fno-common -ffixed-r8 -msoft-float -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  fel-copy.c -c

arm-none-linux-gnueabi-objcopy -O binary fel-copy.o fel-copy.bin

Parameters:
	0x2100	Destination address
	0x2104	Source address
	0x2108	Length

Source address is updated, allowing repeated copy to same destination
*/

#define CONFIG_BASE 0x2100

void copy(void)
{
	unsigned long *b = (void *)CONFIG_BASE;
	unsigned long **ptr = (void *)b++;
	unsigned long *a = *ptr;
	unsigned long i = *b++;
	while (i--) {
		*b++ = *a++;
	}
	*ptr = a;
}

