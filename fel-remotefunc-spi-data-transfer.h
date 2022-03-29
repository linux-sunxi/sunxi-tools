/* Automatically generated, do not edit! */

static void
aw_fel_remotefunc_prepare_spi_batch_data_transfer(feldev_handle *dev,
                                                  uint32_t              buf,
                                                  uint32_t              spi_ctl_reg,
                                                  uint32_t              spi_ctl_xch_bitmask,
                                                  uint32_t              spi_fifo_reg,
                                                  uint32_t              spi_tx_reg,
                                                  uint32_t              spi_rx_reg,
                                                  uint32_t              spi_bc_reg,
                                                  uint32_t              spi_tc_reg,
                                                  uint32_t              spi_bcc_reg)
{
	static uint8_t arm_code[] = {
		0xf0, 0x4f, 0x2d, 0xe9, /*    0:    push     {r4, r5, r6, r7, r8, r9, sl, fp, lr} */
		0xc8, 0x91, 0x9f, 0xe5, /*    4:    ldr      r9, [pc, #456]                     */
		0x14, 0xd0, 0x4d, 0xe2, /*    8:    sub      sp, sp, #20                        */
		0x00, 0x20, 0x8d, 0xe5, /*    c:    str      r2, [sp]                           */
		0x0c, 0x20, 0x8d, 0xe2, /*   10:    add      r2, sp, #12                        */
		0x04, 0x20, 0x8d, 0xe5, /*   14:    str      r2, [sp, #4]                       */
		0x01, 0x60, 0xd0, 0xe5, /*   18:    ldrb     r6, [r0, #1]                       */
		0x00, 0x20, 0xd0, 0xe5, /*   1c:    ldrb     r2, [r0]                           */
		0x06, 0x24, 0x82, 0xe1, /*   20:    orr      r2, r2, r6, lsl #8                 */
		0x22, 0x64, 0xa0, 0xe1, /*   24:    lsr      r6, r2, #8                         */
		0x02, 0x64, 0x86, 0xe1, /*   28:    orr      r6, r6, r2, lsl #8                 */
		0x06, 0x68, 0xa0, 0xe1, /*   2c:    lsl      r6, r6, #16                        */
		0x26, 0x68, 0xa0, 0xe1, /*   30:    lsr      r6, r6, #16                        */
		0x00, 0x00, 0x56, 0xe3, /*   34:    cmp      r6, #0                             */
		0x63, 0x00, 0x00, 0x0a, /*   38:    beq      1cc <spi_batch_data_transfer+0x1cc> */
		0x09, 0x00, 0x56, 0xe1, /*   3c:    cmp      r6, r9                             */
		0x05, 0x20, 0xa0, 0x03, /*   40:    moveq    r2, #5                             */
		0x0c, 0x20, 0xcd, 0x05, /*   44:    strbeq   r2, [sp, #12]                      */
		0x40, 0x20, 0x9d, 0xe5, /*   48:    ldr      r2, [sp, #64]                      */
		0x06, 0x50, 0xa0, 0x11, /*   4c:    movne    r5, r6                             */
		0x02, 0x50, 0xa0, 0x03, /*   50:    moveq    r5, #2                             */
		0x00, 0x50, 0x82, 0xe5, /*   54:    str      r5, [r2]                           */
		0x44, 0x20, 0x9d, 0xe5, /*   58:    ldr      r2, [sp, #68]                      */
		0x00, 0x70, 0xa0, 0x01, /*   5c:    moveq    r7, r0                             */
		0x00, 0x50, 0x82, 0xe5, /*   60:    str      r5, [r2]                           */
		0x48, 0x20, 0x9d, 0xe5, /*   64:    ldr      r2, [sp, #72]                      */
		0x04, 0x00, 0x9d, 0x05, /*   68:    ldreq    r0, [sp, #4]                       */
		0x02, 0x00, 0x80, 0x12, /*   6c:    addne    r0, r0, #2                         */
		0x00, 0x00, 0x52, 0xe3, /*   70:    cmp      r2, #0                             */
		0x00, 0x50, 0x82, 0x15, /*   74:    strne    r5, [r2]                           */
		0x00, 0x20, 0x60, 0xe2, /*   78:    rsb      r2, r0, #0                         */
		0x03, 0x20, 0x02, 0xe2, /*   7c:    and      r2, r2, #3                         */
		0x3c, 0x40, 0x82, 0xe2, /*   80:    add      r4, r2, #60                        */
		0x04, 0x00, 0x55, 0xe1, /*   84:    cmp      r5, r4                             */
		0x05, 0x40, 0xa0, 0x31, /*   88:    movcc    r4, r5                             */
		0x04, 0xe0, 0x80, 0xe0, /*   8c:    add      lr, r0, r4                         */
		0x00, 0xc0, 0xa0, 0xe1, /*   90:    mov      ip, r0                             */
		0x0e, 0x00, 0x5c, 0xe1, /*   94:    cmp      ip, lr                             */
		0x1b, 0x00, 0x00, 0x1a, /*   98:    bne      10c <spi_batch_data_transfer+0x10c> */
		0x04, 0x40, 0x45, 0xe0, /*   9c:    sub      r4, r5, r4                         */
		0x00, 0xa0, 0x0f, 0xe1, /*   a0:    mrs      sl, CPSR                           */
		0xc0, 0xc0, 0x8a, 0xe3, /*   a4:    orr      ip, sl, #192                       */
		0x0c, 0xf0, 0x21, 0xe1, /*   a8:    msr      CPSR_c, ip                         */
		0x00, 0xc0, 0x91, 0xe5, /*   ac:    ldr      ip, [r1]                           */
		0x00, 0x80, 0x9d, 0xe5, /*   b0:    ldr      r8, [sp]                           */
		0x02, 0x00, 0x55, 0xe1, /*   b4:    cmp      r5, r2                             */
		0x0c, 0xc0, 0x88, 0xe1, /*   b8:    orr      ip, r8, ip                         */
		0x05, 0x20, 0xa0, 0x31, /*   bc:    movcc    r2, r5                             */
		0x00, 0xc0, 0x81, 0xe5, /*   c0:    str      ip, [r1]                           */
		0x02, 0x80, 0x80, 0xe0, /*   c4:    add      r8, r0, r2                         */
		0x00, 0xc0, 0xa0, 0xe1, /*   c8:    mov      ip, r0                             */
		0x0c, 0x00, 0x58, 0xe1, /*   cc:    cmp      r8, ip                             */
		0x11, 0x00, 0x00, 0x1a, /*   d0:    bne      11c <spi_batch_data_transfer+0x11c> */
		0x02, 0x20, 0x45, 0xe0, /*   d4:    sub      r2, r5, r2                         */
		0x03, 0x00, 0x52, 0xe3, /*   d8:    cmp      r2, #3                             */
		0x14, 0x00, 0x00, 0x8a, /*   dc:    bhi      134 <spi_batch_data_transfer+0x134> */
		0x00, 0x00, 0x52, 0xe3, /*   e0:    cmp      r2, #0                             */
		0x25, 0x00, 0x00, 0x1a, /*   e4:    bne      180 <spi_batch_data_transfer+0x180> */
		0x0a, 0xf0, 0x21, 0xe1, /*   e8:    msr      CPSR_c, sl                         */
		0x09, 0x00, 0x56, 0xe1, /*   ec:    cmp      r6, r9                             */
		0x05, 0x00, 0x80, 0x10, /*   f0:    addne    r0, r0, r5                         */
		0xc7, 0xff, 0xff, 0x1a, /*   f4:    bne      18 <spi_batch_data_transfer+0x18>  */
		0x0d, 0x20, 0xdd, 0xe5, /*   f8:    ldrb     r2, [sp, #13]                      */
		0x01, 0x00, 0x12, 0xe3, /*   fc:    tst      r2, #1                             */
		0x02, 0x00, 0x87, 0x02, /*  100:    addeq    r0, r7, #2                         */
		0x07, 0x00, 0xa0, 0x11, /*  104:    movne    r0, r7                             */
		0xc2, 0xff, 0xff, 0xea, /*  108:    b        18 <spi_batch_data_transfer+0x18>  */
		0x38, 0xa0, 0x9d, 0xe5, /*  10c:    ldr      sl, [sp, #56]                      */
		0x01, 0x80, 0xdc, 0xe4, /*  110:    ldrb     r8, [ip], #1                       */
		0x00, 0x80, 0xca, 0xe5, /*  114:    strb     r8, [sl]                           */
		0xdd, 0xff, 0xff, 0xea, /*  118:    b        94 <spi_batch_data_transfer+0x94>  */
		0x00, 0xb0, 0x93, 0xe5, /*  11c:    ldr      fp, [r3]                           */
		0x7f, 0x00, 0x1b, 0xe3, /*  120:    tst      fp, #127                           */
		0x3c, 0xb0, 0x9d, 0x15, /*  124:    ldrne    fp, [sp, #60]                      */
		0x00, 0xb0, 0xdb, 0x15, /*  128:    ldrbne   fp, [fp]                           */
		0x01, 0xb0, 0xcc, 0x14, /*  12c:    strbne   fp, [ip], #1                       */
		0xe5, 0xff, 0xff, 0xea, /*  130:    b        cc <spi_batch_data_transfer+0xcc>  */
		0x00, 0xb0, 0x93, 0xe5, /*  134:    ldr      fp, [r3]                           */
		0x7c, 0x00, 0x1b, 0xe3, /*  138:    tst      fp, #124                           */
		0x2b, 0x88, 0xa0, 0xe1, /*  13c:    lsr      r8, fp, #16                        */
		0x3c, 0xb0, 0x9d, 0x15, /*  140:    ldrne    fp, [sp, #60]                      */
		0x7f, 0x80, 0x08, 0xe2, /*  144:    and      r8, r8, #127                       */
		0x00, 0xb0, 0x9b, 0x15, /*  148:    ldrne    fp, [fp]                           */
		0x04, 0xb0, 0x8c, 0x14, /*  14c:    strne    fp, [ip], #4                       */
		0x04, 0x20, 0x42, 0x12, /*  150:    subne    r2, r2, #4                         */
		0x3b, 0x00, 0x58, 0xe3, /*  154:    cmp      r8, #59                            */
		0x00, 0x80, 0xa0, 0xc3, /*  158:    movgt    r8, #0                             */
		0x01, 0x80, 0xa0, 0xd3, /*  15c:    movle    r8, #1                             */
		0x03, 0x00, 0x54, 0xe3, /*  160:    cmp      r4, #3                             */
		0x00, 0x80, 0xa0, 0x93, /*  164:    movls    r8, #0                             */
		0x00, 0x00, 0x58, 0xe3, /*  168:    cmp      r8, #0                             */
		0x38, 0xb0, 0x9d, 0x15, /*  16c:    ldrne    fp, [sp, #56]                      */
		0x04, 0x80, 0x9e, 0x14, /*  170:    ldrne    r8, [lr], #4                       */
		0x04, 0x40, 0x44, 0x12, /*  174:    subne    r4, r4, #4                         */
		0x00, 0x80, 0x8b, 0x15, /*  178:    strne    r8, [fp]                           */
		0xd5, 0xff, 0xff, 0xea, /*  17c:    b        d8 <spi_batch_data_transfer+0xd8>  */
		0x00, 0xb0, 0x93, 0xe5, /*  180:    ldr      fp, [r3]                           */
		0x7f, 0x00, 0x1b, 0xe3, /*  184:    tst      fp, #127                           */
		0x2b, 0x88, 0xa0, 0xe1, /*  188:    lsr      r8, fp, #16                        */
		0x3c, 0xb0, 0x9d, 0x15, /*  18c:    ldrne    fp, [sp, #60]                      */
		0x7f, 0x80, 0x08, 0xe2, /*  190:    and      r8, r8, #127                       */
		0x00, 0xb0, 0xdb, 0x15, /*  194:    ldrbne   fp, [fp]                           */
		0x01, 0xb0, 0xcc, 0x14, /*  198:    strbne   fp, [ip], #1                       */
		0x01, 0x20, 0x42, 0x12, /*  19c:    subne    r2, r2, #1                         */
		0x3b, 0x00, 0x58, 0xe3, /*  1a0:    cmp      r8, #59                            */
		0x00, 0x80, 0xa0, 0xc3, /*  1a4:    movgt    r8, #0                             */
		0x01, 0x80, 0xa0, 0xd3, /*  1a8:    movle    r8, #1                             */
		0x00, 0x00, 0x54, 0xe3, /*  1ac:    cmp      r4, #0                             */
		0x00, 0x80, 0xa0, 0x03, /*  1b0:    moveq    r8, #0                             */
		0x00, 0x00, 0x58, 0xe3, /*  1b4:    cmp      r8, #0                             */
		0x38, 0xb0, 0x9d, 0x15, /*  1b8:    ldrne    fp, [sp, #56]                      */
		0x01, 0x80, 0xde, 0x14, /*  1bc:    ldrbne   r8, [lr], #1                       */
		0x01, 0x40, 0x44, 0x12, /*  1c0:    subne    r4, r4, #1                         */
		0x00, 0x80, 0xcb, 0x15, /*  1c4:    strbne   r8, [fp]                           */
		0xc4, 0xff, 0xff, 0xea, /*  1c8:    b        e0 <spi_batch_data_transfer+0xe0>  */
		0x14, 0xd0, 0x8d, 0xe2, /*  1cc:    add      sp, sp, #20                        */
		0xf0, 0x8f, 0xbd, 0xe8, /*  1d0:    pop      {r4, r5, r6, r7, r8, r9, sl, fp, pc} */
		0xff, 0xff, 0x00, 0x00, /*  1d4:    .word    0x0000ffff                         */
	};
	uint32_t args[] = {
		buf,
		spi_ctl_reg,
		spi_ctl_xch_bitmask,
		spi_fifo_reg,
		spi_tx_reg,
		spi_rx_reg,
		spi_bc_reg,
		spi_tc_reg,
		spi_bcc_reg
	};
	aw_fel_remotefunc_prepare(dev, 56, arm_code, sizeof(arm_code), 9, args);
}
