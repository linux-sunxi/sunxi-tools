 /*
 * Copyright (C) 2024 Marek Kraus <gamelaster@outlook.com>
 * Copyright (C) 2016 Bernhard Nortmann <bernhard.nortmann@web.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
 * ARM thunk code to write the data to SID using register-based access.
 */

SID_BASE			.req	r0
sid_offset			.req	r1
sid_data_offset		.req	r3

.set	SID_PRCTL,	0x40	// SID program/read control register
.set	SID_PRKEY,	0x50	// SID program key value register

.set	SID_OP_LOCK,    0xAC	 // magic value to prevent accidental programming

.set	SID_PRCTL_READ_BIT,		(1 << 1)	// bit 1 of SID_PRCTL, Software Read Start
.set	SID_PRCTL_WRITE_BIT,	(1 << 0)	// bit 0 of SID_PRCTL, Software Program Start

.set	SID_PRCTL_OP_LOCK_POS,	8	// bits 8 - 15
.set	SID_PRCTL_OFFSET_POS,	16	// bits 16 - 23			

sid_write:
	ldr	SID_BASE, sid_base
	ldr	sid_offset, offset
	adr sid_data_offset, sid_data
sid_write_loop:
	ldr r2, [sid_data_offset], #4	// load SID data to write into r2, and increment pointer +4 bytes
	str r2, [SID_BASE, #SID_PRKEY]	// store data from r2 to SID_PRKEY register
	mov r2, sid_offset, lsl #SID_PRCTL_OFFSET_POS	// shift offset where to write data value to correct pos
	orr r2, #SID_OP_LOCK << 8	// shift lock magic value to correct pos
	orr r2, #SID_PRCTL_WRITE_BIT	// set write bit
	str r2, [SID_BASE, #SID_PRCTL]	// write the PRCTL into the register from r2
sid_write_wait:
	ldr r2, [SID_BASE, #SID_PRCTL]	// read SID_PRCTL register to r2
	tst r2, #SID_PRCTL_WRITE_BIT	// check if write bit is still 1
	bne sid_write_wait	// if it is still 1, the write is in-progress, so loop until it is 0

	add sid_offset, #4	// incremet sid write offset
	ldr r2, end		
	cmp sid_offset, r2
	blo sid_write_loop
	mov	r2, #0
	str	r2, [SID_BASE, #SID_PRCTL]
	bx	lr

sid_base:	.word 0x0
offset:		.word 0x0
end:		.word 0x0
sid_data:
