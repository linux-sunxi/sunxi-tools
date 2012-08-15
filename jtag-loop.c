/*
 * (C) Copyright 2012 Jens Andersen <jens.andersen@gmail.com>
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
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

arm-none-linux-gnueabi-gcc  -g -fno-common -ffixed-r8 -msoft-float -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder -Os jtag-loop.c -c

arm-none-linux-gnueabi-objcopy -O binary jtag-loop.o jtag-loop.bin

mksunxiboot jtag-loop.bin jtag-loop.sunxi
*/

void _start(void)
{
	*(volatile unsigned long *)0x01c208b4 = 0x00444444;
	while(1);
}
