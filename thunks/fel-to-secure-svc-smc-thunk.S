/* secure-FEL thunk using a temporary monitor-mode SMC handler. */

.arm

BUF1		.req	r0
BUF2		.req	r1
TMP1		.req	r2
TMP2		.req	r3
SWAPTBL		.req	r4
ORIG_VECTOR	.req	r5
BUFSIZE		.req	r6
ENTRY_ADDR	.req	r8
MON_VECTOR	.req	r9
SAVED_SP	.req	r10
SAVED_LR	.req	r11

entry_point:
	b	setup_stack

stack_begin:
	.space	32, 0xff
stack_end:
	nop

	/* Walk the table and swap all buffers. */
swap_all_buffers:
	adr	SWAPTBL,   appended_data + 16
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

setup_stack:
	ldr	ENTRY_ADDR, appended_data
	ldr	MON_VECTOR, appended_data + 4
	adr	BUF1,      stack_end
	str	sp,        [BUF1, #-4]!
	mov	sp,        BUF1
	mrs	TMP1,      cpsr
	push	{TMP1, lr}

	/* Disable IRQ and FIQ. */
	orr	TMP1,      #0xc0
	msr	cpsr_c,    TMP1

	/* Check if the instructions or data cache is enabled. */
	mrc	p15, 0, TMP1, c1, c0, 0
	tst	TMP1,      #(1 << 2)
	tsteq	TMP1,      #(1 << 12)
	bne	return_to_fel_noswap

	bl	swap_all_buffers
	bl	call_entry
	b	return_to_secure_fel

return_to_secure_fel:
	bl	swap_all_buffers

return_to_fel_noswap:
	pop	{TMP1, lr}
	msr	cpsr_c,    TMP1
	ldr     sp,        [sp]
	bx	lr

call_entry:
	/* Parameterize the copied monitor handler. */
	ldr	TMP2,      appended_data + 8
	adr	TMP1,      monitor_gicc_ctlr
	str	TMP2,      [TMP1]
	ldr	TMP2,      appended_data + 12
	adr	TMP1,      monitor_gicd_ctlr
	str	TMP2,      [TMP1]

	/* Temporarily redirect only the monitor SMC vector. */
	ldr	ORIG_VECTOR, [MON_VECTOR, #8]
	adr	TMP1,      monitor_smc_handler
	add	TMP2,      MON_VECTOR, #16	/* PC at the SMC vector */
	sub	TMP1,      TMP1, TMP2
	mov	TMP1,      TMP1, asr #2
	ldr	TMP2,      branch_offset_mask
	and	TMP1,      TMP1, TMP2
	ldr	TMP2,      branch_opcode
	orr	TMP1,      TMP1, TMP2
	str	TMP1,      [MON_VECTOR, #8]

	/*
	 * The handler returns from the SMC exception into secure SVC, using the
	 * banked SP/LR copied from this non-secure SVC call frame.
	 */
	mcr	p15, 0, TMP1, c7, c10, 4	/* CP15DSB */
	mrc	p15, 0, TMP1, c0, c0, 0		/* read MIDR */
	and	TMP1, TMP1, #(0xf << 16)	/* architecture */
	cmp	TMP1, #(0x6 << 16)		/* ARMv5TEJ */
	mcrgt	p15, 0, TMP1, c7, c5, 4		/* CP15ISB, if > ARMv5TEJ */
	mov	SAVED_SP,  sp
	mov	SAVED_LR,  lr
	.word	0xe1600070			/* smc	#0 */
	/* The custom handler switches to secure SVC and does not return here. */
	bx	ENTRY_ADDR

monitor_smc_handler:
	str	ORIG_VECTOR, [MON_VECTOR, #8]
	mov	TMP1,      #0
	mcr	p15, 0, TMP1, c1, c1, 0		/* SCR = secure */
	mcr	p15, 0, TMP1, c12, c0, 1	/* MVBAR = 0 */

	ldr	TMP1,      monitor_gicc_ctlr
	mov	TMP2,      #0xf
	str	TMP2,      [TMP1]
	mov	TMP2,      #0xf8
	str	TMP2,      [TMP1, #4]
	ldr	TMP1,      monitor_gicd_ctlr
	mov	TMP2,      #3
	str	TMP2,      [TMP1]

	.word	0xe123f30a			/* msr	SP_svc, SAVED_SP */
	.word	0xe122f30b			/* msr	LR_svc, SAVED_LR */
	.word	0xf57ff06f			/* isb */
	mrs	TMP1,      cpsr
	bic	TMP1,      TMP1, #0x1f
	orr	TMP1,      TMP1, #0x13
	msr	cpsr_fc,   TMP1			/* switch to secure-SVC mode */
	bx	ENTRY_ADDR

monitor_gicc_ctlr:
	.word	0
monitor_gicd_ctlr:
	.word	0
branch_offset_mask:
	.word	0x00ffffff
branch_opcode:
	.word	0xea000000

appended_data:
/*
 * The appended data uses the following format:
 *
 *     struct {
 *         uint32_t          entry_addr;
 *         uint32_t          monitor_vector_addr;
 *         uint32_t          gicc_ctlr_addr;
 *         uint32_t          gicd_ctlr_addr;
 *         sram_swap_buffers swaptbl[];
 *     };
 */
