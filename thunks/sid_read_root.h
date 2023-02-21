		/* <sid_read_root_key>: */
		htole32(0xe59f0044), /*    0:  ldr   r0, [pc, #68]           */
		htole32(0xe59f1044), /*    4:  ldr   r1, [pc, #68]           */
		htole32(0xe28f3048), /*    8:  add   r3, pc, #72             */
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
		htole32(0xe4832004), /*   2c:  str   r2, [r3], #4            */
		htole32(0xe2811004), /*   30:  add   r1, r1, #4              */
		htole32(0xe59f2018), /*   34:  ldr   r2, [pc, #24]           */
		htole32(0xe1510002), /*   38:  cmp   r1, r2                  */
		htole32(0x3afffff2), /*   3c:  bcc   c <sid_read_loop>       */
		htole32(0xe3a02000), /*   40:  mov   r2, #0                  */
		htole32(0xe5802040), /*   44:  str   r2, [r0, #64]           */
		htole32(0xe12fff1e), /*   48:  bx    lr                      */
		/* <sid_base>: */
		htole32(0x00000000), /*   4c:  .word 0x00000000              */
		/* <offset>: */
		htole32(0x00000000), /*   50:  .word 0x00000000              */
		/* <end>: */
		htole32(0x00000000), /*   54:  .word 0x00000000              */
