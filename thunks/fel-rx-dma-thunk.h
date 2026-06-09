	/* <fel_rx_dma_thunk>: */
	0x00000008, /*        0:    .word      0x00000008                   */
	0x0000033c, /*        4:    .word      0x0000033c                   */
	/* <install_mmu_remap>: */
	0xe28f0fcb, /*        8:    add        r0, pc, #812                 */
	0xe5904000, /*        c:    ldr        r4, [r0]                     */
	0xe5905004, /*       10:    ldr        r5, [r0, #4]                 */
	0xe5909008, /*       14:    ldr        r9, [r0, #8]                 */
	0xe590700c, /*       18:    ldr        r7, [r0, #12]                */
	0xe1a06629, /*       1c:    lsr        r6, r9, #12                  */
	0xe1a06606, /*       20:    lsl        r6, r6, #12                  */
	0xe28faf55, /*       24:    add        sl, pc, #340                 */
	0xe1a0ca29, /*       28:    lsr        ip, r9, #20                  */
	0xe1a0ca0c, /*       2c:    lsl        ip, ip, #20                  */
	0xe1a00629, /*       30:    lsr        r0, r9, #12                  */
	0xe20000ff, /*       34:    and        r0, r0, #255                 */
	0xe3a010ff, /*       38:    mov        r1, #255                     */
	0xe1500001, /*       3c:    cmp        r0, r1                       */
	0x03a010fe, /*       40:    moveq      r1, #254                     */
	0xe1a02a0a, /*       44:    lsl        r2, sl, #20                  */
	0xe1a02a22, /*       48:    lsr        r2, r2, #20                  */
	0xe18c0601, /*       4c:    orr        r0, ip, r1, lsl #12          */
	0xe1800002, /*       50:    orr        r0, r0, r2                   */
	0xe0400009, /*       54:    sub        r0, r0, r9                   */
	0xe2400008, /*       58:    sub        r0, r0, #8                   */
	0xe1a00140, /*       5c:    asr        r0, r0, #2                   */
	0xe3c004ff, /*       60:    bic        r0, r0, #-16777216           */
	0xe380b4ea, /*       64:    orr        fp, r0, #-369098752          */
	0xe3a00000, /*       68:    mov        r0, #0                       */
	0xe1a01004, /*       6c:    mov        r1, r4                       */
	0xe59f20f8, /*       70:    ldr        r2, [pc, #248]               */
	0xe59f30f8, /*       74:    ldr        r3, [pc, #248]               */
	0xe1838a00, /*       78:    orr        r8, r3, r0, lsl #20          */
	0xe4818004, /*       7c:    str        r8, [r1], #4                 */
	0xe2800001, /*       80:    add        r0, r0, #1                   */
	0xe2522001, /*       84:    subs       r2, r2, #1                   */
	0x1afffffa, /*       88:    bne        78 <L2_SMALL_PAGE_FLAGS+0x6> */
	0xe2411004, /*       8c:    sub        r1, r1, #4                   */
	0xe59f80e0, /*       90:    ldr        r8, [pc, #224]               */
	0xe5818000, /*       94:    str        r8, [r1]                     */
	0xe1a00006, /*       98:    mov        r0, r6                       */
	0xe1a01007, /*       9c:    mov        r1, r7                       */
	0xe3a02b01, /*       a0:    mov        r2, #1024                    */
	0xe4903004, /*       a4:    ldr        r3, [r0], #4                 */
	0xe4813004, /*       a8:    str        r3, [r1], #4                 */
	0xe2522001, /*       ac:    subs       r2, r2, #1                   */
	0x1afffffb, /*       b0:    bne        a4 <USB_RXCSR_HI+0x1d>       */
	0xe0491006, /*       b4:    sub        r1, r9, r6                   */
	0xe787b001, /*       b8:    str        fp, [r7, r1]                 */
	0xe3a00000, /*       bc:    mov        r0, #0                       */
	0xe1a01005, /*       c0:    mov        r1, r5                       */
	0xe3a02c01, /*       c4:    mov        r2, #256                     */
	0xe3a03072, /*       c8:    mov        r3, #114                     */
	0xe1a0ca29, /*       cc:    lsr        ip, r9, #20                  */
	0xe1a0ca0c, /*       d0:    lsl        ip, ip, #20                  */
	0xe18c8600, /*       d4:    orr        r8, ip, r0, lsl #12          */
	0xe1888003, /*       d8:    orr        r8, r8, r3                   */
	0xe4818004, /*       dc:    str        r8, [r1], #4                 */
	0xe2800001, /*       e0:    add        r0, r0, #1                   */
	0xe2522001, /*       e4:    subs       r2, r2, #1                   */
	0x1afffff9, /*       e8:    bne        d4 <USB_RXCSR_DMA_BITS_HI+0x2c> */
	0xe1a06626, /*       ec:    lsr        r6, r6, #12                  */
	0xe20660ff, /*       f0:    and        r6, r6, #255                 */
	0xe1877003, /*       f4:    orr        r7, r7, r3                   */
	0xe7857106, /*       f8:    str        r7, [r5, r6, lsl #2]         */
	0xe3a010ff, /*       fc:    mov        r1, #255                     */
	0xe1560001, /*      100:    cmp        r6, r1                       */
	0x03a010fe, /*      104:    moveq      r1, #254                     */
	0xe1a0862a, /*      108:    lsr        r8, sl, #12                  */
	0xe1a08608, /*      10c:    lsl        r8, r8, #12                  */
	0xe1888003, /*      110:    orr        r8, r8, r3                   */
	0xe7858101, /*      114:    str        r8, [r5, r1, lsl #2]         */
	0xe59f305c, /*      118:    ldr        r3, [pc, #92]                */
	0xe1850003, /*      11c:    orr        r0, r5, r3                   */
	0xe1a01a29, /*      120:    lsr        r1, r9, #20                  */
	0xe7840101, /*      124:    str        r0, [r4, r1, lsl #2]         */
	0xe59f003c, /*      128:    ldr        r0, [pc, #60]                */
	0xee030f10, /*      12c:    mcr        15, 0, r0, cr3, cr0, {0}     */
	0xe3a00000, /*      130:    mov        r0, #0                       */
	0xee020f50, /*      134:    mcr        15, 0, r0, cr2, cr0, {2}     */
	0xee024f10, /*      138:    mcr        15, 0, r4, cr2, cr0, {0}     */
	0xe3a00000, /*      13c:    mov        r0, #0                       */
	0xee080f17, /*      140:    mcr        15, 0, r0, cr8, cr7, {0}     */
	0xee070f15, /*      144:    mcr        15, 0, r0, cr7, cr5, {0}     */
	0xee070fd5, /*      148:    mcr        15, 0, r0, cr7, cr5, {6}     */
	0xf57ff04f, /*      14c:    dsb        sy                           */
	0xf57ff06f, /*      150:    isb        sy                           */
	0xee110f10, /*      154:    mrc        15, 0, r0, cr1, cr0, {0}     */
	0xe3800001, /*      158:    orr        r0, r0, #1                   */
	0xe3800b06, /*      15c:    orr        r0, r0, #6144                */
	0xee010f10, /*      160:    mcr        15, 0, r0, cr1, cr0, {0}     */
	0xf57ff06f, /*      164:    isb        sy                           */
	0xe12fff1e, /*      168:    bx         lr                           */
	/* <dacr_value>: */
	0x55555555, /*      16c:    .word      0x55555555                   */
	/* <l1_table_entries>: */
	0x00001000, /*      170:    .word      0x00001000                   */
	/* <l1_section_flags>: */
	0x00000de2, /*      174:    .word      0x00000de2                   */
	/* <l1_last_section>: */
	0xfff01de2, /*      178:    .word      0xfff01de2                   */
	/* <l1_coarse_descriptor_flags>: */
	0x000001e1, /*      17c:    .word      0x000001e1                   */
	/* <brom_copy_from_fifo_dma_patch>: */
	0xe92d47f0, /*      180:    push       {r4, r5, r6, r7, r8, r9, sl, lr} */
	0xe1a04000, /*      184:    mov        r4, r0                       */
	0xe1a05002, /*      188:    mov        r5, r2                       */
	0xe5956004, /*      18c:    ldr        r6, [r5, #4]                 */
	0xe5957010, /*      190:    ldr        r7, [r5, #16]                */
	0xe0466007, /*      194:    sub        r6, r6, r7                   */
	0xe1560003, /*      198:    cmp        r6, r3                       */
	0x31a03006, /*      19c:    movcc      r3, r6                       */
	0xe1a00006, /*      1a0:    mov        r0, r6                       */
	0xe1a004a0, /*      1a4:    lsr        r0, r0, #9                   */
	0xe1a00480, /*      1a8:    lsl        r0, r0, #9                   */
	0xe046a000, /*      1ac:    sub        sl, r6, r0                   */
	0xe59f7198, /*      1b0:    ldr        r7, [pc, #408]               */
	0xe1500007, /*      1b4:    cmp        r0, r7                       */
	0x81a00007, /*      1b8:    movhi      r0, r7                       */
	0x83a0a000, /*      1bc:    movhi      sl, #0                       */
	0xe3530c02, /*      1c0:    cmp        r3, #512                     */
	0x1a000050, /*      1c4:    bne        30c <copy_pio>               */
	0xe1816000, /*      1c8:    orr        r6, r1, r0                   */
	0xe3160003, /*      1cc:    tst        r6, #3                       */
	0x1a00004d, /*      1d0:    bne        30c <copy_pio>               */
	0xe59f6170, /*      1d4:    ldr        r6, [pc, #368]               */
	0xe5d68042, /*      1d8:    ldrb       r8, [r6, #66]                */
	0xe5d69043, /*      1dc:    ldrb       r9, [r6, #67]                */
	0xe1a07088, /*      1e0:    lsl        r7, r8, #1                   */
	0xe2477001, /*      1e4:    sub        r7, r7, #1                   */
	0xe1a07087, /*      1e8:    lsl        r7, r7, #1                   */
	0xe3877001, /*      1ec:    orr        r7, r7, #1                   */
	0xe5c67043, /*      1f0:    strb       r7, [r6, #67]                */
	0xe5d67087, /*      1f4:    ldrb       r7, [r6, #135]               */
	0xe3877008, /*      1f8:    orr        r7, r7, #8                   */
	0xe5c67087, /*      1fc:    strb       r7, [r6, #135]               */
	0xe3877080, /*      200:    orr        r7, r7, #128                 */
	0xe3877020, /*      204:    orr        r7, r7, #32                  */
	0xe5c67087, /*      208:    strb       r7, [r6, #135]               */
	0xe3c77008, /*      20c:    bic        r7, r7, #8                   */
	0xe5c67087, /*      210:    strb       r7, [r6, #135]               */
	0xe3877008, /*      214:    orr        r7, r7, #8                   */
	0xe5c67087, /*      218:    strb       r7, [r6, #135]               */
	0xe3a07001, /*      21c:    mov        r7, #1                       */
	0xe5867504, /*      220:    str        r7, [r6, #1284]              */
	0xe5867500, /*      224:    str        r7, [r6, #1280]              */
	0xe5861544, /*      228:    str        r1, [r6, #1348]              */
	0xe5860548, /*      22c:    str        r0, [r6, #1352]              */
	0xe59f70fc, /*      230:    ldr        r7, [pc, #252]               */
	0xe1877008, /*      234:    orr        r7, r7, r8                   */
	0xe5867540, /*      238:    str        r7, [r6, #1344]              */
	0xe59f70f4, /*      23c:    ldr        r7, [pc, #244]               */
	/* <copy_dma_wait>: */
	0xe5968504, /*      240:    ldr        r8, [r6, #1284]              */
	0xe3180001, /*      244:    tst        r8, #1                       */
	0x1a00000e, /*      248:    bne        288 <copy_dma_done>          */
	0xe596854c, /*      24c:    ldr        r8, [r6, #1356]              */
	0xe3580000, /*      250:    cmp        r8, #0                       */
	0x0a00000b, /*      254:    beq        288 <copy_dma_done>          */
	0xe2577001, /*      258:    subs       r7, r7, #1                   */
	0x1afffff7, /*      25c:    bne        240 <copy_dma_wait>          */
	0xe5967540, /*      260:    ldr        r7, [r6, #1344]              */
	0xe3c77102, /*      264:    bic        r7, r7, #-2147483648         */
	0xe5867540, /*      268:    str        r7, [r6, #1344]              */
	0xe5d67087, /*      26c:    ldrb       r7, [r6, #135]               */
	0xe3c770a8, /*      270:    bic        r7, r7, #168                 */
	0xe5c67087, /*      274:    strb       r7, [r6, #135]               */
	0xe5c69043, /*      278:    strb       r9, [r6, #67]                */
	0xe1580000, /*      27c:    cmp        r8, r0                       */
	0x1a00001f, /*      280:    bne        304 <copy_dma_failed>        */
	0xea000020, /*      284:    b          30c <copy_pio>               */
	/* <copy_dma_done>: */
	0xe5967540, /*      288:    ldr        r7, [r6, #1344]              */
	0xe3c77102, /*      28c:    bic        r7, r7, #-2147483648         */
	0xe5867540, /*      290:    str        r7, [r6, #1344]              */
	0xe3a07001, /*      294:    mov        r7, #1                       */
	0xe5867504, /*      298:    str        r7, [r6, #1284]              */
	0xe5d67087, /*      29c:    ldrb       r7, [r6, #135]               */
	0xe3c770a8, /*      2a0:    bic        r7, r7, #168                 */
	0xe5c67087, /*      2a4:    strb       r7, [r6, #135]               */
	0xe5c69043, /*      2a8:    strb       r9, [r6, #67]                */
	0xe5957010, /*      2ac:    ldr        r7, [r5, #16]                */
	0xe0877000, /*      2b0:    add        r7, r7, r0                   */
	0xe5857010, /*      2b4:    str        r7, [r5, #16]                */
	0xe35a0000, /*      2b8:    cmp        sl, #0                       */
	0x0a00001b, /*      2bc:    beq        330 <copy_done>              */
	0xe0811000, /*      2c0:    add        r1, r1, r0                   */
	0xe59f706c, /*      2c4:    ldr        r7, [pc, #108]               */
	/* <copy_tail_wait>: */
	0xe5d68086, /*      2c8:    ldrb       r8, [r6, #134]               */
	0xe3180001, /*      2cc:    tst        r8, #1                       */
	0x1a000002, /*      2d0:    bne        2e0 <copy_tail>              */
	0xe2577001, /*      2d4:    subs       r7, r7, #1                   */
	0x1afffffa, /*      2d8:    bne        2c8 <copy_tail_wait>         */
	0xea000013, /*      2dc:    b          330 <copy_done>              */
	/* <copy_tail>: */
	0xe5957010, /*      2e0:    ldr        r7, [r5, #16]                */
	0xe087700a, /*      2e4:    add        r7, r7, sl                   */
	0xe5857010, /*      2e8:    str        r7, [r5, #16]                */
	0xe1a0300a, /*      2ec:    mov        r3, sl                       */
	/* <copy_tail_loop>: */
	0xe2533001, /*      2f0:    subs       r3, r3, #1                   */
	0x4a00000d, /*      2f4:    bmi        330 <copy_done>              */
	0xe5d47000, /*      2f8:    ldrb       r7, [r4]                     */
	0xe4c17001, /*      2fc:    strb       r7, [r1], #1                 */
	0xeafffffa, /*      300:    b          2f0 <copy_tail_loop>         */
	/* <copy_dma_failed>: */
	0xe3a00000, /*      304:    mov        r0, #0                       */
	0xea000008, /*      308:    b          330 <copy_done>              */
	/* <copy_pio>: */
	0xe1a00003, /*      30c:    mov        r0, r3                       */
	0xe5957010, /*      310:    ldr        r7, [r5, #16]                */
	0xe0877003, /*      314:    add        r7, r7, r3                   */
	0xe5857010, /*      318:    str        r7, [r5, #16]                */
	/* <copy_loop>: */
	0xe2533001, /*      31c:    subs       r3, r3, #1                   */
	0x4a000002, /*      320:    bmi        330 <copy_done>              */
	0xe5d47000, /*      324:    ldrb       r7, [r4]                     */
	0xe4c17001, /*      328:    strb       r7, [r1], #1                 */
	0xeafffffa, /*      32c:    b          31c <copy_loop>              */
	/* <copy_done>: */
	0xe8bd87f0, /*      330:    pop        {r4, r5, r6, r7, r8, r9, sl, pc} */
	/* <usb_dma_chan_cfg_rx_512>: */
	0x82000010, /*      334:    .word      0x82000010                   */
	/* <dma_timeout>: */
	0x10000000, /*      338:    .word      0x10000000                   */
	/* <fel_rx_dma_params>: */
	0x00000000, /*      33c:    .word      0x00000000                   */
	/* <l2_tt_addr>: */
	0x00000000, /*      340:    .word      0x00000000                   */
	/* <brom_hook_addr>: */
	0x00000000, /*      344:    .word      0x00000000                   */
	/* <brom_hook_page_shadow>: */
	0x00000000, /*      348:    .word      0x00000000                   */
	/* <usb_base>: */
	0x00000000, /*      34c:    .word      0x00000000                   */
	/* <dma_max_len>: */
	0x00000000, /*      350:    .word      0x00000000                   */
