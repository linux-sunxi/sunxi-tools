/*
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
 * ARM thunk code to read the SID root key using register-based access.
 *
 * This is necessary for certain SoCs (e.g. H3), as the values read via
 * memory mapping might not be consistent. For background information see
 * https://groups.google.com/forum/#!topic/linux-sunxi/ynyIP8c61Qs
 */

SID_BASE	.req	r0
sid_key_index	.req	r1

.set	SID_PRCTL,	0x40	/* SID program/read control register */
.set	SID_PRKEY,	0x50	/* SID program key value register */
.set	SID_RDKEY,	0x60	/* SID read key value register */

.set	SID_OP_LOCK,	0xAC	/* Efuse operation lock value */
.set	SID_READ_START,	(1 << 1) /* bit 1 of SID_PRCTL, Software Read Start */
.set	SID_PG_START,	(1 << 0) /* bit 0 of SID_PRCTL, Software Program Start */

sid_read_root_key:
	ldr	SID_BASE, sid_base
	mov	sid_key_index, #0
	adr	r3, sid_result			/* result pointer */
sid_read_loop:
	mov	r2, sid_key_index, lsl #16	/* PG_INDEX value */
	orr	r2, #SID_OP_LOCK << 8	/* OP_LOCK to enable SID_READ_START */
	orr	r2, #SID_READ_START
	str	r2, [SID_BASE, #SID_PRCTL]	/* write SID_PRCTL */
sid_read_wait:
	ldr	r2, [SID_BASE, #SID_PRCTL]	/* read SID_PRCTL */
	tst	r2, #SID_READ_START	/* check if read operation completed */
	bne	sid_read_wait		/* loop while bit 1 still set */

	ldr	r2, [SID_BASE, #SID_RDKEY]	/* read SID key value */
	str	r2, [r3, sid_key_index]		/* store SID value */

	add	sid_key_index, #4
	cmp	sid_key_index, #16
	blo	sid_read_loop		/* loop while (sid_key_index < 0x10) */

	mov	r2, #0
	str	r2, [SID_BASE, #SID_PRCTL]	/* clear SID_PRCTL */
	bx	lr

sid_base:	.word 0
sid_result:	.word 0 /* receives the four "root key" 32-bit words */
		.word 0
		.word 0
		.word 0
