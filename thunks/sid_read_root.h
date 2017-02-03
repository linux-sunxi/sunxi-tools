		/* <sid_read_root_key>: */
		htole32(0xe59f0040), /*    0:  ldr   r0, [pc, #64]           */
		htole32(0xe3a01000), /*    4:  mov   r1, #0                  */
		htole32(0xe28f303c), /*    8:  add   r3, pc, #60             */
		/* <sid_read_loop>: */
		htole32(0xe1a02801), /*    c:  lsl   r2, r1, #16             */
		htole32(0xe3822b2b), /*   10:  orr   r2, r2, #44032          */
		htole32(0xe3822002), /*   14:  orr   r2, r2, #2              */
		htole32(0xe5802040), /*   18:  str   r2, [r0, #64]           */
		/* <sid_read_wait>: */
		htole32(0xe5902040), /*   1c:  ldr   r2, [r0, #64]           */
		htole32(0xe3120002), /*   20:  tst   r2, #2                  */
		htole32(0x1afffffc), /*   24:  bne   1c <sid_read_wait>      */
		htole32(0xe5902060), /*   28:  ldr   r2, [r0, #96]           */
		htole32(0xe7832001), /*   2c:  str   r2, [r3, r1]            */
		htole32(0xe2811004), /*   30:  add   r1, r1, #4              */
		htole32(0xe3510010), /*   34:  cmp   r1, #16                 */
		htole32(0x3afffff3), /*   38:  bcc   c <sid_read_loop>       */
		htole32(0xe3a02000), /*   3c:  mov   r2, #0                  */
		htole32(0xe5802040), /*   40:  str   r2, [r0, #64]           */
		htole32(0xe12fff1e), /*   44:  bx    lr                      */
		/* <sid_base>: */
		htole32(0x00000000), /*   48:  .word 0x00000000              */
		/* <sid_result>: */
