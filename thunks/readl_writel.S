/*
 * Thunk code for buffered 'long' (i.e. 32-bit) read and write operations
 */

.equ	MAX_WORDS, 0x100 - 12

fel_readl_n:
	ldr	r0, 1f	/* read_addr */
	adr	r1, 3f	/* read_data */
	ldr	r2, 2f	/* read_count */
	/* limit word count to a maximum value */
	cmp	r2, #MAX_WORDS
	movgt	r2, #MAX_WORDS
read_loop:
	subs	r2, #1
	bxmi	lr
	ldr	r3, [r0], #4
	str	r3, [r1], #4
	b	read_loop

1:	.word	0	/* read_addr */
2:	.word	0	/* read_count */
3:	.word	0	/* read_data */

fel_writel_n:
	ldr	r0, 1f	/* write_addr */
	adr	r1, 3f	/* write_data */
	ldr	r2, 2f	/* write_count */
	/* limit word count to a maximum value */
	cmp	r2, #MAX_WORDS
	movgt	r2, #MAX_WORDS
write_loop:
	subs	r2, #1
	bxmi	lr
	ldr	r3, [r1], #4
	str	r3, [r0], #4
	b	write_loop

1:	.word	0	/* write_addr */
2:	.word	0	/* write_count */
3:	.word	0	/* write_data */
