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
	0xe28f40dc, /*       28:    add        r4, pc, #220                 */
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
	0xe59f80a4, /*       5c:    ldr        r8, [pc, #164]               */
	0xe24f0044, /*       60:    sub        r0, pc, #68                  */
	0xe520d004, /*       64:    str        sp, [r0, #-4]!               */
	0xe1a0d000, /*       68:    mov        sp, r0                       */
	0xe10f2000, /*       6c:    mrs        r2, CPSR                     */
	0xe92d4004, /*       70:    push       {r2, lr}                     */
	0xe38220c0, /*       74:    orr        r2, r2, #192                 */
	0xe121f002, /*       78:    msr        CPSR_c, r2                   */
	0xee112f10, /*       7c:    mrc        15, 0, r2, cr1, cr0, {0}     */
	0xe3013004, /*       80:    movw       r3, #4100                    */
	0xe1120003, /*       84:    tst        r2, r3                       */
	0x1a000012, /*       88:    bne        d8 <cache_is_unsupported>    */
	0xebffffe5, /*       8c:    bl         28 <swap_all_buffers>        */
	0xe3067c39, /*       90:    movw       r7, #27705                   */
	0xe3457f0a, /*       94:    movt       r7, #24330                   */
	0xe1a00008, /*       98:    mov        r0, r8                       */
	0xe5905010, /*       9c:    ldr        r5, [r0, #16]                */
	0xe4902004, /*       a0:    ldr        r2, [r0], #4                 */
	0xe2555004, /*       a4:    subs       r5, r5, #4                   */
	0xe0877002, /*       a8:    add        r7, r7, r2                   */
	0x1afffffb, /*       ac:    bne        a0 <check_next_word>         */
	0xe598200c, /*       b0:    ldr        r2, [r8, #12]                */
	0xe0577082, /*       b4:    subs       r7, r7, r2, lsl #1           */
	0x1a00000a, /*       b8:    bne        e8 <checksum_is_bad>         */
	0xe304262e, /*       bc:    movw       r2, #17966                   */
	0xe3442c45, /*       c0:    movt       r2, #19525                   */
	0xe5882008, /*       c4:    str        r2, [r8, #8]                 */
	0xf57ff04f, /*       c8:    dsb        sy                           */
	0xf57ff06f, /*       cc:    isb        sy                           */
	0xe12fff38, /*       d0:    blx        r8                           */
	0xea000006, /*       d4:    b          f4 <return_to_fel>           */
	0xe3032f2e, /*       d8:    movw       r2, #16174                   */
	0xe3432f3f, /*       dc:    movt       r2, #16191                   */
	0xe5882008, /*       e0:    str        r2, [r8, #8]                 */
	0xea000003, /*       e4:    b          f8 <return_to_fel_noswap>    */
	0xe304222e, /*       e8:    movw       r2, #16942                   */
	0xe3442441, /*       ec:    movt       r2, #17473                   */
	0xe5882008, /*       f0:    str        r2, [r8, #8]                 */
	0xebffffcb, /*       f4:    bl         28 <swap_all_buffers>        */
	0xe8bd4004, /*       f8:    pop        {r2, lr}                     */
	0xe121f002, /*       fc:    msr        CPSR_c, r2                   */
	0xe59dd000, /*      100:    ldr        sp, [sp]                     */
	0xe12fff1e, /*      104:    bx         lr                           */
