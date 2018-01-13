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
	0xe28f40e0, /*       28:    add        r4, pc, #224                 */
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
	0xe59f80a8, /*       5c:    ldr        r8, [pc, #168]               */
	0xe24f0044, /*       60:    sub        r0, pc, #68                  */
	0xe520d004, /*       64:    str        sp, [r0, #-4]!               */
	0xe1a0d000, /*       68:    mov        sp, r0                       */
	0xe10f2000, /*       6c:    mrs        r2, CPSR                     */
	0xe92d4004, /*       70:    push       {r2, lr}                     */
	0xe38220c0, /*       74:    orr        r2, r2, #192                 */
	0xe121f002, /*       78:    msr        CPSR_c, r2                   */
	0xee112f10, /*       7c:    mrc        15, 0, r2, cr1, cr0, {0}     */
	0xe59f3070, /*       80:    ldr        r3, [pc, #112]               */
	0xe1120003, /*       84:    tst        r2, r3                       */
	0x1a000010, /*       88:    bne        d0 <cache_is_unsupported>    */
	0xebffffe5, /*       8c:    bl         28 <swap_all_buffers>        */
	0xe59f7064, /*       90:    ldr        r7, [pc, #100]               */
	0xe1a00008, /*       94:    mov        r0, r8                       */
	0xe5905010, /*       98:    ldr        r5, [r0, #16]                */
	0xe4902004, /*       9c:    ldr        r2, [r0], #4                 */
	0xe2555004, /*       a0:    subs       r5, r5, #4                   */
	0xe0877002, /*       a4:    add        r7, r7, r2                   */
	0x1afffffb, /*       a8:    bne        9c <check_next_word>         */
	0xe598200c, /*       ac:    ldr        r2, [r8, #12]                */
	0xe0577082, /*       b0:    subs       r7, r7, r2, lsl #1           */
	0x1a000008, /*       b4:    bne        dc <checksum_is_bad>         */
	0xe59f2040, /*       b8:    ldr        r2, [pc, #64]                */
	0xe5882008, /*       bc:    str        r2, [r8, #8]                 */
	0xe3a02000, /*       c0:    mov        r2, #0                       */
	0xee072f9a, /*       c4:    mcr        15, 0, r2, cr7, cr10, {4}    */
	0xe12fff38, /*       c8:    blx        r8                           */
	0xea000004, /*       cc:    b          e4 <return_to_fel>           */
	0xe59f202c, /*       d0:    ldr        r2, [pc, #44]                */
	0xe5882008, /*       d4:    str        r2, [r8, #8]                 */
	0xea000002, /*       d8:    b          e8 <return_to_fel_noswap>    */
	0xe59f2024, /*       dc:    ldr        r2, [pc, #36]                */
	0xe5882008, /*       e0:    str        r2, [r8, #8]                 */
	0xebffffcf, /*       e4:    bl         28 <swap_all_buffers>        */
	0xe8bd4004, /*       e8:    pop        {r2, lr}                     */
	0xe121f002, /*       ec:    msr        CPSR_c, r2                   */
	0xe59dd000, /*       f0:    ldr        sp, [sp]                     */
	0xe12fff1e, /*       f4:    bx         lr                           */
	0x00001004, /*       f8:    .word      0x00001004                   */
	0x5f0a6c39, /*       fc:    .word      0x5f0a6c39                   */
	0x4c45462e, /*      100:    .word      0x4c45462e                   */
	0x3f3f3f2e, /*      104:    .word      0x3f3f3f2e                   */
	0x4441422e, /*      108:    .word      0x4441422e                   */
