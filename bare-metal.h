/*
 * Copyright (C) 2016  Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Partially based on the uart code from ar100-info
 *
 * (C) Copyright 2013 Stefan Kristiansson <stefan.kristiansson@saunalahti.fi>
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

/*
 * Partially based on the sunxi gpio code from U-Boot
 *
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * Based on earlier arch/arm/cpu/armv7/sunxi/gpio.c:
 *
 * (C) Copyright 2007-2011
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Tom Cubie <tangliang@allwinnertech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _BARE_METAL_H_
#define _BARE_METAL_H_

typedef unsigned int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

#ifndef NULL
#define NULL ((void*)0)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define set_wbit(addr, v)	(*((volatile unsigned long  *)(addr)) |= (unsigned long)(v))
#define readl(addr)		(*((volatile unsigned long  *)(addr)))
#define writel(v, addr)		(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

#define SUNXI_UART0_BASE	0x01C28000
#define SUNXI_PIO_BASE		0x01C20800
#define AW_CCM_BASE		0x01c20000
#define AW_SRAMCTRL_BASE	0x01c00000

#define A80_SRAMCTRL_BASE	0x00800000
#define A80_CCM_BASE		0x06000000
#define A80_PIO_BASE		0x06000800
#define A80_UART0_BASE		0x07000000

#define H6_UART0_BASE		0x05000000
#define H6_PIO_BASE		0x0300B000
#define H6_CCM_BASE		0x03001000
#define H6_SRAMCTRL_BASE	0x03000000

#define R329_UART0_BASE		0x02500000
#define R329_PIO_BASE		0x02000400
#define R329_CCM_BASE		0x02001000

#define V853_PIO_BASE		0x02000000

#define SUNIV_UART0_BASE	0x01c25000

#define SRAM_A1_ADDR_0		0x00000000
#define SRAM_A1_ADDR_10000	0x00010000
#define SRAM_A1_ADDR_20000	0x00020000
#define SRAM_A1_ADDR_100000	0x00100000

/*****************************************************************************
 * GPIO code, borrowed from U-Boot                                           *
 *****************************************************************************/

#define SUNXI_GPIO_A    0
#define SUNXI_GPIO_B    1
#define SUNXI_GPIO_C    2
#define SUNXI_GPIO_D    3
#define SUNXI_GPIO_E    4
#define SUNXI_GPIO_F    5
#define SUNXI_GPIO_G    6
#define SUNXI_GPIO_H    7
#define SUNXI_GPIO_I    8

#define GPIO_BANK(pin)		((pin) >> 5)
#define GPIO_NUM(pin)		((pin) & 0x1F)

#define GPIO_CFG_BASE(bank)	((u32 *)(pio_base + (bank) * pio_bank_size))
#define GPIO_CFG_INDEX(pin)	(((pin) & 0x1F) >> 3)
#define GPIO_CFG_OFFSET(pin)	((((pin) & 0x1F) & 0x7) << 2)

#define GPIO_PULL_BASE(bank)	((u32 *)(pio_base + (bank) * pio_bank_size + pio_pull_off))
#define GPIO_PULL_INDEX(pin)	(((pin) & 0x1f) >> 4)
#define GPIO_PULL_OFFSET(pin)	((((pin) & 0x1f) & 0xf) << 1)

#define GPIO_DAT_BASE(bank)	((u32 *)(pio_base + (bank) * pio_bank_size + pio_dat_off))

/* GPIO bank sizes */
#define SUNXI_GPIO_A_NR    (32)
#define SUNXI_GPIO_B_NR    (32)
#define SUNXI_GPIO_C_NR    (32)
#define SUNXI_GPIO_D_NR    (32)
#define SUNXI_GPIO_E_NR    (32)
#define SUNXI_GPIO_F_NR    (32)
#define SUNXI_GPIO_G_NR    (32)
#define SUNXI_GPIO_H_NR    (32)
#define SUNXI_GPIO_I_NR    (32)

#define SUNXI_GPIO_NEXT(__gpio) ((__gpio##_START) + (__gpio##_NR) + 0)

enum sunxi_gpio_number {
	SUNXI_GPIO_A_START = 0,
	SUNXI_GPIO_B_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_A),
	SUNXI_GPIO_C_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_B),
	SUNXI_GPIO_D_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_C),
	SUNXI_GPIO_E_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_D),
	SUNXI_GPIO_F_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_E),
	SUNXI_GPIO_G_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_F),
	SUNXI_GPIO_H_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_G),
	SUNXI_GPIO_I_START = SUNXI_GPIO_NEXT(SUNXI_GPIO_H),
};

/* SUNXI GPIO number definitions */
#define SUNXI_GPA(_nr)          (SUNXI_GPIO_A_START + (_nr))
#define SUNXI_GPB(_nr)          (SUNXI_GPIO_B_START + (_nr))
#define SUNXI_GPC(_nr)          (SUNXI_GPIO_C_START + (_nr))
#define SUNXI_GPD(_nr)          (SUNXI_GPIO_D_START + (_nr))
#define SUNXI_GPE(_nr)          (SUNXI_GPIO_E_START + (_nr))
#define SUNXI_GPF(_nr)          (SUNXI_GPIO_F_START + (_nr))
#define SUNXI_GPG(_nr)          (SUNXI_GPIO_G_START + (_nr))
#define SUNXI_GPH(_nr)          (SUNXI_GPIO_H_START + (_nr))
#define SUNXI_GPI(_nr)          (SUNXI_GPIO_I_START + (_nr))

/* GPIO pin function config */
#define MUX_GPIO_INPUT		0
#define MUX_GPIO_OUTPUT		1
#define MUX_2			2
#define MUX_3			3
#define MUX_4			4
#define MUX_5			5
#define MUX_6			6

/* GPIO pin pull-up/down config */
#define SUNXI_GPIO_PULL_DISABLE (0)
#define SUNXI_GPIO_PULL_UP      (1)
#define SUNXI_GPIO_PULL_DOWN    (2)

#define BIT(x)	(1U << (x))
#define FLAG_VAR0		0
#define FLAG_VAR1		BIT(0)
#define FLAG_UART_ON_PORTF	BIT(1)
#define FLAG_NEW_GPIO		BIT(2)
#define FLAG_NEW_CLOCK		BIT(3)
#define FLAG_UART_ON_APB1	BIT(4)
#define FLAG_A80_CLOCK		BIT(5)

#define FLAG_NCAT2		FLAG_NEW_GPIO | FLAG_NEW_CLOCK

enum { BOOT_DEVICE_UNK, BOOT_DEVICE_FEL, BOOT_DEVICE_MMC0, BOOT_DEVICE_SPI };

struct soc_info {
	u16	id;
	char	name[10];
	u8	flags;

	const struct {
		u32	base;
	} pio;

	const struct {
		u32	base;
	} ccu;

	const struct {
		u32	a1_base;
	} sram;

	const struct {
		u32	base;
		u16	pin_tx;
		u8	pinmux;
	} uart0;

	const struct {
		u16	pin_tms;
		u16	pin_tdi;
		u16	pin_tdo;
		u16	pin_tck;
		u8	pinmux;
	} jtag;
};

const struct soc_info *sunxi_detect_soc(void);
void gpio_init(const struct soc_info *soc);
void jtag_init(const struct soc_info *soc);
void uart0_init(const struct soc_info *soc);
void uart0_putc(char c);
void uart0_puts(const char *s);
int get_boot_device(const struct soc_info *soc);

#endif
