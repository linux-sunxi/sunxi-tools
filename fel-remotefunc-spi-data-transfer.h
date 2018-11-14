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
		0xf0, 0x0f, 0x2d, 0xe9, /*    0:    push     {r4, r5, r6, r7, r8, r9, sl, fp}   */
		0x18, 0xd0, 0x4d, 0xe2, /*    4:    sub      sp, sp, #24                        */
		0x38, 0x50, 0x9d, 0xe5, /*    8:    ldr      r5, [sp, #56]                      */
		0x3c, 0x60, 0x9d, 0xe5, /*    c:    ldr      r6, [sp, #60]                      */
		0x06, 0x00, 0x8d, 0xe9, /*   10:    stmib    sp, {r1, r2}                       */
		0x00, 0xa0, 0xd0, 0xe5, /*   14:    ldrb     sl, [r0]                           */
		0x01, 0x20, 0xd0, 0xe5, /*   18:    ldrb     r2, [r0, #1]                       */
		0x0a, 0xa4, 0x92, 0xe1, /*   1c:    orrs     sl, r2, sl, lsl #8                 */
		0x6a, 0x00, 0x00, 0x0a, /*   20:    beq      1d0 <spi_batch_data_transfer+0x1d0> */
		0xff, 0x2f, 0x0f, 0xe3, /*   24:    movw     r2, #65535                         */
		0x02, 0x00, 0x5a, 0xe1, /*   28:    cmp      sl, r2                             */
		0x18, 0x80, 0x8d, 0x02, /*   2c:    addeq    r8, sp, #24                        */
		0x02, 0x80, 0x80, 0x12, /*   30:    addne    r8, r0, #2                         */
		0x05, 0xb0, 0xa0, 0x03, /*   34:    moveq    fp, #5                             */
		0x48, 0xc0, 0x9d, 0xe5, /*   38:    ldr      ip, [sp, #72]                      */
		0x08, 0xb0, 0x68, 0x05, /*   3c:    strbeq   fp, [r8, #-8]!                     */
		0x00, 0x10, 0x68, 0xe2, /*   40:    rsb      r1, r8, #0                         */
		0x40, 0x20, 0x9d, 0xe5, /*   44:    ldr      r2, [sp, #64]                      */
		0x03, 0x10, 0x01, 0xe2, /*   48:    and      r1, r1, #3                         */
		0x44, 0xb0, 0x9d, 0xe5, /*   4c:    ldr      fp, [sp, #68]                      */
		0x0a, 0x70, 0xa0, 0x11, /*   50:    movne    r7, sl                             */
		0x0c, 0x00, 0x8d, 0x05, /*   54:    streq    r0, [sp, #12]                      */
		0x3c, 0x00, 0x81, 0xe2, /*   58:    add      r0, r1, #60                        */
		0x02, 0x70, 0xa0, 0x03, /*   5c:    moveq    r7, #2                             */
		0x00, 0x00, 0x5c, 0xe3, /*   60:    cmp      ip, #0                             */
		0x00, 0x70, 0x82, 0xe5, /*   64:    str      r7, [r2]                           */
		0x08, 0x20, 0xa0, 0xe1, /*   68:    mov      r2, r8                             */
		0x00, 0x70, 0x8b, 0xe5, /*   6c:    str      r7, [fp]                           */
		0x00, 0x70, 0x8c, 0x15, /*   70:    strne    r7, [ip]                           */
		0x07, 0x00, 0x50, 0xe1, /*   74:    cmp      r0, r7                             */
		0x07, 0x00, 0xa0, 0x21, /*   78:    movcs    r0, r7                             */
		0x00, 0x40, 0x88, 0xe0, /*   7c:    add      r4, r8, r0                         */
		0x01, 0xc0, 0xd2, 0xe4, /*   80:    ldrb     ip, [r2], #1                       */
		0x04, 0x00, 0x52, 0xe1, /*   84:    cmp      r2, r4                             */
		0x00, 0xc0, 0xc5, 0xe5, /*   88:    strb     ip, [r5]                           */
		0xfb, 0xff, 0xff, 0x1a, /*   8c:    bne      80 <spi_batch_data_transfer+0x80>  */
		0x07, 0x00, 0x60, 0xe0, /*   90:    rsb      r0, r0, r7                         */
		0x00, 0x90, 0x0f, 0xe1, /*   94:    mrs      r9, CPSR                           */
		0xc0, 0xc0, 0x89, 0xe3, /*   98:    orr      ip, r9, #192                       */
		0x0c, 0xf0, 0x21, 0xe1, /*   9c:    msr      CPSR_c, ip                         */
		0x04, 0xc0, 0x9d, 0xe5, /*   a0:    ldr      ip, [sp, #4]                       */
		0x07, 0x00, 0x51, 0xe1, /*   a4:    cmp      r1, r7                             */
		0x07, 0x10, 0xa0, 0x21, /*   a8:    movcs    r1, r7                             */
		0x08, 0xb0, 0x9d, 0xe5, /*   ac:    ldr      fp, [sp, #8]                       */
		0x00, 0x40, 0x9c, 0xe5, /*   b0:    ldr      r4, [ip]                           */
		0x01, 0xc0, 0x88, 0xe0, /*   b4:    add      ip, r8, r1                         */
		0x00, 0xc0, 0x8d, 0xe5, /*   b8:    str      ip, [sp]                           */
		0x08, 0xc0, 0xa0, 0xe1, /*   bc:    mov      ip, r8                             */
		0x0b, 0x40, 0x84, 0xe1, /*   c0:    orr      r4, r4, fp                         */
		0x04, 0xb0, 0x9d, 0xe5, /*   c4:    ldr      fp, [sp, #4]                       */
		0x00, 0x40, 0x8b, 0xe5, /*   c8:    str      r4, [fp]                           */
		0x00, 0xb0, 0x9d, 0xe5, /*   cc:    ldr      fp, [sp]                           */
		0x0b, 0x00, 0x5c, 0xe1, /*   d0:    cmp      ip, fp                             */
		0x06, 0x00, 0x00, 0x0a, /*   d4:    beq      f4 <spi_batch_data_transfer+0xf4>  */
		0x00, 0x40, 0x93, 0xe5, /*   d8:    ldr      r4, [r3]                           */
		0x7f, 0x00, 0x14, 0xe3, /*   dc:    tst      r4, #127                           */
		0xfc, 0xff, 0xff, 0x0a, /*   e0:    beq      d8 <spi_batch_data_transfer+0xd8>  */
		0x00, 0x40, 0xd6, 0xe5, /*   e4:    ldrb     r4, [r6]                           */
		0x01, 0x40, 0xcc, 0xe4, /*   e8:    strb     r4, [ip], #1                       */
		0x0b, 0x00, 0x5c, 0xe1, /*   ec:    cmp      ip, fp                             */
		0xf8, 0xff, 0xff, 0x1a, /*   f0:    bne      d8 <spi_batch_data_transfer+0xd8>  */
		0x07, 0x10, 0x61, 0xe0, /*   f4:    rsb      r1, r1, r7                         */
		0x03, 0x00, 0x51, 0xe3, /*   f8:    cmp      r1, #3                             */
		0x12, 0x00, 0x00, 0x9a, /*   fc:    bls      14c <spi_batch_data_transfer+0x14c> */
		0x00, 0x40, 0x93, 0xe5, /*  100:    ldr      r4, [r3]                           */
		0x7f, 0xb0, 0x04, 0xe2, /*  104:    and      fp, r4, #127                       */
		0x54, 0x48, 0xe6, 0xe7, /*  108:    ubfx     r4, r4, #16, #7                    */
		0x03, 0x00, 0x5b, 0xe3, /*  10c:    cmp      fp, #3                             */
		0x04, 0x10, 0x41, 0xc2, /*  110:    subgt    r1, r1, #4                         */
		0x00, 0xb0, 0x96, 0xc5, /*  114:    ldrgt    fp, [r6]                           */
		0x04, 0xb0, 0x8c, 0xc4, /*  118:    strgt    fp, [ip], #4                       */
		0x03, 0x00, 0x50, 0xe3, /*  11c:    cmp      r0, #3                             */
		0x00, 0xb0, 0xa0, 0x93, /*  120:    movls    fp, #0                             */
		0x01, 0xb0, 0xa0, 0x83, /*  124:    movhi    fp, #1                             */
		0x3b, 0x00, 0x54, 0xe3, /*  128:    cmp      r4, #59                            */
		0x00, 0xb0, 0xa0, 0xc3, /*  12c:    movgt    fp, #0                             */
		0x00, 0x00, 0x5b, 0xe3, /*  130:    cmp      fp, #0                             */
		0xef, 0xff, 0xff, 0x0a, /*  134:    beq      f8 <spi_batch_data_transfer+0xf8>  */
		0x04, 0x40, 0x92, 0xe4, /*  138:    ldr      r4, [r2], #4                       */
		0x03, 0x00, 0x51, 0xe3, /*  13c:    cmp      r1, #3                             */
		0x04, 0x00, 0x40, 0xe2, /*  140:    sub      r0, r0, #4                         */
		0x00, 0x40, 0x85, 0xe5, /*  144:    str      r4, [r5]                           */
		0xec, 0xff, 0xff, 0x8a, /*  148:    bhi      100 <spi_batch_data_transfer+0x100> */
		0x00, 0x00, 0x51, 0xe3, /*  14c:    cmp      r1, #0                             */
		0x10, 0x00, 0x00, 0x0a, /*  150:    beq      198 <spi_batch_data_transfer+0x198> */
		0x00, 0x40, 0x93, 0xe5, /*  154:    ldr      r4, [r3]                           */
		0x7f, 0x00, 0x14, 0xe3, /*  158:    tst      r4, #127                           */
		0x54, 0x48, 0xe6, 0xe7, /*  15c:    ubfx     r4, r4, #16, #7                    */
		0x01, 0x10, 0x41, 0x12, /*  160:    subne    r1, r1, #1                         */
		0x00, 0xb0, 0xd6, 0x15, /*  164:    ldrbne   fp, [r6]                           */
		0x01, 0xb0, 0xcc, 0x14, /*  168:    strbne   fp, [ip], #1                       */
		0x00, 0xb0, 0x90, 0xe2, /*  16c:    adds     fp, r0, #0                         */
		0x01, 0xb0, 0xa0, 0x13, /*  170:    movne    fp, #1                             */
		0x3b, 0x00, 0x54, 0xe3, /*  174:    cmp      r4, #59                            */
		0x00, 0xb0, 0xa0, 0xc3, /*  178:    movgt    fp, #0                             */
		0x00, 0x00, 0x5b, 0xe3, /*  17c:    cmp      fp, #0                             */
		0xf1, 0xff, 0xff, 0x0a, /*  180:    beq      14c <spi_batch_data_transfer+0x14c> */
		0x01, 0x40, 0xd2, 0xe4, /*  184:    ldrb     r4, [r2], #1                       */
		0x00, 0x00, 0x51, 0xe3, /*  188:    cmp      r1, #0                             */
		0x01, 0x00, 0x40, 0xe2, /*  18c:    sub      r0, r0, #1                         */
		0x00, 0x40, 0xc5, 0xe5, /*  190:    strb     r4, [r5]                           */
		0xee, 0xff, 0xff, 0x1a, /*  194:    bne      154 <spi_batch_data_transfer+0x154> */
		0x09, 0xf0, 0x21, 0xe1, /*  198:    msr      CPSR_c, r9                         */
		0xff, 0xcf, 0x0f, 0xe3, /*  19c:    movw     ip, #65535                         */
		0x0c, 0x00, 0x5a, 0xe1, /*  1a0:    cmp      sl, ip                             */
		0x07, 0x00, 0x88, 0x10, /*  1a4:    addne    r0, r8, r7                         */
		0x99, 0xff, 0xff, 0x1a, /*  1a8:    bne      14 <spi_batch_data_transfer+0x14>  */
		0x11, 0x20, 0xdd, 0xe5, /*  1ac:    ldrb     r2, [sp, #17]                      */
		0x01, 0x00, 0x12, 0xe3, /*  1b0:    tst      r2, #1                             */
		0x08, 0x00, 0x00, 0x1a, /*  1b4:    bne      1dc <spi_batch_data_transfer+0x1dc> */
		0x0c, 0xb0, 0x9d, 0xe5, /*  1b8:    ldr      fp, [sp, #12]                      */
		0x02, 0x00, 0x8b, 0xe2, /*  1bc:    add      r0, fp, #2                         */
		0x00, 0xa0, 0xd0, 0xe5, /*  1c0:    ldrb     sl, [r0]                           */
		0x01, 0x20, 0xd0, 0xe5, /*  1c4:    ldrb     r2, [r0, #1]                       */
		0x0a, 0xa4, 0x92, 0xe1, /*  1c8:    orrs     sl, r2, sl, lsl #8                 */
		0x94, 0xff, 0xff, 0x1a, /*  1cc:    bne      24 <spi_batch_data_transfer+0x24>  */
		0x18, 0xd0, 0x8d, 0xe2, /*  1d0:    add      sp, sp, #24                        */
		0xf0, 0x0f, 0xbd, 0xe8, /*  1d4:    pop      {r4, r5, r6, r7, r8, r9, sl, fp}   */
		0x1e, 0xff, 0x2f, 0xe1, /*  1d8:    bx       lr                                 */
		0x0c, 0x00, 0x9d, 0xe5, /*  1dc:    ldr      r0, [sp, #12]                      */
		0x8b, 0xff, 0xff, 0xea, /*  1e0:    b        14 <spi_batch_data_transfer+0x14>  */
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
