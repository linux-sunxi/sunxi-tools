		/* <rmr_request>: */
		htole32(0xe59f0028), /*    0:  ldr   r0, [pc, #40]           */
		htole32(0xe59f1028), /*    4:  ldr   r1, [pc, #40]           */
		htole32(0xe5801000), /*    8:  str   r1, [r0]                */
		htole32(0xf57ff04f), /*    c:  dsb   sy                      */
		htole32(0xf57ff06f), /*   10:  isb   sy                      */
		htole32(0xe59f101c), /*   14:  ldr   r1, [pc, #28]           */
		htole32(0xee1c0f50), /*   18:  mrc   15, 0, r0, cr12, cr0, {2} */
		htole32(0xe1800001), /*   1c:  orr   r0, r0, r1              */
		htole32(0xee0c0f50), /*   20:  mcr   15, 0, r0, cr12, cr0, {2} */
		htole32(0xf57ff06f), /*   24:  isb   sy                      */
		htole32(0xe320f003), /*   28:  wfi                           */
		htole32(0xeafffffd), /*   2c:  b     28 <rmr_request+0x28>   */
