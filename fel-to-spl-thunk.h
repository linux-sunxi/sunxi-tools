	0xea000015, /*        0:    b          5c <setup_stack>             */
	0xe1a00000, /*        4:    nop                                     */
	0xe1a00000, /*        8:    nop                                     */
	0xe1a00000, /*        c:    nop                                     */
	0xe1a00000, /*       10:    nop                                     */
	0xe1a00000, /*       14:    nop                                     */
	0xe1a00000, /*       18:    nop                                     */
	0xe1a00000, /*       1c:    nop                                     */
	0xe1a00000, /*       20:    nop                                     */
	0xe1a00000, /*       24:    nop                                     */
	0xe28f40e4, /*       28:    add        r4, pc, #228                 */
	0xe4940004, /*       2c:    ldr        r0, [r4], #4                 */
	0xe4941004, /*       30:    ldr        r1, [r4], #4                 */
	0xe4946004, /*       34:    ldr        r6, [r4], #4                 */
	0xe3560000, /*       38:    cmp        r6, #0                       */
	0x012fff1e, /*       3c:    bxeq       lr                           */
	0xe5902000, /*       40:    ldr        r2, [r0]                     */
	0xe5913000, /*       44:    ldr        r3, [r1]                     */
	0xe2566004, /*       48:    subs       r6, r6, #4                   */
	0xe4812004, /*       4c:    str        r2, [r1], #4                 */
	0xe4803004, /*       50:    str        r3, [r0], #4                 */
	0x1afffff9, /*       54:    bne        40 <swap_next_word>          */
	0xeafffff3, /*       58:    b          2c <swap_next_buffer>        */
	0xe24f0040, /*       5c:    sub        r0, pc, #64                  */
	0xe520d004, /*       60:    str        sp, [r0, #-4]!               */
	0xe1a0d000, /*       64:    mov        sp, r0                       */
	0xe10f2000, /*       68:    mrs        r2, CPSR                     */
	0xe92d4004, /*       6c:    push       {r2, lr}                     */
	0xe38220c0, /*       70:    orr        r2, r2, #192                 */
	0xe121f002, /*       74:    msr        CPSR_c, r2                   */
	0xee112f10, /*       78:    mrc        15, 0, r2, cr1, cr0, {0}     */
	0xe3013004, /*       7c:    movw       r3, #4100                    */
	0xe1120003, /*       80:    tst        r2, r3                       */
	0x1a000014, /*       84:    bne        dc <cache_is_unsupported>    */
	0xebffffe6, /*       88:    bl         28 <swap_all_buffers>        */
	0xe3067c39, /*       8c:    movw       r7, #27705                   */
	0xe3457f0a, /*       90:    movt       r7, #24330                   */
	0xe3a00000, /*       94:    mov        r0, #0                       */
	0xe5905010, /*       98:    ldr        r5, [r0, #16]                */
	0xe4902004, /*       9c:    ldr        r2, [r0], #4                 */
	0xe2555004, /*       a0:    subs       r5, r5, #4                   */
	0xe0877002, /*       a4:    add        r7, r7, r2                   */
	0x1afffffb, /*       a8:    bne        9c <check_next_word>         */
	0xe3a00000, /*       ac:    mov        r0, #0                       */
	0xe590200c, /*       b0:    ldr        r2, [r0, #12]                */
	0xe0577082, /*       b4:    subs       r7, r7, r2, lsl #1           */
	0x1a00000c, /*       b8:    bne        f0 <checksum_is_bad>         */
	0xe3a00000, /*       bc:    mov        r0, #0                       */
	0xe304262e, /*       c0:    movw       r2, #17966                   */
	0xe3442c45, /*       c4:    movt       r2, #19525                   */
	0xe5802008, /*       c8:    str        r2, [r0, #8]                 */
	0xf57ff04f, /*       cc:    dsb        sy                           */
	0xf57ff06f, /*       d0:    isb        sy                           */
	0xe12fff30, /*       d4:    blx        r0                           */
	0xea000008, /*       d8:    b          100 <return_to_fel>          */
	0xe3a00000, /*       dc:    mov        r0, #0                       */
	0xe3032f2e, /*       e0:    movw       r2, #16174                   */
	0xe3432f3f, /*       e4:    movt       r2, #16191                   */
	0xe5802008, /*       e8:    str        r2, [r0, #8]                 */
	0xea000004, /*       ec:    b          104 <return_to_fel_noswap>   */
	0xe3a00000, /*       f0:    mov        r0, #0                       */
	0xe304222e, /*       f4:    movw       r2, #16942                   */
	0xe3442441, /*       f8:    movt       r2, #17473                   */
	0xe5802008, /*       fc:    str        r2, [r0, #8]                 */
	0xebffffc8, /*      100:    bl         28 <swap_all_buffers>        */
	0xe8bd4004, /*      104:    pop        {r2, lr}                     */
	0xe121f002, /*      108:    msr        CPSR_c, r2                   */
	0xe59dd000, /*      10c:    ldr        sp, [sp]                     */
	0xe12fff1e, /*      110:    bx         lr                           */
