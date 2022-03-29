/*
 * Copyright © 2015 Siarhei Siamashka <siarhei.siamashka@gmail.com>
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

.arm

BUF1		.req	r0
BUF2		.req	r1
TMP1		.req	r2
TMP2		.req	r3
SWAPTBL		.req	r4
FULLSIZE	.req	r5
BUFSIZE		.req	r6
CHECKSUM	.req	r7
SPL_ADDR	.req	r8

entry_point:
	b	setup_stack

stack_begin:
	.space	32, 0xff
stack_end:
	nop

	/* A function, which walks the table and swaps all buffers */
swap_all_buffers:
	adr	SWAPTBL,   appended_data + 4
swap_next_buffer:
	ldr	BUF1,      [SWAPTBL],  #4
	ldr	BUF2,      [SWAPTBL],  #4
	ldr	BUFSIZE,   [SWAPTBL],  #4
	cmp	BUFSIZE,   #0
	bxeq    lr
swap_next_word:
	ldr	TMP1,      [BUF1]
	ldr	TMP2,      [BUF2]
	subs	BUFSIZE,   BUFSIZE,    #4
	str	TMP1,      [BUF2],     #4
	str	TMP2,      [BUF1],     #4
	bne	swap_next_word
	b	swap_next_buffer

setup_stack: /* Save the original SP, LR and CPSR to stack */
	ldr	SPL_ADDR,  appended_data
	adr	BUF1,      stack_end
	str	sp,        [BUF1, #-4]!
	mov	sp,        BUF1
	mrs	TMP1,      cpsr
	push	{TMP1, lr}

	/* Disable IRQ and FIQ */
	orr	TMP1,      #0xc0
	msr	cpsr_c,    TMP1

	/* Check if the instructions or data cache is enabled */
	mrc	p15, 0, TMP1, c1, c0, 0
	tst	TMP1,      #(1 << 2)
	tsteq	TMP1,      #(1 << 12)
	bne	cache_is_unsupported

	bl	swap_all_buffers

verify_checksum:
	ldr	CHECKSUM,  checksum_seed
	mov	BUF1,      SPL_ADDR
	ldr     FULLSIZE,  [BUF1, #16]
check_next_word:
	ldr	TMP1,      [BUF1],   #4
	subs	FULLSIZE,  FULLSIZE, #4
	add	CHECKSUM,  CHECKSUM, TMP1
	bne	check_next_word

	ldr	TMP1,      [SPL_ADDR, #12]
	subs	CHECKSUM,  CHECKSUM, TMP1, lsl #1
	bne	checksum_is_bad

	/* Change 'eGON.BT0' -> 'eGON.FEL' */
	ldr	TMP1,	   egon_fel_str
	str	TMP1,      [SPL_ADDR, #8]

	/*
	 * Call the SPL code, but before that make sure the CPU sees the
	 * recently uploaded code. This requires a DSB and ISB.
	 * The "dsb" and "isb" *instructions* are not available in ARMv5TE,
	 * but at least for DSB we can use the CP15 register encoding. This
	 * works for ARMv7 and v8 as well, because we have checked our SCTLR
	 * before (in fel.c), so we know that CP15BEN is set.
	 * The ARM926 core does not implement ISB, instead the TRM recommends
	 * just a branch to achieve the same "flush the pipeline" effect.
	 * As just this is not sufficient for later cores, check the MIDR
	 * register, and do the DSB only for ARMv6 or later.
	 * The input register for the CP15 instruction is ignored.
	 */
	mcr	p15, 0, TMP1, c7, c10, 4	/* CP15DSB */
	mrc	p15, 0, TMP1, c0, c0, 0		/* read MIDR */
	and	TMP1, TMP1, #(0xf << 16)	/* architecture */
	cmp	TMP1, #(0x6 << 16)		/* ARMv5TEJ */
	mcrgt	p15, 0, TMP1, c7, c5, 4		/* CP15ISB, if > ARMv5TEJ */
	blx	SPL_ADDR

	/* Return back to FEL */
	b	return_to_fel

cache_is_unsupported:
	/* Bail out if cache is enabled and change 'eGON.BT0' -> 'eGON.???' */
	ldr	TMP1,	   cache_enabled_str
	str	TMP1,      [SPL_ADDR, #8]
	b	return_to_fel_noswap

checksum_is_bad:
	/* The checksum test failed, so change 'eGON.BT0' -> 'eGON.BAD' */
	ldr	TMP1,	   checksum_failed_str
	str	TMP1,      [SPL_ADDR, #8]

return_to_fel:
	bl	swap_all_buffers
return_to_fel_noswap:
	pop	{TMP1, lr}
	msr	cpsr_c,    TMP1 /* Restore the original CPSR */
	ldr     sp,        [sp]
	bx	lr

checksum_seed:
	.word	0x5f0a6c39
egon_fel_str:
	.ascii	".FEL"
cache_enabled_str:
	.ascii	".???"
checksum_failed_str:
	.ascii	".BAD"

appended_data:
/*
 * The appended data uses the following format:
 *
 *     struct {
 *         uint32_t          spl_addr;
 *         sram_swap_buffers swaptbl[];
 *     };
 *
 * More details about the 'spl_addr' variable and the 'sram_swap_buffers'
 * struct can be found in the 'fel.c' source file.
 */
