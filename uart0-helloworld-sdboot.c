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

typedef unsigned int u32;
typedef unsigned short int u16;
typedef unsigned char u8;

#ifndef NULL
#define NULL ((void*)0)
#endif

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

static const struct soc_info {
	u16	soc_id;
	char	soc_name[10];
	u32	pio_base;
	u32	ccu_base;
	u32	sram_a1_base;
	u32	uart0_base;
	u16	uart0_tx_pin;
	u8	uart0_pinmux;
	u8	flags;
} soc_table[] = {
	{ 0x1623, "A10", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPB(22), MUX_2 },
	{ 0x1625, "A10s", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPB(19), MUX_2, FLAG_VAR0 },
	{ 0x1625, "A13", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPB(19), MUX_2, FLAG_VAR1 | FLAG_UART_ON_PORTF },
	{ 0x1633, "A31/A31s", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPH(20), MUX_2, },
	{ 0x1639, "A80", A80_PIO_BASE, A80_CCM_BASE, SRAM_A1_ADDR_10000,
		A80_UART0_BASE, SUNXI_GPH(12), MUX_2, FLAG_A80_CLOCK },
	{ 0x1651, "A20", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPB(22), MUX_2 },
	{ 0x1663, "F1C100s", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNIV_UART0_BASE, SUNXI_GPE(0), MUX_5, FLAG_UART_ON_APB1 },
	{ 0x1673, "A83T", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPB(9), MUX_2 },
	{ 0x1689, "A64", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_10000,
		SUNXI_UART0_BASE, SUNXI_GPB(8), MUX_4 },
	{ 0x1680, "H2+", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPA(4), MUX_2, FLAG_VAR1 },
	{ 0x1680, "H3", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPA(4), MUX_2, FLAG_VAR0 },
	{ 0x1681, "V3s", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPB(8), MUX_3 },
	{ 0x1701, "R40", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_0,
		SUNXI_UART0_BASE, SUNXI_GPB(22), MUX_2 },
	{ 0x1708, "T7", H6_PIO_BASE, H6_CCM_BASE, SRAM_A1_ADDR_20000,
		H6_UART0_BASE, SUNXI_GPB(8), MUX_4, FLAG_NEW_CLOCK },
	{ 0x1718, "H5", SUNXI_PIO_BASE, AW_CCM_BASE, SRAM_A1_ADDR_10000,
		SUNXI_UART0_BASE, SUNXI_GPA(4), MUX_2 },
	{ 0x1719, "A63", H6_PIO_BASE, H6_CCM_BASE, SRAM_A1_ADDR_10000,
		H6_UART0_BASE, SUNXI_GPB(9), MUX_4, FLAG_NEW_CLOCK },
	{ 0x1721, "V5", H6_PIO_BASE, H6_CCM_BASE, SRAM_A1_ADDR_20000,
		H6_UART0_BASE, SUNXI_GPB(9), MUX_2, FLAG_NEW_CLOCK },
	{ 0x1728, "H6", H6_PIO_BASE, H6_CCM_BASE, SRAM_A1_ADDR_20000,
		H6_UART0_BASE, SUNXI_GPH(0), MUX_2, FLAG_NEW_CLOCK },
	{ 0x1817, "V831", H6_PIO_BASE, H6_CCM_BASE, SRAM_A1_ADDR_20000,
		H6_UART0_BASE, SUNXI_GPH(9), MUX_5, FLAG_NEW_CLOCK },
	{ 0x1823, "H616", H6_PIO_BASE, H6_CCM_BASE, SRAM_A1_ADDR_20000,
		H6_UART0_BASE, SUNXI_GPH(0), MUX_2, FLAG_NEW_CLOCK },
	{ 0x1851, "R329", R329_PIO_BASE, R329_CCM_BASE, SRAM_A1_ADDR_100000,
		R329_UART0_BASE, SUNXI_GPB(4), MUX_2, FLAG_NCAT2 },
	{ 0x1855, "A133", H6_PIO_BASE, H6_CCM_BASE, SRAM_A1_ADDR_20000,
		H6_UART0_BASE, SUNXI_GPB(9), MUX_2, FLAG_NEW_CLOCK },
	{ 0x1859, "R528", V853_PIO_BASE, R329_CCM_BASE, SRAM_A1_ADDR_20000,
		R329_UART0_BASE, SUNXI_GPE(2), MUX_6, FLAG_NCAT2 },
	{ 0x1886, "V853", V853_PIO_BASE, R329_CCM_BASE, SRAM_A1_ADDR_20000,
		R329_UART0_BASE, SUNXI_GPH(9), MUX_5, FLAG_NCAT2 },
	{ 0x1890, "A523", V853_PIO_BASE, R329_CCM_BASE, SRAM_A1_ADDR_20000,
		R329_UART0_BASE, SUNXI_GPB(9), MUX_2, FLAG_NCAT2 },
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static const struct soc_info *find_soc_info(int soc_id, int variant)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(soc_table); i++) {
		if (soc_table[i].soc_id != soc_id)
			continue;

		if (variant == (soc_table[i].flags & FLAG_VAR1))
			return &soc_table[i];
	}

	return NULL;
}

static u32 pio_base;
static u32 pio_bank_size, pio_dat_off, pio_pull_off;

static int sunxi_gpio_set_cfgpin(u32 pin, u32 val)
{
	u32 cfg;
	u32 bank = GPIO_BANK(pin);
	u32 index = GPIO_CFG_INDEX(pin);
	u32 offset = GPIO_CFG_OFFSET(pin);
	u32 *addr = GPIO_CFG_BASE(bank) + index;
	cfg = readl(addr);
	cfg &= ~(0xf << offset);
	cfg |= val << offset;
	writel(cfg, addr);
	return 0;
}

static int sunxi_gpio_set_pull(u32 pin, u32 val)
{
	u32 cfg;
	u32 bank = GPIO_BANK(pin);
	u32 index = GPIO_PULL_INDEX(pin);
	u32 offset = GPIO_PULL_OFFSET(pin);
	u32 *addr = GPIO_PULL_BASE(bank) + index;
	cfg = readl(addr);
	cfg &= ~(0x3 << offset);
	cfg |= val << offset;
	writel(cfg, addr);
	return 0;
}

/*****************************************************************************
 * Nearly all the Allwinner SoCs are using the same VER_REG register for     *
 * runtime SoC type identification. For additional details see:              *
 *                                                                           *
 *    https://linux-sunxi.org/SRAM_Controller_Register_Guide                 *
 *                                                                           *
 * Allwinner A80 is an oddball and has a non-standard address of the VER_REG *
 *                                                                           *
 * Allwinner A10s and A13 are using the same SoC type id, but they can be    *
 * differentiated using a certain part of the SID register.                  *
 *                                                                           *
 * Allwinner H6 has its memory map totally reworked, but the SRAM controller *
 * remains similar; the base of it is moved to 0x03000000.                   *
 *****************************************************************************/

#define VER_REG			(AW_SRAMCTRL_BASE + 0x24)
#define H6_VER_REG		(H6_SRAMCTRL_BASE + 0x24)
#define A80_VER_REG		(A80_SRAMCTRL_BASE + 0x24)
#define SUN4I_SID_BASE		0x01C23800
#define SUN8I_SID_BASE		0x01C14000

#define SID_PRCTL	0x40	/* SID program/read control register */
#define SID_RDKEY	0x60	/* SID read key value register */

#define SID_OP_LOCK	0xAC	/* Efuse operation lock value */
#define SID_READ_START	(1 << 1) /* bit 1 of SID_PRCTL, Software Read Start */

static u32 sid_read_key(u32 sid_base, u32 offset)
{
	u32 reg_val;

	reg_val = (offset & 0x1FF) << 16; /* PG_INDEX value */
	reg_val |= (SID_OP_LOCK << 8) | SID_READ_START; /* request read access */
	writel(reg_val, sid_base + SID_PRCTL);

	while (readl(sid_base + SID_PRCTL) & SID_READ_START) ; /* wait while busy */

	reg_val = readl(sid_base + SID_RDKEY); /* read SID key value */
	writel(0, sid_base + SID_PRCTL); /* clear SID_PRCTL (removing SID_OP_LOCK) */

	return reg_val;
}

/* A10s and A13 share the same ID, so we need a little more effort on those */
static int sunxi_get_sun5i_variant(void)
{
	if ((readl(SUN4I_SID_BASE + 8) & 0xf000) != 0x7000)
		return FLAG_VAR1;

	return FLAG_VAR0;
}

/* H2+ and H3 share the same ID, we can differentiate them by SID_RKEY0 */
static int sunxi_get_h3_variant(void)
{
	u32 sid0 = sid_read_key(SUN8I_SID_BASE, 0);

	/* H2+ uses those SID IDs */
	if ((sid0 & 0xff) == 0x42 || (sid0 & 0xff) == 0x83)
		return FLAG_VAR1;

	/*
	 * Note: according to Allwinner sources, H3 is expected
	 * to show up as 0x00, 0x81 or ("H3D") 0x58 here.
	 */

	return FLAG_VAR0;
}

static const struct soc_info *sunxi_detect_soc(void)
{
	int variant = 0;
	u32 soc_id;
	u32 midr;
	u32 reg;

	asm volatile("mrc p15, 0, %0, c0, c0, 0" : "=r" (midr));
	if (((midr >> 4) & 0xFFF) == 0xc08) { /* ARM Cortex-A8: A10/A10s/A13 */
		reg = VER_REG;
	} else if ((readl(0x03021008) & 0xfff) == 0x43b) {// GICD_IIDR @ NCAT
		reg = H6_VER_REG;
	} else if ((readl(0x01c81008) & 0xfff) == 0x43b) {// GICD_IIDR @ legacy
		reg = VER_REG;
	} else if ((readl(0x01c41008) & 0xfff) == 0x43b) {// GICD_IIDR @ A80
		reg = A80_VER_REG;
	} else if ((readl(0x03400008) & 0xfff) == 0x43b) {// GICD_IIDR @ GIC-600
		reg = H6_VER_REG;
	} else {
		while (1);				// unknown
	}

	set_wbit(reg, 1U << 15);
	soc_id = readl(reg) >> 16;

	switch(soc_id) {
	case 0x1680:
		variant = sunxi_get_h3_variant();
		break;
	case 0x1625:
		variant = sunxi_get_sun5i_variant();
		break;
	}

	return find_soc_info(soc_id, variant);
}

/*****************************************************************************
 * UART is mostly the same on A10/A13/A20/A31/H3/A64, except that newer SoCs *
 * have changed the APB numbering scheme (A10/A13/A20 used to have APB0 and  *
 * APB1 names, but newer SoCs just have renamed them into APB1 and APB2).    *
 * The constants below are using the new APB numbering convention.           *
 * Also the newer SoCs have introduced the APB2_RESET register, but writing  *
 * to it effectively goes nowhere on older SoCs and is harmless.             *
 *****************************************************************************/

#define CONFIG_CONS_INDEX	1

static void clock_init_uart(const struct soc_info *soc)
{
	if (soc->flags & FLAG_NEW_CLOCK) {
		set_wbit(soc->ccu_base + 0x90c,
			 0x10001 << (CONFIG_CONS_INDEX - 1));
	} else {
		int bit = 16 + CONFIG_CONS_INDEX - 1;
		int gate_ofs = 0x06c;
		int reset_ofs = 0x2d8;

		if (soc->flags & FLAG_UART_ON_APB1) {
			bit = 20 + CONFIG_CONS_INDEX - 1;
			gate_ofs = 0x068;
			reset_ofs = 0x2d0;
		} else if (soc->flags & FLAG_A80_CLOCK) {
			gate_ofs = 0x594;
			reset_ofs = 0x5b4;
		}
		/* Open the clock gate for UART0 */
		set_wbit(soc->ccu_base + gate_ofs, 1U << bit);
		/* Deassert UART0 reset (not really needed on old SoCs) */
		set_wbit(soc->ccu_base + reset_ofs, 1U << bit);
	}
}

/*****************************************************************************
 * UART0 pins muxing is different for different SoC variants.                *
 * Allwinner A13 is a bit special, because there are no dedicated UART0 pins *
 * and they are shared with MMC0.                                            *
 *****************************************************************************/

static void gpio_init(const struct soc_info *soc)
{
	pio_base = soc->pio_base;

	if (soc->flags & FLAG_NEW_GPIO) {
		/* GPIO V2 */
		pio_bank_size = 0x30;
		pio_dat_off = 0x10;
		pio_pull_off = 0x24;
	} else {
		/* GPIO V1 */
		pio_bank_size = 0x24;
		pio_dat_off = 0x10;
		pio_pull_off = 0x1c;
	}

	if (soc->flags & FLAG_UART_ON_PORTF) {
		/* Disable normal UART0 pins to avoid conflict */
		sunxi_gpio_set_cfgpin(soc->uart0_tx_pin, MUX_GPIO_INPUT);
		sunxi_gpio_set_cfgpin(soc->uart0_tx_pin + 1, MUX_GPIO_INPUT);

		/* Use SD breakout board to access UART0 on MMC0 pins */
		sunxi_gpio_set_cfgpin(SUNXI_GPF(2), soc->uart0_pinmux);
		sunxi_gpio_set_cfgpin(SUNXI_GPF(4), soc->uart0_pinmux);
		sunxi_gpio_set_pull(SUNXI_GPF(4), SUNXI_GPIO_PULL_UP);
	} else {
		sunxi_gpio_set_cfgpin(soc->uart0_tx_pin, soc->uart0_pinmux);
		sunxi_gpio_set_cfgpin(soc->uart0_tx_pin + 1, soc->uart0_pinmux);
		sunxi_gpio_set_pull(soc->uart0_tx_pin + 1, SUNXI_GPIO_PULL_UP);
	}
}

/*****************************************************************************/

static u32 uart0_base;

#define UART0_RBR (uart0_base + 0x0)    /* receive buffer register */
#define UART0_THR (uart0_base + 0x0)    /* transmit holding register */
#define UART0_DLL (uart0_base + 0x0)    /* divisor latch low register */

#define UART0_DLH (uart0_base + 0x4)    /* divisor latch high register */
#define UART0_IER (uart0_base + 0x4)    /* interrupt enable reigster */

#define UART0_IIR (uart0_base + 0x8)    /* interrupt identity register */
#define UART0_FCR (uart0_base + 0x8)    /* fifo control register */

#define UART0_LCR (uart0_base + 0xc)    /* line control register */

#define UART0_LSR (uart0_base + 0x14)   /* line status register */

#define BAUD_115200		13	/* 24 * 1000 * 1000 / 16 / 115200 */
/* The BROM sets the CPU clock to 204MHz, AHB=CPU/2, APB=AHB/2 => 51 MHz */
#define BAUD_115200_SUNIV 	28	/* 51 * 1000 * 1000 / 16 / 115200 */
#define NO_PARITY      (0)
#define ONE_STOP_BIT   (0)
#define DAT_LEN_8_BITS (3)
#define LC_8_N_1       (NO_PARITY << 3 | ONE_STOP_BIT << 2 | DAT_LEN_8_BITS)

static void uart0_init(const struct soc_info *soc)
{
	clock_init_uart(soc);

	uart0_base = soc->uart0_base;

	/* select dll dlh */
	writel(0x80, UART0_LCR);
	/* set baudrate */
	writel(0, UART0_DLH);
	if (soc->soc_id == 0x1663)
		writel(BAUD_115200_SUNIV, UART0_DLL);
	else
		writel(BAUD_115200, UART0_DLL);
	/* set line control */
	writel(LC_8_N_1, UART0_LCR);
}

static void uart0_putc(char c)
{
	while (!(readl(UART0_LSR) & (1 << 6))) {}
	writel(c, UART0_THR);
}

static void uart0_puts(const char *s)
{
	while (*s) {
		if (*s == '\n')
			uart0_putc('\r');
		uart0_putc(*s++);
	}
}

/*****************************************************************************/

/* A workaround for https://patchwork.ozlabs.org/patch/622173 */
void __attribute__((section(".start"))) __attribute__((naked)) start(void)
{
	asm volatile("b     main             \n"
		     ".long 0xffffffff       \n"
		     ".long 0xffffffff       \n"
		     ".long 0xffffffff       \n");
}

enum { BOOT_DEVICE_UNK, BOOT_DEVICE_FEL, BOOT_DEVICE_MMC0, BOOT_DEVICE_SPI };

static int get_boot_device(const struct soc_info *soc)
{
	u32 *spl_signature = (void *)soc->sram_a1_base + 0x4;

	/* Check the eGON.BT0 magic in the SPL header */
	if (spl_signature[0] != 0x4E4F4765 || spl_signature[1] != 0x3054422E)
		return BOOT_DEVICE_FEL;

	u32 boot_dev = spl_signature[9] & 0xFF; /* offset into SPL = 0x28 */
	if (boot_dev == 0)
		return BOOT_DEVICE_MMC0;
	if (boot_dev == 3)
		return BOOT_DEVICE_SPI;

	return BOOT_DEVICE_UNK;
}

int main(void)
{
	const struct soc_info *soc = sunxi_detect_soc();

	if (soc == NULL)
		return 0;

	gpio_init(soc);
	uart0_init(soc);

	uart0_puts("\nHello from Allwinner ");
	uart0_puts(soc->soc_name);
	uart0_puts("!\n");

	switch (get_boot_device(soc)) {
	case BOOT_DEVICE_FEL:
		uart0_puts("Returning back to FEL.\n");
		return 0;
	case BOOT_DEVICE_MMC0:
		uart0_puts("Booted from MMC0, entering an infinite loop.\n");
		while (1) {}
	case BOOT_DEVICE_SPI:
		uart0_puts("Booted from SPI0, entering an infinite loop.\n");
		while (1) {}
	default:
		uart0_puts("Booted from unknown media, entering an infinite loop.\n");
		while (1) {}
	};

	return 0;
}
