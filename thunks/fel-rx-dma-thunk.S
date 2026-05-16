/*
 * FEL RX DMA thunk.
 */

.arm

.equ	L1_TABLE_ENTRIES,		4096
.equ	L2_TABLE_ENTRIES,		256
.equ	BROM_PAGE_WORDS,		1024
.equ	L1_SECTION_FLAGS,		0x00000de2
.equ	L1_SECTION_NORMAL,		0x00001000
.equ	L1_COARSE_DESCRIPTOR_FLAGS,	0x1e1
.equ	L2_SMALL_PAGE_FLAGS,		0x72

.equ	USB_INDEX,			0x0042
.equ	USB_VEND0,			0x0043
.equ	USB_INTRRXE,			0x004a
.equ	USB_INTRUSB,			0x004c
.equ	USB_INTRUSBE,			0x0050
.equ	USB_RXCSR,			0x0086
.equ	USB_RXCSR_HI,			0x0087
.equ	USB_DMA_INTE,			0x0500
.equ	USB_DMA_INTS,			0x0504
.equ	USB_DMA_CHAN_CFN0,		0x0540
.equ	USB_DMA_SDRAM_ADD0,		0x0544
.equ	USB_DMA_BC0,			0x0548
.equ	USB_DMA_RESIDUAL_BC0,		0x054c
.equ	USB_DMA_CHAN_ENABLE,		0x80000000
.equ	USB_DMA_RX_DIR,			0x00000010
.equ	USB_DMA_EP_LEN_512,		0x02000000
.equ	USB_DMA_CHAN_CFG_RX_512,		USB_DMA_CHAN_ENABLE | USB_DMA_RX_DIR | USB_DMA_EP_LEN_512
.equ	USB_RXCSR_DMA_REQ_MODE_HI,	0x08
.equ	USB_RXCSR_DMA_REQ_EN_HI,		0x20
.equ	USB_RXCSR_AUTO_CLEAR_HI,		0x80
.equ	USB_RXCSR_DMA_BITS_HI,		USB_RXCSR_DMA_REQ_MODE_HI | USB_RXCSR_DMA_REQ_EN_HI | USB_RXCSR_AUTO_CLEAR_HI
.equ	USB_RXCSR_RX_PKT_READY,		0x01
.equ	USB_INTR_SOF,			0x08
.equ	FEL_RX_DMA_IRQ_GICV2,		0x01
.equ	FEL_RX_DMA_IRQ_GICV3,		0x02
.equ	WFI_WAIT_DMA,			0x01
.equ	GICC_IAR,			0x000c
.equ	GICC_EOIR,			0x0010
.equ	GICD_ICPENDR,			0x0280
.equ	GIC_SPURIOUS_INTID,		0x03fc

	.global fel_rx_dma_thunk
fel_rx_dma_thunk:
	.word	install_mmu_remap - fel_rx_dma_thunk
	.word	fel_rx_dma_params - fel_rx_dma_thunk

fel_rx_dma_params_ref:
	.word	fel_rx_dma_params - fel_rx_dma_params_ref

install_mmu_remap:
	@ Build a temporary first-level translation table. Most sections keep
	@ the BROM's physical mapping, while the section containing the BROM
	@ FIFO-copy helper is replaced with a second-level table. The helper's
	@ page is shadowed, and a spare page in the same section is mapped to
	@ this thunk so the patched branch is always in range.
	adr	r0, fel_rx_dma_params_ref
	ldr	r1, [r0]
	add	r0, r0, r1
	ldr	r4, [r0, #0x00]
	ldr	r5, [r0, #0x04]
	ldr	r9, [r0, #0x08]
	ldr	r7, [r0, #0x0c]
	mov	r6, r9, lsr #12
	mov	r6, r6, lsl #12
	adr	r10, brom_copy_from_fifo_dma_patch

	@ Encode an ARM branch from the BROM FIFO-copy helper to the patch.
	@ Use virtual page 0xff in the hooked 1 MiB section, unless that is
	@ the helper's own page.
	mov	r12, r9, lsr #20
	mov	r12, r12, lsl #20
	mov	r0, r9, lsr #12
	and	r0, r0, #0xff
	mov	r1, #0xff
	cmp	r0, r1
	moveq	r1, #0xfe
	mov	r2, r10, lsl #20
	mov	r2, r2, lsr #20
	orr	r0, r12, r1, lsl #12
	orr	r0, r0, r2
	sub	r0, r0, r9
	sub	r0, r0, #8
	mov	r0, r0, asr #2
	bic	r0, r0, #0xff000000
	orr	r11, r0, #0xea000000

	mov	r0, #0
	mov	r1, r4
	ldr	r2, l1_table_entries
	ldr	r3, l1_section_flags
1:
	orr	r8, r3, r0, lsl #20
	str	r8, [r1], #4
	add	r0, r0, #1
	subs	r2, r2, #1
	bne	1b

	sub	r1, r1, #4
	ldr	r8, l1_last_section
	str	r8, [r1]

	@ Copy the hooked BROM page to SRAM, then overwrite only the helper
	@ entry with the branch. The MMU remap below makes fetches from the
	@ original BROM virtual address land in this shadow page.
	mov	r0, r6
	mov	r1, r7
	mov	r2, #BROM_PAGE_WORDS
2:
	ldr	r3, [r0], #4
	str	r3, [r1], #4
	subs	r2, r2, #1
	bne	2b

	sub	r1, r9, r6
	str	r11, [r7, r1]

	@ Identity-map the hooked 1 MiB section with small pages, except for
	@ the hook page and the patch page.
	mov	r0, #0
	mov	r1, r5
	mov	r2, #L2_TABLE_ENTRIES
	mov	r3, #L2_SMALL_PAGE_FLAGS
	mov	r12, r9, lsr #20
	mov	r12, r12, lsl #20
3:
	orr	r8, r12, r0, lsl #12
	orr	r8, r8, r3
	str	r8, [r1], #4
	add	r0, r0, #1
	subs	r2, r2, #1
	bne	3b

	mov	r6, r6, lsr #12
	and	r6, r6, #0xff
	orr	r7, r7, r3
	str	r7, [r5, r6, lsl #2]

	mov	r1, #0xff
	cmp	r6, r1
	moveq	r1, #0xfe
	mov	r8, r10, lsr #12
	mov	r8, r8, lsl #12
	orr	r8, r8, r3
	str	r8, [r5, r1, lsl #2]

	ldr	r3, l1_coarse_descriptor_flags
	orr	r0, r5, r3
	mov	r1, r9, lsr #20
	str	r0, [r4, r1, lsl #2]

	@ Install the new tables, invalidate stale address-translation state,
	@ then enable the MMU, I-cache and branch prediction. The data cache is
	@ intentionally left as the BROM had it.
	ldr	r0, dacr_value
	mcr	p15, 0, r0, c3, c0, 0

	mov	r0, #0
	mcr	p15, 0, r0, c2, c0, 2

	mcr	p15, 0, r4, c2, c0, 0

	mov	r0, #0
	mcr	p15, 0, r0, c8, c7, 0
	mcr	p15, 0, r0, c7, c5, 0
	mcr	p15, 0, r0, c7, c5, 6
	.inst	0xf57ff04f
	.inst	0xf57ff06f

	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #1
	orr	r0, r0, #0x1800
	mcr	p15, 0, r0, c1, c0, 0
	.inst	0xf57ff06f
	bx	lr

dacr_value:
	.word	0x55555555
l1_table_entries:
	.word	L1_TABLE_ENTRIES
l1_section_flags:
	.word	L1_SECTION_FLAGS
l1_last_section:
	.word	(L1_TABLE_ENTRIES - 1) << 20 | L1_SECTION_FLAGS | L1_SECTION_NORMAL
l1_coarse_descriptor_flags:
	.word	L1_COARSE_DESCRIPTOR_FLAGS

	.balign	32
wfi_vector_table:
	b	.
	b	.
	b	.
	b	.
	b	.
	b	.
	b	wfi_irq_handler
	b	.

wfi_irq_handler:
	stmdb	sp!, {r0-r3, r12}

	@ Ignore unrelated interrupts by returning to WFI. The main path sets
	@ wfi_wait_mode before waiting for DMA completion.
	ldr	r3, usb_base
	mov	r12, #USB_INTR_SOF
	strb	r12, [r3, #USB_INTRUSB]
	ldr	r0, wfi_wait_mode
	cmp	r0, #WFI_WAIT_DMA
	beq	wfi_irq_check_dma
	b	wfi_irq_retry

wfi_irq_check_dma:
	ldr	r12, [r3, #USB_DMA_INTS]
	tst	r12, #1
	bne	1f
	ldr	r12, [r3, #USB_DMA_RESIDUAL_BC0]
	cmp	r12, #0
	bne	wfi_irq_retry
1:
	ldr	r12, [r3, #USB_DMA_INTE]
	bic	r12, r12, #1
	str	r12, [r3, #USB_DMA_INTE]
	b	wfi_irq_accept

wfi_irq_retry:
	adr	lr, copy_dma_wfi
	b	wfi_irq_eoi

wfi_irq_accept:
	ldr	lr, wfi_return_addr

wfi_irq_eoi:
	ldr	r0, irq_type
	cmp	r0, #FEL_RX_DMA_IRQ_GICV3
	beq	wfi_irq_handler_gicv3
	ldr	r1, gicc_base
	ldr	r2, [r1, #GICC_IAR]
	cmp	r2, #GIC_SPURIOUS_INTID
	bhs	wfi_irq_handler_done
	str	r2, [r1, #GICC_EOIR]
	b	wfi_irq_handler_done
wfi_irq_handler_gicv3:
	mrc	p15, 0, r2, c12, c12, 0
	cmp	r2, #GIC_SPURIOUS_INTID
	bhs	wfi_irq_handler_done
	mov	r0, r2
	bl	gicv3_eoi_dir_clear
wfi_irq_handler_done:
	add	lr, lr, #4
	ldmia	sp!, {r0-r3, r12}
	subs	pc, lr, #4

gicv3_eoi_dir_clear:
	mcr	p15, 0, r0, c12, c12, 1
	mcr	p15, 0, r0, c12, c11, 1
	ldr	r1, gicd_base
	cmp	r1, #0
	bxeq	lr
	mov	r2, r0, lsr #5
	add	r1, r1, r2, lsl #2
	and	r2, r0, #0x1f
	mov	r3, #1
	mov	r3, r3, lsl r2
	str	r3, [r1, #GICD_ICPENDR]
	bx	lr

	.balign	4
brom_copy_from_fifo_dma_patch:
	@ Hooked BROM helper arguments:
	@   r0 = USB FIFO register address
	@   r1 = destination buffer
	@   r2 = BROM FIFO state
	@   r3 = requested byte count
	stmdb	sp!, {r4-r11, lr}
	mov	r4, r0
	mov	r5, r2

	@ Keep PIO limited to the ROM-requested byte count. Use DMA only when
	@ the remaining AWUSB transfer is 512-byte aligned; otherwise leave the
	@ command on the original byte-PIO path.
	ldr	r6, [r5, #0x04]
	ldr	r7, [r5, #0x10]
	sub	r6, r6, r7
	cmp	r6, r3
	movcc	r3, r6
	mov	r0, r6
	mov	r0, r0, lsr #9
	mov	r0, r0, lsl #9
	sub	r10, r6, r0
	ldr	r7, dma_max_len
	cmp	r0, r7
	movhi	r0, r7
	movhi	r10, #0
	cmp	r10, #0
	bne	copy_pio
	cmp	r3, #512
	bne	copy_pio
	orr	r6, r1, r0
	tst	r6, #3
	bne	copy_pio

	@ Use normal byte PIO unless the BROM asks for one aligned high-speed
	@ packet. Then coalesce full packets with MUSB/USBC RX endpoint DMA.
	ldr	r6, usb_base
	ldrb	r8, [r6, #USB_INDEX]
	ldrb	r9, [r6, #USB_VEND0]
	ldr	r11, [r6, #USB_DMA_INTE]

	@ Temporarily mask RX endpoint interrupts. SOF gives the temporary
	@ IRQ handler repeated interrupt-driven chances to observe DMA
	@ completion, since inner-DMA completion is not routed on all SoCs.
	ldrb	r7, [r6, #USB_INTRUSBE]
	ldrh	r8, [r6, #USB_INTRRXE]
	stmdb	sp!, {r7, r8}

	@ Enable DMA completion as an IRQ source too. The temporary handler
	@ masks it before returning, and the normal cleanup restores DMA_INTE.
	orr	r7, r11, #1
	str	r7, [r6, #USB_DMA_INTE]
	mov	r7, #1
	str	r7, [r6, #USB_DMA_INTS]
	mov	r7, #0
	strh	r7, [r6, #USB_INTRRXE]
	mov	r7, #USB_INTR_SOF
	strb	r7, [r6, #USB_INTRUSBE]

	@ Route the active RX endpoint's DRQ to the DMA bus.
	ldrb	r8, [r6, #USB_INDEX]
	mov	r7, r8, lsl #1
	sub	r7, r7, #1
	mov	r7, r7, lsl #1
	orr	r7, r7, #1
	strb	r7, [r6, #USB_VEND0]

	@ Match the BSP's RX endpoint DMA programming sequence.
	ldrb	r7, [r6, #USB_RXCSR_HI]
	orr	r7, r7, #USB_RXCSR_DMA_REQ_MODE_HI
	strb	r7, [r6, #USB_RXCSR_HI]
	orr	r7, r7, #USB_RXCSR_AUTO_CLEAR_HI
	orr	r7, r7, #USB_RXCSR_DMA_REQ_EN_HI
	strb	r7, [r6, #USB_RXCSR_HI]
	bic	r7, r7, #USB_RXCSR_DMA_REQ_MODE_HI
	strb	r7, [r6, #USB_RXCSR_HI]
	orr	r7, r7, #USB_RXCSR_DMA_REQ_MODE_HI
	strb	r7, [r6, #USB_RXCSR_HI]

	mov	r7, #1
	str	r7, [r6, #USB_DMA_INTS]
	str	r1, [r6, #USB_DMA_SDRAM_ADD0]
	str	r0, [r6, #USB_DMA_BC0]
	ldr	r7, usb_dma_chan_cfg_rx_512
	orr	r7, r7, r8
	str	r7, [r6, #USB_DMA_CHAN_CFN0]

	@ The BROM entered this helper from the USB IRQ handler. Drop that
	@ active state before WFI so the inner-DMA interrupt can wake the
	@ temporary vector, then restore the original vector/state afterwards.
	mrs	r2, cpsr
	mrs	r3, spsr
	mrc	p15, 0, r12, c12, c0, 0
	adr	r7, saved_cpsr
	str	r2, [r7]
	adr	r7, saved_spsr
	str	r3, [r7]
	adr	r7, saved_vbar
	str	r12, [r7]
	adr	r7, wfi_return_addr
	adr	r8, copy_dma_after_dma_irq
	str	r8, [r7]
	adr	r7, wfi_wait_mode
	mov	r8, #WFI_WAIT_DMA
	str	r8, [r7]
	adr	r8, wfi_vector_table
	mcr	p15, 0, r8, c12, c0, 0
	.inst	0xf57ff06f
	ldr	r8, gic_irq_id
	ldr	r7, irq_type
	cmp	r7, #FEL_RX_DMA_IRQ_GICV3
	beq	copy_dma_drop_gicv3_irq
	ldr	lr, gicc_base
	str	r8, [lr, #GICC_EOIR]
	b	copy_dma_dropped_irq
copy_dma_drop_gicv3_irq:
	stmdb	sp!, {r0-r3}
	mov	r0, r8
	bl	gicv3_eoi_dir_clear
	ldmia	sp!, {r0-r3}
copy_dma_dropped_irq:
	.inst	0xf57ff04f
	bic	r8, r2, #0x1f
	orr	r8, r8, #0x13
	bic	r8, r8, #0x80
	msr	cpsr_c, r8
	.inst	0xf57ff06f
copy_dma_wfi:
	.inst	0xe320f003

copy_dma_after_dma_irq:
	ldr	r7, [r6, #USB_DMA_CHAN_CFN0]
	bic	r7, r7, #USB_DMA_CHAN_ENABLE
	str	r7, [r6, #USB_DMA_CHAN_CFN0]
	mov	r7, #1
	str	r7, [r6, #USB_DMA_INTS]
	ldrb	r7, [r6, #USB_RXCSR_HI]
	bic	r7, r7, #USB_RXCSR_DMA_BITS_HI
	strb	r7, [r6, #USB_RXCSR_HI]
	strb	r9, [r6, #USB_VEND0]
	mov	r7, #0
	strb	r7, [r6, #USB_INTRUSBE]
	mov	r7, #USB_INTR_SOF
	strb	r7, [r6, #USB_INTRUSB]

	ldr	r7, [r5, #0x10]
	add	r7, r7, r0
	str	r7, [r5, #0x10]

copy_dma_restore_irq_state:
	adr	r7, saved_cpsr
	ldr	r2, [r7]
	adr	r7, saved_spsr
	ldr	r3, [r7]
	adr	r7, saved_vbar
	ldr	r12, [r7]
	msr	cpsr_c, r2
	mcr	p15, 0, r12, c12, c0, 0
	.inst	0xf57ff06f
	msr	spsr_cxsf, r3
	ldmia	sp!, {r7, r8}
	strh	r8, [r6, #USB_INTRRXE]
	strb	r7, [r6, #USB_INTRUSBE]
	str	r11, [r6, #USB_DMA_INTE]
	b	copy_done

copy_pio:
	mov	r0, r3
	ldr	r7, [r5, #0x10]
	add	r7, r7, r3
	str	r7, [r5, #0x10]
copy_loop:
	subs	r3, r3, #1
	bmi	copy_done
	ldrb	r7, [r4]
	strb	r7, [r1], #1
	b	copy_loop

copy_done:
	ldmia	sp!, {r4-r11, pc}

usb_dma_chan_cfg_rx_512:
	.word	USB_DMA_CHAN_CFG_RX_512
wfi_return_addr:
	.word	0
wfi_wait_mode:
	.word	0
saved_cpsr:
	.word	0
saved_spsr:
	.word	0
saved_vbar:
	.word	0

	.balign	4
fel_rx_dma_params:
l1_tt_addr:
	.word	0
l2_tt_addr:
	.word	0
brom_hook_addr:
	.word	0
brom_hook_page_shadow:
	.word	0
usb_base:
	.word	0
dma_max_len:
	.word	0
irq_type:
	.word	0
gicc_base:
	.word	0
gicd_base:
	.word	0
gic_irq_id:
	.word	0
