/*
 * copy "upwards", increasing destination and source addresses
 */
fel_memcpy_up:
	ldr	r0, 1f		/* dst_addr */
	ldr	r1, 2f		/* src_addr */
	ldr	r2, 3f		/* bytes */
	sub	r3, r1, r0
	tst	r3, #3		/* test LSB for word alignment */
	bne	copyup_tail	/* unaligned access, copy byte-wise */
copyup_head:
	tst	r1, #3		/* word boundary? */
	beq	copyup_loop
	ldrb	r3, [r1], #1	/* load and post-inc */
	strb	r3, [r0], #1	/* store and post-inc */
	subs	r2, #1		/* r2 -= 1 */
	bpl	copyup_head
	bx	lr		/* early return on small byte count (r2 < 0) */
copyup_loop:
	subs	r2, #4		/* r2 -= 4 */
	ldrpl	r3, [r1], #4	/* load and post-inc */
	strpl	r3, [r0], #4	/* store and post-inc */
	bpl	copyup_loop	/* while (r2 >= 0) */
	add	r2, #4		/* r2 = remaining byte count */
copyup_tail:
	subs	r2, #1		/* r2 -= 1 */
	bxmi	lr		/* return on (r2 < 0) */
	ldrb	r3, [r1], #1	/* load and post-inc */
	strb	r3, [r0], #1	/* store and post-inc */
	b	copyup_tail

1:	.word	0	/* dst_addr */
2:	.word	0	/* src_addr */
3:	.word	0	/* bytes */

/*
 * copy "downwards", using base-relative indexing
 */
fel_memcpy_down:
	ldr	r0, 1f		/* dst_addr */
	ldr	r1, 2f		/* src_addr */
	ldr	r2, 3f		/* bytes */
	sub	r3, r0, r1
	tst	r3, #3		/* test LSB for word alignment */
	bne	copydn_tail	/* unaligned access, copy byte-wise */
copydn_head:
	add	r3, r1, r2	/* r3 = r1 + r2, for alignment check */
	tst	r3, #3		/* word boundary? */
	beq	copydn_loop
	subs	r2, #1		/* r2 -= 1 */
	bxmi	lr		/* early return on small byte count (r2 < 0) */
	ldrb	r3, [r1, r2]	/* load byte */
	strb	r3, [r0, r2]	/* store byte */
	b	copydn_head
copydn_loop:
	subs	r2, #4		/* r2 -= 4 */
	ldrpl	r3, [r1, r2]	/* load word */
	strpl	r3, [r0, r2]	/* store word */
	bpl	copydn_loop	/* while (r2 >= 0) */
	add	r2, #4		/* r2 = remaining byte count */
copydn_tail:
	subs	r2, #1		/* r2 -= 1 */
	bxmi	lr		/* return on (r2 < 0) */
	ldrb	r3, [r1, r2]	/* load byte */
	strb	r3, [r0, r2]	/* store byte */
	b	copydn_tail

1:	.word	0	/* dst_addr */
2:	.word	0	/* src_addr */
3:	.word	0	/* bytes */
