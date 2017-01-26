		/* <fel_memcpy_up>: */
		htole32(0xe59f0054), /*    0:  ldr   r0, [pc, #84]           */
		htole32(0xe59f1054), /*    4:  ldr   r1, [pc, #84]           */
		htole32(0xe59f2054), /*    8:  ldr   r2, [pc, #84]           */
		htole32(0xe0413000), /*    c:  sub   r3, r1, r0              */
		htole32(0xe3130003), /*   10:  tst   r3, #3                  */
		htole32(0x1a00000b), /*   14:  bne   48 <copyup_tail>        */
		/* <copyup_head>: */
		htole32(0xe3110003), /*   18:  tst   r1, #3                  */
		htole32(0x0a000004), /*   1c:  beq   34 <copyup_loop>        */
		htole32(0xe4d13001), /*   20:  ldrb  r3, [r1], #1            */
		htole32(0xe4c03001), /*   24:  strb  r3, [r0], #1            */
		htole32(0xe2522001), /*   28:  subs  r2, r2, #1              */
		htole32(0x5afffff9), /*   2c:  bpl   18 <copyup_head>        */
		htole32(0xe12fff1e), /*   30:  bx    lr                      */
		/* <copyup_loop>: */
		htole32(0xe2522004), /*   34:  subs  r2, r2, #4              */
		htole32(0x54913004), /*   38:  ldrpl r3, [r1], #4            */
		htole32(0x54803004), /*   3c:  strpl r3, [r0], #4            */
		htole32(0x5afffffb), /*   40:  bpl   34 <copyup_loop>        */
		htole32(0xe2822004), /*   44:  add   r2, r2, #4              */
		/* <copyup_tail>: */
		htole32(0xe2522001), /*   48:  subs  r2, r2, #1              */
		htole32(0x412fff1e), /*   4c:  bxmi  lr                      */
		htole32(0xe4d13001), /*   50:  ldrb  r3, [r1], #1            */
		htole32(0xe4c03001), /*   54:  strb  r3, [r0], #1            */
		htole32(0xeafffffa), /*   58:  b     48 <copyup_tail>        */
		/* <fel_memcpy_down>: */
		htole32(0xe59f0058), /*   68:  ldr   r0, [pc, #88]           */
		htole32(0xe59f1058), /*   6c:  ldr   r1, [pc, #88]           */
		htole32(0xe59f2058), /*   70:  ldr   r2, [pc, #88]           */
		htole32(0xe0403001), /*   74:  sub   r3, r0, r1              */
		htole32(0xe3130003), /*   78:  tst   r3, #3                  */
		htole32(0x1a00000c), /*   7c:  bne   b4 <copydn_tail>        */
		/* <copydn_head>: */
		htole32(0xe0813002), /*   80:  add   r3, r1, r2              */
		htole32(0xe3130003), /*   84:  tst   r3, #3                  */
		htole32(0x0a000004), /*   88:  beq   a0 <copydn_loop>        */
		htole32(0xe2522001), /*   8c:  subs  r2, r2, #1              */
		htole32(0x412fff1e), /*   90:  bxmi  lr                      */
		htole32(0xe7d13002), /*   94:  ldrb  r3, [r1, r2]            */
		htole32(0xe7c03002), /*   98:  strb  r3, [r0, r2]            */
		htole32(0xeafffff7), /*   9c:  b     80 <copydn_head>        */
		/* <copydn_loop>: */
		htole32(0xe2522004), /*   a0:  subs  r2, r2, #4              */
		htole32(0x57913002), /*   a4:  ldrpl r3, [r1, r2]            */
		htole32(0x57803002), /*   a8:  strpl r3, [r0, r2]            */
		htole32(0x5afffffb), /*   ac:  bpl   a0 <copydn_loop>        */
		htole32(0xe2822004), /*   b0:  add   r2, r2, #4              */
		/* <copydn_tail>: */
		htole32(0xe2522001), /*   b4:  subs  r2, r2, #1              */
		htole32(0x412fff1e), /*   b8:  bxmi  lr                      */
		htole32(0xe7d13002), /*   bc:  ldrb  r3, [r1, r2]            */
		htole32(0xe7c03002), /*   c0:  strb  r3, [r0, r2]            */
		htole32(0xeafffffa), /*   c4:  b     b4 <copydn_tail>        */
