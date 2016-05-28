/*
 * Copyright (C) 2016  Bernhard Nortmann <bernhard.nortmann@web.de>
 *
 * Based on previous works
 * Copyright (C) 2016  Siarhei Siamashka <siarhei.siamashka@gmail.com>
 * Copyright (C) 2012  Henrik Nordstrom <henrik@henriknordstrom.net>
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
 */

/*
 * This file is a utility stub (bootloader code) to force the device into
 * FEL mode, by jumping directly to the corresponding (N-)BROM entry point.
 *
 * Build instructions:
 *   make fel-sdboot.sunxi
 *
 *   If needed, adjust CROSS_COMPILE and MKSUNXIBOOT according to your
 *   toolchain, e.g.
 *   make fel-sdboot.sunxi CROSS_COMPILE=armv7a-hardfloat-linux-gnueabi- \
 *   MKSUNXIBOOT=/usr/local/bin/mksunxiboot
 *
 *
 * Install instructions:
 *   dd if=fel-sdboot.sunxi of=/dev/sdX bs=1024 seek=8
 */

SCTRL	.req	r0
.equ	V_BIT, (1 << 13)

.equ	BROM_ENTRY_LOW,  0x00000020
.equ	BROM_ENTRY_HIGH, 0xFFFF0020

/*
 * In cases where insufficient padding is added by an old mksunxiboot,
 * _start may be 0x20, which means that the instruction at 0x28 could get
 * corrupted by the BROM - see https://patchwork.ozlabs.org/patch/622173/
 *
 * Apply a workaround to avoid (= skip over) that memory location.
 * _main would be at 0x30 in that particular case. With newer (properly
 * fixed) versions of mksunxiboot, this code ends up at higher addresses
 * and will be moot, but harmless.
 */
_start:
	b	_main
	nop
	nop
	nop

_main:
	mrc	p15, 0, SCTRL, c1, c0, 0
	tst	SCTRL, #V_BIT			@ test SCTRL.V
	moveq	lr, #BROM_ENTRY_LOW
	ldrne	lr, =BROM_ENTRY_HIGH
	bx	lr
