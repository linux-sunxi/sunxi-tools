/*
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * Early uart print for debugging.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SUNXI_EARLY_PRINT_H
#define _SUNXI_EARLY_PRINT_H

#define SUNXI_UART_BASE SUNXI_UART0_BASE

#define UART_RBR(n) (SUNXI_UART_BASE + (n)*0x400 + 0x0)    /* receive buffer register */
#define UART_THR(n) (SUNXI_UART_BASE + (n)*0x400 + 0x0)    /* transmit holding register */
#define UART_DLL(n) (SUNXI_UART_BASE + (n)*0x400 + 0x0)    /* divisor latch low register */

#define UART_DLH(n) (SUNXI_UART_BASE + (n)*0x400 + 0x4)    /* divisor latch high register */
#define UART_IER(n) (SUNXI_UART_BASE + (n)*0x400 + 0x4)    /* interrupt enable reigster */

#define UART_IIR(n) (SUNXI_UART_BASE + (n)*0x400 + 0x8)    /* interrupt identity register */
#define UART_FCR(n) (SUNXI_UART_BASE + (n)*0x400 + 0x8)    /* fifo control register */

#define UART_LCR(n) (SUNXI_UART_BASE + (n)*0x400 + 0xc)    /* line control register */

#define UART_LSR(n) (SUNXI_UART_BASE + (n)*0x400 + 0x14)   /* line status register */
#define UART_RBR(n) (SUNXI_UART_BASE + (n)*0x400 + 0x0)    /* receive buffer register */
#define UART_THR(n) (SUNXI_UART_BASE + (n)*0x400 + 0x0)    /* transmit holding register */
#define UART_DLL(n) (SUNXI_UART_BASE + (n)*0x400 + 0x0)    /* divisor latch low register */


#define BAUD_115200    (0xD) /* 24 * 1000 * 1000 / 16 / 115200 = 13 */
#define NO_PARITY      (0)
#define ONE_STOP_BIT   (0)
#define DAT_LEN_8_BITS (3)
#define LC_8_N_1          (NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

#ifndef __ASSEMBLY__
void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
#endif /* __ASSEMBLY__ */

#endif /* _SUNXI_EARLY_PRINT_H */
