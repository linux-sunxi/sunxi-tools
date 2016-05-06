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

arm-none-linux-gnueabi-gcc  -g  -Os   -fno-common -ffixed-r8 -msoft-float -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  fel-boot.c -c

arm-none-linux-gnueabi-objcopy -O binary fel-boot.o fel-boot.bin

mksunxiboot fel-boot.bin fel-boot.sunxi

Install instructions:

dd if=fel-boot.sunxi of=/dev/sdX bs=1024 seek=8

*/


void _start(void)
{
	unsigned int sctlr;

	/*
	 * FEL mode fails to activate in an unpredictable way without
	 * this NOP padding. Minor changes in the code, such as checking
	 * the PC register (PC >= 0x10000) instead of SCTLR.V or doing
	 * jump instead of call to the FEL handler in the BROM sometimes
	 * break on A64 and sometimes break on A10/A13/A20. Trying to
	 * add DSB & ISB instructions and/or invalidating caches and
	 * BTB do not seem to make any difference. Only adding a bunch
	 * of NOP instructions in the beginning helps.
	 */
	asm volatile(".rept 32 \n nop \n .endr");

	asm volatile("mrc p15, 0, %0, c1, c0, 0" : "=r" (sctlr));

	if (sctlr & (1 << 13)) /* SCTLR.V */
		((void (*)(void))0xffff0020)();
	else
		((void (*)(void))0x00000020)();
}
