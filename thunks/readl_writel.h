		/* <fel_readl_n>: */
		htole32(0xe59f0020), /*    0:  ldr   r0, [pc, #32]           */
		htole32(0xe28f1024), /*    4:  add   r1, pc, #36             */
		htole32(0xe59f201c), /*    8:  ldr   r2, [pc, #28]           */
		htole32(0xe35200f4), /*    c:  cmp   r2, #244                */
		htole32(0xc3a020f4), /*   10:  movgt r2, #244                */
		/* <read_loop>: */
		htole32(0xe2522001), /*   14:  subs  r2, r2, #1              */
		htole32(0x412fff1e), /*   18:  bxmi  lr                      */
		htole32(0xe4903004), /*   1c:  ldr   r3, [r0], #4            */
		htole32(0xe4813004), /*   20:  str   r3, [r1], #4            */
		htole32(0xeafffffa), /*   24:  b     14 <read_loop>          */
		/* <fel_writel_n>: */
		htole32(0xe59f0020), /*   34:  ldr   r0, [pc, #32]           */
		htole32(0xe28f1024), /*   38:  add   r1, pc, #36             */
		htole32(0xe59f201c), /*   3c:  ldr   r2, [pc, #28]           */
		htole32(0xe35200f4), /*   40:  cmp   r2, #244                */
		htole32(0xc3a020f4), /*   44:  movgt r2, #244                */
		/* <write_loop>: */
		htole32(0xe2522001), /*   48:  subs  r2, r2, #1              */
		htole32(0x412fff1e), /*   4c:  bxmi  lr                      */
		htole32(0xe4913004), /*   50:  ldr   r3, [r1], #4            */
		htole32(0xe4803004), /*   54:  str   r3, [r0], #4            */
		htole32(0xeafffffa), /*   58:  b     48 <write_loop>         */
