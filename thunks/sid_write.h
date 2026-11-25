		/* <sid_write>: */
		htole32(0xe59f0044), /*    0:  ldr   r0, [pc, #68] @ 4c <sid_base> */
		htole32(0xe59f1044), /*    4:  ldr   r1, [pc, #68] @ 50 <offset> */
		htole32(0xe28f3048), /*    8:  add   r3, pc, #72 @ 0x48      */
		/* <sid_write_loop>: */
		htole32(0xe4932004), /*    c:  ldr   r2, [r3], #4            */
		htole32(0xe5802050), /*   10:  str   r2, [r0, #80] @ 0x50    */
		htole32(0xe1a02801), /*   14:  lsl   r2, r1, #16             */
		htole32(0xe3822b2b), /*   18:  orr   r2, r2, #44032 @ 0xac00 */
		htole32(0xe3822001), /*   1c:  orr   r2, r2, #1              */
		htole32(0xe5802040), /*   20:  str   r2, [r0, #64] @ 0x40    */
		/* <sid_write_wait>: */
		htole32(0xe5902040), /*   24:  ldr   r2, [r0, #64] @ 0x40    */
		htole32(0xe3120001), /*   28:  tst   r2, #1                  */
		htole32(0x1afffffc), /*   2c:  bne   24 <sid_write_wait>     */
		htole32(0xe2811004), /*   30:  add   r1, r1, #4              */
		htole32(0xe59f2018), /*   34:  ldr   r2, [pc, #24] @ 54 <end> */
		htole32(0xe1510002), /*   38:  cmp   r1, r2                  */
		htole32(0x3afffff2), /*   3c:  bcc   c <sid_write_loop>      */
		htole32(0xe3a02000), /*   40:  mov   r2, #0                  */
		htole32(0xe5802040), /*   44:  str   r2, [r0, #64] @ 0x40    */
		htole32(0xe12fff1e), /*   48:  bx    lr                      */
		/* <sid_base>: */
		htole32(0x00000000), /*   4c:  .word 0x00000000              */
		/* <offset>: */
		htole32(0x00000000), /*   50:  .word 0x00000000              */
		/* <end>: */
		htole32(0x00000000), /*   54:  .word 0x00000000              */
