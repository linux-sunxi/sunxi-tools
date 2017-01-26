/*
 * Request AArch32/AArch64 warm reset, using RVBAR and Reset Management Register
 */

rmr_request:
	ldr	r0, 1f	/* RVBAR register address */
	ldr	r1, 2f	/* desired entry point (reset vector) */
	str	r1, [r0]
	dsb
	isb	/* make sure we write the address */

	ldr	r1, 3f	/* RMR mode: bit 1 = RR, bit 0 = AA64 */
	mrc	p15, 0, r0, c12, c0, 2	/* read RMR */
	orr	r0, r0, r1		/* request warm reset (according to rmr_mode) */
	mcr	p15, 0, r0, c12, c0, 2	/* write RMR, trigger reset */

	isb
0:
	wfi
	b	0b	/* loop */

1:	.word	0	/* rvbar_reg */
2:	.word	0	/* entry_point */
3:	.word	0	/* rmr_mode (2 = AArch32, 3 = AArch64) */
