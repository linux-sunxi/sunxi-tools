/*
 * Copyright © 2016 Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

typedef unsigned int  u32;
typedef unsigned char u8;

#define readl(addr)		(*((volatile u32 *)(addr)))
#define writel(v, addr)		(*((volatile u32 *)(addr)) = (u32)(v))
#define readb(addr)		(*((volatile u8 *)(addr)))
#define writeb(v, addr)		(*((volatile u8 *)(addr)) = (u8)(v))

/*
 * This is a basic full-duplex SPI data transfer function (we are sending a
 * block of data and receiving the same amount of data back), doing the job
 * without any help from DMA. And because we can be running in some rather
 * adverse conditions (with default PMIC settings, low CPU clock speed and
 * CPU caches disabled), it is necessary to use 32-bit accesses to read/write
 * the FIFO buffers. As a result, Allwinner A13 with the default 408MHz CPU
 * clock speed can successfully handle at least 12 MHz SPI clock speed.
 *
 * Supports both sun4i and sun6i variants of the SPI controller (they only
 * need different hardware register addresses passed as arguments).
 */
static void inline __attribute((always_inline)) spi_data_transfer(void *buf,
							   u32   bufsize,
							   void *spi_ctl_reg,
							   u32   spi_ctl_xch_bitmask,
							   void *spi_fifo_reg,
							   void *spi_tx_reg,
							   void *spi_rx_reg,
							   void *spi_bc_reg,
							   void *spi_tc_reg,
							   void *spi_bcc_reg)
{
	u32  cnt;
	u32  rxsize  = bufsize;
	u32  txsize  = bufsize;
	u8  *rxbuf8 = buf;
	u8  *txbuf8 = buf;
	u32 *rxbuf;
	u32 *txbuf;
	u32  cpsr;

	/* sun6i uses 3 registers, sun4i only needs 2 */
	writel(bufsize, spi_bc_reg);
	writel(bufsize, spi_tc_reg);
	if (spi_bcc_reg)
		writel(bufsize, spi_bcc_reg);

	/* Fill the TX buffer with some initial data */
	cnt = (-(u32)txbuf8 & 3) + 60;
	if (cnt > txsize)
		cnt = txsize;
	while (cnt-- > 0) {
		writeb(*txbuf8++, spi_tx_reg);
		txsize--;
	}

	/* Temporarily disable IRQ & FIQ */
	asm volatile("mrs %0, cpsr" : "=r" (cpsr));
	asm volatile("msr cpsr_c, %0" :: "r" (cpsr | 0xC0));

	/* Start the data transfer */
	writel(readl(spi_ctl_reg) | spi_ctl_xch_bitmask, spi_ctl_reg);

	/* Read the initial unaligned part of the data */
	cnt = (-(u32)rxbuf8 & 3);
	if (cnt > rxsize)
		cnt = rxsize;
	while (cnt > 0) {
		u32 fiforeg = readl(spi_fifo_reg);
		int rxfifo = fiforeg & 0x7F;
		if (rxfifo > 0) {
			*rxbuf8++ = readb(spi_rx_reg);
			cnt--;
			rxsize--;
		}
	}

	/* Fast processing of the aligned part (read/write 32-bit at a time) */
	rxbuf = (u32 *)rxbuf8;
	txbuf = (u32 *)txbuf8;
	while (rxsize >= 4) {
		u32 fiforeg = readl(spi_fifo_reg);
		int rxfifo = fiforeg & 0x7F;
		int txfifo = (fiforeg >> 16) & 0x7F;
		if (rxfifo >= 4) {
			*rxbuf++ = readl(spi_rx_reg);
			rxsize -= 4;
		}
		if (txfifo < 60 && txsize >= 4) {
			writel(*txbuf++, spi_tx_reg);
			txsize -= 4;
		}
	}

	/* Handle the trailing part pf the data */
	rxbuf8 = (u8 *)rxbuf;
	txbuf8 = (u8 *)txbuf;
	while (rxsize >= 1) {
		u32 fiforeg = readl(spi_fifo_reg);
		int rxfifo = fiforeg & 0x7F;
		int txfifo = (fiforeg >> 16) & 0x7F;
		if (rxfifo >= 1) {
			*rxbuf8++ = readb(spi_rx_reg);
			rxsize -= 1;
		}
		if (txfifo < 60 && txsize >= 1) {
			writeb(*txbuf8++, spi_tx_reg);
			txsize -= 1;
		}
	}

	/* Restore CPSR */
	asm volatile("msr cpsr_c, %0" :: "r" (cpsr));
}

void spi_batch_data_transfer(u8   *buf,
			     void *spi_ctl_reg,
			     u32   spi_ctl_xch_bitmask,
			     void *spi_fifo_reg,
			     void *spi_tx_reg,
			     void *spi_rx_reg,
			     void *spi_bc_reg,
			     void *spi_tc_reg,
			     void *spi_bcc_reg)
{
	u8 wait_for_completion_cmd[2];
	u8 *backup_buf;
	u32 bufsize;

	while (1) {
		u32 code = (buf[0] << 8) | buf[1];

		/* End of data */
		if (code == 0)
			return;

		if (code == 0xFFFF) {
			/* Wait for completion, part 1 */
			backup_buf = buf;
			buf = wait_for_completion_cmd;
			wait_for_completion_cmd[0] = 0x05;
			bufsize = 2;
		} else {
			/* Normal buffer */
			buf += 2;
			bufsize = code;
		}

		spi_data_transfer(buf, bufsize, spi_ctl_reg, spi_ctl_xch_bitmask,
						spi_fifo_reg, spi_tx_reg, spi_rx_reg,
						spi_bc_reg, spi_tc_reg, spi_bcc_reg);
		buf += bufsize;

		if (code == 0xFFFF) {
			/* Wait for completion, part 2 */
			buf = backup_buf;
			if (wait_for_completion_cmd[1] & 1) {
				/* Still busy */
				continue;
			}
			/* Advance to the next code */
			buf = backup_buf + 2;
		}
	}
}
