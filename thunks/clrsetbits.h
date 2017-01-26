		/* <fel_clrsetbits_le32>: */
		htole32(0xe59f0018), /*    0:  ldr   r0, [pc, #24]           */
		htole32(0xe5901000), /*    4:  ldr   r1, [r0]                */
		htole32(0xe59f2014), /*    8:  ldr   r2, [pc, #20]           */
		htole32(0xe1c11002), /*    c:  bic   r1, r1, r2              */
		htole32(0xe59f2010), /*   10:  ldr   r2, [pc, #16]           */
		htole32(0xe1811002), /*   14:  orr   r1, r1, r2              */
		htole32(0xe5801000), /*   18:  str   r1, [r0]                */
		htole32(0xe12fff1e), /*   1c:  bx    lr                      */
