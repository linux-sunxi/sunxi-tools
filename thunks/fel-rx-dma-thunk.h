	/* <fel_rx_dma_thunk>: */
	0x0000000c, /*        0:    .word      0x0000000c                   */
	0x000004c8, /*        4:    .word      0x000004c8                   */
	/* <fel_rx_dma_params_ref>: */
	0x000004c0, /*        8:    .word      0x000004c0                   */
	/* <install_mmu_remap>: */
	0xe24f000c, /*        c:    sub        r0, pc, #12                  */
	0xe5901000, /*       10:    ldr        r1, [r0]                     */
	0xe0800001, /*       14:    add        r0, r0, r1                   */
	0xe5904000, /*       18:    ldr        r4, [r0]                     */
	0xe5905004, /*       1c:    ldr        r5, [r0, #4]                 */
	0xe5909008, /*       20:    ldr        r9, [r0, #8]                 */
	0xe590700c, /*       24:    ldr        r7, [r0, #12]                */
	0xe1a06629, /*       28:    lsr        r6, r9, #12                  */
	0xe1a06606, /*       2c:    lsl        r6, r6, #12                  */
	0xe28fae25, /*       30:    add        sl, pc, #592                 */
	0xe1a0ca29, /*       34:    lsr        ip, r9, #20                  */
	0xe1a0ca0c, /*       38:    lsl        ip, ip, #20                  */
	0xe1a00629, /*       3c:    lsr        r0, r9, #12                  */
	0xe20000ff, /*       40:    and        r0, r0, #255                 */
	0xe3a010ff, /*       44:    mov        r1, #255                     */
	0xe1500001, /*       48:    cmp        r0, r1                       */
	0x03a010fe, /*       4c:    moveq      r1, #254                     */
	0xe1a02a0a, /*       50:    lsl        r2, sl, #20                  */
	0xe1a02a22, /*       54:    lsr        r2, r2, #20                  */
	0xe18c0601, /*       58:    orr        r0, ip, r1, lsl #12          */
	0xe1800002, /*       5c:    orr        r0, r0, r2                   */
	0xe0400009, /*       60:    sub        r0, r0, r9                   */
	0xe2400008, /*       64:    sub        r0, r0, #8                   */
	0xe1a00140, /*       68:    asr        r0, r0, #2                   */
	0xe3c004ff, /*       6c:    bic        r0, r0, #-16777216           */
	0xe380b4ea, /*       70:    orr        fp, r0, #-369098752          */
	0xe3a00000, /*       74:    mov        r0, #0                       */
	0xe1a01004, /*       78:    mov        r1, r4                       */
	0xe59f20f8, /*       7c:    ldr        r2, [pc, #248]               */
	0xe59f30f8, /*       80:    ldr        r3, [pc, #248]               */
	0xe1838a00, /*       84:    orr        r8, r3, r0, lsl #20          */
	0xe4818004, /*       88:    str        r8, [r1], #4                 */
	0xe2800001, /*       8c:    add        r0, r0, #1                   */
	0xe2522001, /*       90:    subs       r2, r2, #1                   */
	0x1afffffa, /*       94:    bne        84 <USB_RXCSR_AUTO_CLEAR_HI+0x4> */
	0xe2411004, /*       98:    sub        r1, r1, #4                   */
	0xe59f80e0, /*       9c:    ldr        r8, [pc, #224]               */
	0xe5818000, /*       a0:    str        r8, [r1]                     */
	0xe1a00006, /*       a4:    mov        r0, r6                       */
	0xe1a01007, /*       a8:    mov        r1, r7                       */
	0xe3a02b01, /*       ac:    mov        r2, #1024                    */
	0xe4903004, /*       b0:    ldr        r3, [r0], #4                 */
	0xe4813004, /*       b4:    str        r3, [r1], #4                 */
	0xe2522001, /*       b8:    subs       r2, r2, #1                   */
	0x1afffffb, /*       bc:    bne        b0 <USB_RXCSR_DMA_BITS_HI+0x8> */
	0xe0491006, /*       c0:    sub        r1, r9, r6                   */
	0xe787b001, /*       c4:    str        fp, [r7, r1]                 */
	0xe3a00000, /*       c8:    mov        r0, #0                       */
	0xe1a01005, /*       cc:    mov        r1, r5                       */
	0xe3a02c01, /*       d0:    mov        r2, #256                     */
	0xe3a03072, /*       d4:    mov        r3, #114                     */
	0xe1a0ca29, /*       d8:    lsr        ip, r9, #20                  */
	0xe1a0ca0c, /*       dc:    lsl        ip, ip, #20                  */
	0xe18c8600, /*       e0:    orr        r8, ip, r0, lsl #12          */
	0xe1888003, /*       e4:    orr        r8, r8, r3                   */
	0xe4818004, /*       e8:    str        r8, [r1], #4                 */
	0xe2800001, /*       ec:    add        r0, r0, #1                   */
	0xe2522001, /*       f0:    subs       r2, r2, #1                   */
	0x1afffff9, /*       f4:    bne        e0 <USB_RXCSR_DMA_BITS_HI+0x38> */
	0xe1a06626, /*       f8:    lsr        r6, r6, #12                  */
	0xe20660ff, /*       fc:    and        r6, r6, #255                 */
	0xe1877003, /*      100:    orr        r7, r7, r3                   */
	0xe7857106, /*      104:    str        r7, [r5, r6, lsl #2]         */
	0xe3a010ff, /*      108:    mov        r1, #255                     */
	0xe1560001, /*      10c:    cmp        r6, r1                       */
	0x03a010fe, /*      110:    moveq      r1, #254                     */
	0xe1a0862a, /*      114:    lsr        r8, sl, #12                  */
	0xe1a08608, /*      118:    lsl        r8, r8, #12                  */
	0xe1888003, /*      11c:    orr        r8, r8, r3                   */
	0xe7858101, /*      120:    str        r8, [r5, r1, lsl #2]         */
	0xe59f305c, /*      124:    ldr        r3, [pc, #92]                */
	0xe1850003, /*      128:    orr        r0, r5, r3                   */
	0xe1a01a29, /*      12c:    lsr        r1, r9, #20                  */
	0xe7840101, /*      130:    str        r0, [r4, r1, lsl #2]         */
	0xe59f003c, /*      134:    ldr        r0, [pc, #60]                */
	0xee030f10, /*      138:    mcr        15, 0, r0, cr3, cr0, {0}     */
	0xe3a00000, /*      13c:    mov        r0, #0                       */
	0xee020f50, /*      140:    mcr        15, 0, r0, cr2, cr0, {2}     */
	0xee024f10, /*      144:    mcr        15, 0, r4, cr2, cr0, {0}     */
	0xe3a00000, /*      148:    mov        r0, #0                       */
	0xee080f17, /*      14c:    mcr        15, 0, r0, cr8, cr7, {0}     */
	0xee070f15, /*      150:    mcr        15, 0, r0, cr7, cr5, {0}     */
	0xee070fd5, /*      154:    mcr        15, 0, r0, cr7, cr5, {6}     */
	0xf57ff04f, /*      158:    dsb        sy                           */
	0xf57ff06f, /*      15c:    isb        sy                           */
	0xee110f10, /*      160:    mrc        15, 0, r0, cr1, cr0, {0}     */
	0xe3800001, /*      164:    orr        r0, r0, #1                   */
	0xe3800b06, /*      168:    orr        r0, r0, #6144                */
	0xee010f10, /*      16c:    mcr        15, 0, r0, cr1, cr0, {0}     */
	0xf57ff06f, /*      170:    isb        sy                           */
	0xe12fff1e, /*      174:    bx         lr                           */
	/* <dacr_value>: */
	0x55555555, /*      178:    .word      0x55555555                   */
	/* <l1_table_entries>: */
	0x00001000, /*      17c:    .word      0x00001000                   */
	/* <l1_section_flags>: */
	0x00000de2, /*      180:    .word      0x00000de2                   */
	/* <l1_last_section>: */
	0xfff01de2, /*      184:    .word      0xfff01de2                   */
	/* <l1_coarse_descriptor_flags>: */
	0x000001e1, /*      188:    .word      0x000001e1                   */
	0xe1a00000, /*      18c:    nop                                     */
	0xe1a00000, /*      190:    nop                                     */
	0xe1a00000, /*      194:    nop                                     */
	0xe1a00000, /*      198:    nop                                     */
	0xe1a00000, /*      19c:    nop                                     */
	/* <wfi_vector_table>: */
	0xeafffffe, /*      1a0:    b          1a0 <wfi_vector_table>       */
	0xeafffffe, /*      1a4:    b          1a4 <wfi_vector_table+0x4>   */
	0xeafffffe, /*      1a8:    b          1a8 <wfi_vector_table+0x8>   */
	0xeafffffe, /*      1ac:    b          1ac <wfi_vector_table+0xc>   */
	0xeafffffe, /*      1b0:    b          1b0 <wfi_vector_table+0x10>  */
	0xeafffffe, /*      1b4:    b          1b4 <wfi_vector_table+0x14>  */
	0xea000000, /*      1b8:    b          1c0 <wfi_irq_handler>        */
	0xeafffffe, /*      1bc:    b          1bc <wfi_vector_table+0x1c>  */
	/* <wfi_irq_handler>: */
	0xe92d100f, /*      1c0:    push       {r0, r1, r2, r3, ip}         */
	0xe59f330c, /*      1c4:    ldr        r3, [pc, #780]               */
	0xe3a0c008, /*      1c8:    mov        ip, #8                       */
	0xe5c3c04c, /*      1cc:    strb       ip, [r3, #76]                */
	0xe59f02e0, /*      1d0:    ldr        r0, [pc, #736]               */
	0xe3500001, /*      1d4:    cmp        r0, #1                       */
	0x0a000000, /*      1d8:    beq        1e0 <wfi_irq_check_dma>      */
	0xea000009, /*      1dc:    b          208 <wfi_irq_retry>          */
	/* <wfi_irq_check_dma>: */
	0xe593c504, /*      1e0:    ldr        ip, [r3, #1284]              */
	0xe31c0001, /*      1e4:    tst        ip, #1                       */
	0x1a000002, /*      1e8:    bne        1f8 <L1_COARSE_DESCRIPTOR_FLAGS+0x17> */
	0xe593c54c, /*      1ec:    ldr        ip, [r3, #1356]              */
	0xe35c0000, /*      1f0:    cmp        ip, #0                       */
	0x1a000003, /*      1f4:    bne        208 <wfi_irq_retry>          */
	0xe593c500, /*      1f8:    ldr        ip, [r3, #1280]              */
	0xe3ccc001, /*      1fc:    bic        ip, ip, #1                   */
	0xe583c500, /*      200:    str        ip, [r3, #1280]              */
	0xea000001, /*      204:    b          210 <wfi_irq_accept>         */
	/* <wfi_irq_retry>: */
	0xe28fef7e, /*      208:    add        lr, pc, #504                 */
	0xea000000, /*      20c:    b          214 <wfi_irq_eoi>            */
	/* <wfi_irq_accept>: */
	0xe59fe29c, /*      210:    ldr        lr, [pc, #668]               */
	/* <wfi_irq_eoi>: */
	0xe59f02c4, /*      214:    ldr        r0, [pc, #708]               */
	0xe3500002, /*      218:    cmp        r0, #2                       */
	0x0a000005, /*      21c:    beq        238 <wfi_irq_handler_gicv3>  */
	0xe59f12bc, /*      220:    ldr        r1, [pc, #700]               */
	0xe591200c, /*      224:    ldr        r2, [r1, #12]                */
	0xe3520fff, /*      228:    cmp        r2, #1020                    */
	0x2a000006, /*      22c:    bcs        24c <wfi_irq_handler_done>   */
	0xe5812010, /*      230:    str        r2, [r1, #16]                */
	0xea000004, /*      234:    b          24c <wfi_irq_handler_done>   */
	/* <wfi_irq_handler_gicv3>: */
	0xee1c2f1c, /*      238:    mrc        15, 0, r2, cr12, cr12, {0}   */
	0xe3520fff, /*      23c:    cmp        r2, #1020                    */
	0x2a000001, /*      240:    bcs        24c <wfi_irq_handler_done>   */
	0xe1a00002, /*      244:    mov        r0, r2                       */
	0xeb000002, /*      248:    bl         258 <gicv3_eoi_dir_clear>    */
	/* <wfi_irq_handler_done>: */
	0xe28ee004, /*      24c:    add        lr, lr, #4                   */
	0xe8bd100f, /*      250:    pop        {r0, r1, r2, r3, ip}         */
	0xe25ef004, /*      254:    subs       pc, lr, #4                   */
	/* <gicv3_eoi_dir_clear>: */
	0xee0c0f3c, /*      258:    mcr        15, 0, r0, cr12, cr12, {1}   */
	0xee0c0f3b, /*      25c:    mcr        15, 0, r0, cr12, cr11, {1}   */
	0xe59f1280, /*      260:    ldr        r1, [pc, #640]               */
	0xe3510000, /*      264:    cmp        r1, #0                       */
	0x012fff1e, /*      268:    bxeq       lr                           */
	0xe1a022a0, /*      26c:    lsr        r2, r0, #5                   */
	0xe0811102, /*      270:    add        r1, r1, r2, lsl #2           */
	0xe200201f, /*      274:    and        r2, r0, #31                  */
	0xe3a03001, /*      278:    mov        r3, #1                       */
	0xe1a03213, /*      27c:    lsl        r3, r3, r2                   */
	0xe5813280, /*      280:    str        r3, [r1, #640]               */
	0xe12fff1e, /*      284:    bx         lr                           */
	/* <brom_copy_from_fifo_dma_patch>: */
	0xe92d4ff0, /*      288:    push       {r4, r5, r6, r7, r8, r9, sl, fp, lr} */
	0xe1a04000, /*      28c:    mov        r4, r0                       */
	0xe1a05002, /*      290:    mov        r5, r2                       */
	0xe5956004, /*      294:    ldr        r6, [r5, #4]                 */
	0xe5957010, /*      298:    ldr        r7, [r5, #16]                */
	0xe0466007, /*      29c:    sub        r6, r6, r7                   */
	0xe1560003, /*      2a0:    cmp        r6, r3                       */
	0x31a03006, /*      2a4:    movcc      r3, r6                       */
	0xe1a00006, /*      2a8:    mov        r0, r6                       */
	0xe1a004a0, /*      2ac:    lsr        r0, r0, #9                   */
	0xe1a00480, /*      2b0:    lsl        r0, r0, #9                   */
	0xe046a000, /*      2b4:    sub        sl, r6, r0                   */
	0xe59f721c, /*      2b8:    ldr        r7, [pc, #540]               */
	0xe1500007, /*      2bc:    cmp        r0, r7                       */
	0x81a00007, /*      2c0:    movhi      r0, r7                       */
	0x83a0a000, /*      2c4:    movhi      sl, #0                       */
	0xe35a0000, /*      2c8:    cmp        sl, #0                       */
	0x1a00006d, /*      2cc:    bne        488 <copy_pio>               */
	0xe3530c02, /*      2d0:    cmp        r3, #512                     */
	0x1a00006b, /*      2d4:    bne        488 <copy_pio>               */
	0xe1816000, /*      2d8:    orr        r6, r1, r0                   */
	0xe3160003, /*      2dc:    tst        r6, #3                       */
	0x1a000068, /*      2e0:    bne        488 <copy_pio>               */
	0xe59f61ec, /*      2e4:    ldr        r6, [pc, #492]               */
	0xe5d68042, /*      2e8:    ldrb       r8, [r6, #66]                */
	0xe5d69043, /*      2ec:    ldrb       r9, [r6, #67]                */
	0xe596b500, /*      2f0:    ldr        fp, [r6, #1280]              */
	0xe5d67050, /*      2f4:    ldrb       r7, [r6, #80]                */
	0xe1d684ba, /*      2f8:    ldrh       r8, [r6, #74]                */
	0xe92d0180, /*      2fc:    push       {r7, r8}                     */
	0xe38b7001, /*      300:    orr        r7, fp, #1                   */
	0xe5867500, /*      304:    str        r7, [r6, #1280]              */
	0xe3a07001, /*      308:    mov        r7, #1                       */
	0xe5867504, /*      30c:    str        r7, [r6, #1284]              */
	0xe3a07000, /*      310:    mov        r7, #0                       */
	0xe1c674ba, /*      314:    strh       r7, [r6, #74]                */
	0xe3a07008, /*      318:    mov        r7, #8                       */
	0xe5c67050, /*      31c:    strb       r7, [r6, #80]                */
	0xe5d68042, /*      320:    ldrb       r8, [r6, #66]                */
	0xe1a07088, /*      324:    lsl        r7, r8, #1                   */
	0xe2477001, /*      328:    sub        r7, r7, #1                   */
	0xe1a07087, /*      32c:    lsl        r7, r7, #1                   */
	0xe3877001, /*      330:    orr        r7, r7, #1                   */
	0xe5c67043, /*      334:    strb       r7, [r6, #67]                */
	0xe5d67087, /*      338:    ldrb       r7, [r6, #135]               */
	0xe3877008, /*      33c:    orr        r7, r7, #8                   */
	0xe5c67087, /*      340:    strb       r7, [r6, #135]               */
	0xe3877080, /*      344:    orr        r7, r7, #128                 */
	0xe3877020, /*      348:    orr        r7, r7, #32                  */
	0xe5c67087, /*      34c:    strb       r7, [r6, #135]               */
	0xe3c77008, /*      350:    bic        r7, r7, #8                   */
	0xe5c67087, /*      354:    strb       r7, [r6, #135]               */
	0xe3877008, /*      358:    orr        r7, r7, #8                   */
	0xe5c67087, /*      35c:    strb       r7, [r6, #135]               */
	0xe3a07001, /*      360:    mov        r7, #1                       */
	0xe5867504, /*      364:    str        r7, [r6, #1284]              */
	0xe5861544, /*      368:    str        r1, [r6, #1348]              */
	0xe5860548, /*      36c:    str        r0, [r6, #1352]              */
	0xe59f7138, /*      370:    ldr        r7, [pc, #312]               */
	0xe1877008, /*      374:    orr        r7, r7, r8                   */
	0xe5867540, /*      378:    str        r7, [r6, #1344]              */
	0xe10f2000, /*      37c:    mrs        r2, CPSR                     */
	0xe14f3000, /*      380:    mrs        r3, SPSR                     */
	0xee1ccf10, /*      384:    mrc        15, 0, ip, cr12, cr0, {0}    */
	0xe28f7f4b, /*      388:    add        r7, pc, #300                 */
	0xe5872000, /*      38c:    str        r2, [r7]                     */
	0xe28f7f4a, /*      390:    add        r7, pc, #296                 */
	0xe5873000, /*      394:    str        r3, [r7]                     */
	0xe28f7f49, /*      398:    add        r7, pc, #292                 */
	0xe587c000, /*      39c:    str        ip, [r7]                     */
	0xe28f7f43, /*      3a0:    add        r7, pc, #268                 */
	0xe28f8060, /*      3a4:    add        r8, pc, #96                  */
	0xe5878000, /*      3a8:    str        r8, [r7]                     */
	0xe28f7f41, /*      3ac:    add        r7, pc, #260                 */
	0xe3a08001, /*      3b0:    mov        r8, #1                       */
	0xe5878000, /*      3b4:    str        r8, [r7]                     */
	0xe24f8e22, /*      3b8:    sub        r8, pc, #544                 */
	0xee0c8f10, /*      3bc:    mcr        15, 0, r8, cr12, cr0, {0}    */
	0xf57ff06f, /*      3c0:    isb        sy                           */
	0xe59f8120, /*      3c4:    ldr        r8, [pc, #288]               */
	0xe59f7110, /*      3c8:    ldr        r7, [pc, #272]               */
	0xe3570002, /*      3cc:    cmp        r7, #2                       */
	0x0a000002, /*      3d0:    beq        3e0 <copy_dma_drop_gicv3_irq> */
	0xe59fe108, /*      3d4:    ldr        lr, [pc, #264]               */
	0xe58e8010, /*      3d8:    str        r8, [lr, #16]                */
	0xea000003, /*      3dc:    b          3f0 <copy_dma_dropped_irq>   */
	/* <copy_dma_drop_gicv3_irq>: */
	0xe92d000f, /*      3e0:    push       {r0, r1, r2, r3}             */
	0xe1a00008, /*      3e4:    mov        r0, r8                       */
	0xebffff9a, /*      3e8:    bl         258 <gicv3_eoi_dir_clear>    */
	0xe8bd000f, /*      3ec:    pop        {r0, r1, r2, r3}             */
	/* <copy_dma_dropped_irq>: */
	0xf57ff04f, /*      3f0:    dsb        sy                           */
	0xe3c2801f, /*      3f4:    bic        r8, r2, #31                  */
	0xe3888013, /*      3f8:    orr        r8, r8, #19                  */
	0xe3c88080, /*      3fc:    bic        r8, r8, #128                 */
	0xe121f008, /*      400:    msr        CPSR_c, r8                   */
	0xf57ff06f, /*      404:    isb        sy                           */
	/* <copy_dma_wfi>: */
	0xe320f003, /*      408:    wfi                                     */
	/* <copy_dma_after_dma_irq>: */
	0xe5967540, /*      40c:    ldr        r7, [r6, #1344]              */
	0xe3c77102, /*      410:    bic        r7, r7, #-2147483648         */
	0xe5867540, /*      414:    str        r7, [r6, #1344]              */
	0xe3a07001, /*      418:    mov        r7, #1                       */
	0xe5867504, /*      41c:    str        r7, [r6, #1284]              */
	0xe5d67087, /*      420:    ldrb       r7, [r6, #135]               */
	0xe3c770a8, /*      424:    bic        r7, r7, #168                 */
	0xe5c67087, /*      428:    strb       r7, [r6, #135]               */
	0xe5c69043, /*      42c:    strb       r9, [r6, #67]                */
	0xe3a07000, /*      430:    mov        r7, #0                       */
	0xe5c67050, /*      434:    strb       r7, [r6, #80]                */
	0xe3a07008, /*      438:    mov        r7, #8                       */
	0xe5c6704c, /*      43c:    strb       r7, [r6, #76]                */
	0xe5957010, /*      440:    ldr        r7, [r5, #16]                */
	0xe0877000, /*      444:    add        r7, r7, r0                   */
	0xe5857010, /*      448:    str        r7, [r5, #16]                */
	/* <copy_dma_restore_irq_state>: */
	0xe28f7068, /*      44c:    add        r7, pc, #104                 */
	0xe5972000, /*      450:    ldr        r2, [r7]                     */
	0xe28f7064, /*      454:    add        r7, pc, #100                 */
	0xe5973000, /*      458:    ldr        r3, [r7]                     */
	0xe28f7060, /*      45c:    add        r7, pc, #96                  */
	0xe597c000, /*      460:    ldr        ip, [r7]                     */
	0xe121f002, /*      464:    msr        CPSR_c, r2                   */
	0xee0ccf10, /*      468:    mcr        15, 0, ip, cr12, cr0, {0}    */
	0xf57ff06f, /*      46c:    isb        sy                           */
	0xe16ff003, /*      470:    msr        SPSR_fsxc, r3                */
	0xe8bd0180, /*      474:    pop        {r7, r8}                     */
	0xe1c684ba, /*      478:    strh       r8, [r6, #74]                */
	0xe5c67050, /*      47c:    strb       r7, [r6, #80]                */
	0xe586b500, /*      480:    str        fp, [r6, #1280]              */
	0xea000008, /*      484:    b          4ac <copy_done>              */
	/* <copy_pio>: */
	0xe1a00003, /*      488:    mov        r0, r3                       */
	0xe5957010, /*      48c:    ldr        r7, [r5, #16]                */
	0xe0877003, /*      490:    add        r7, r7, r3                   */
	0xe5857010, /*      494:    str        r7, [r5, #16]                */
	/* <copy_loop>: */
	0xe2533001, /*      498:    subs       r3, r3, #1                   */
	0x4a000002, /*      49c:    bmi        4ac <copy_done>              */
	0xe5d47000, /*      4a0:    ldrb       r7, [r4]                     */
	0xe4c17001, /*      4a4:    strb       r7, [r1], #1                 */
	0xeafffffa, /*      4a8:    b          498 <copy_loop>              */
	/* <copy_done>: */
	0xe8bd8ff0, /*      4ac:    pop        {r4, r5, r6, r7, r8, r9, sl, fp, pc} */
	/* <usb_dma_chan_cfg_rx_512>: */
	0x82000010, /*      4b0:    .word      0x82000010                   */
	/* <wfi_return_addr>: */
	0x00000000, /*      4b4:    .word      0x00000000                   */
	/* <wfi_wait_mode>: */
	0x00000000, /*      4b8:    .word      0x00000000                   */
	/* <saved_cpsr>: */
	0x00000000, /*      4bc:    .word      0x00000000                   */
	/* <saved_spsr>: */
	0x00000000, /*      4c0:    .word      0x00000000                   */
	/* <saved_vbar>: */
	0x00000000, /*      4c4:    .word      0x00000000                   */
	/* <fel_rx_dma_params>: */
	0x00000000, /*      4c8:    .word      0x00000000                   */
	/* <l2_tt_addr>: */
	0x00000000, /*      4cc:    .word      0x00000000                   */
	/* <brom_hook_addr>: */
	0x00000000, /*      4d0:    .word      0x00000000                   */
	/* <brom_hook_page_shadow>: */
	0x00000000, /*      4d4:    .word      0x00000000                   */
	/* <usb_base>: */
	0x00000000, /*      4d8:    .word      0x00000000                   */
	/* <dma_max_len>: */
	0x00000000, /*      4dc:    .word      0x00000000                   */
	/* <irq_type>: */
	0x00000000, /*      4e0:    .word      0x00000000                   */
	/* <gicc_base>: */
	0x00000000, /*      4e4:    .word      0x00000000                   */
	/* <gicd_base>: */
	0x00000000, /*      4e8:    .word      0x00000000                   */
	/* <gic_irq_id>: */
	0x00000000, /*      4ec:    .word      0x00000000                   */
