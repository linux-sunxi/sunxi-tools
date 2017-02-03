/*
 * Thunk code to assist with bitwise operations (set/clear) via FEL
 */

fel_clrsetbits_le32:
	ldr	r0, 1f		/* address */
	ldr	r1, [r0]	/* load value */
	ldr	r2, 2f		/* clrbits mask */
	bic	r1, r2		/* clear bits, post-increment r1 */
	ldr	r2, 3f		/* setbits mask */
	orr	r1, r2		/* set bits (logical "or") */
	str	r1, [r0]	/* store result */
	bx	lr

1:	.word	0	/* addr */
2:	.word	0	/* clrbits (= bits to clear) */
3:	.word	0	/* setbits (= bits to set) */
