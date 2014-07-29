/*
 * (C) Copyright 2012 Jens Andersen <jens.andersen@gmail.com>
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

arm-none-linux-gnueabi-gcc  -g -fno-common -ffixed-r8 -msoft-float -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  jtag-loop.S -c

arm-none-linux-gnueabi-objcopy -O binary jtag-loop.o jtag-loop.bin

mksunxiboot jtag-loop.bin jtag-loop.sunxi
*/

.file "fel-loop.S"
.global entry
.text 
.code 32
.section ".start", "ax"

entry:
    ldr r0,=0x01c208b4
	ldr r1,=0x00444444
	str r1, [r0]
    b .
