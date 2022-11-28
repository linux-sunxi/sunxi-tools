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

#define set_wbit(addr, v)	(*((volatile unsigned long  *)(addr)) |= (unsigned long)(v))
#define readl(addr)		(*((volatile unsigned long  *)(addr)))
#define writel(v, addr)		(*((volatile unsigned long  *)(addr)) = (unsigned long)(v))

#define SUNXI_UART0_BASE	0x01C28000
#define SUNXI_PIO_BASE		0x01C20800
#define AW_CCM_BASE		0x01c20000
#define AW_SRAMCTRL_BASE	0x01c00000

#define H6_UART0_BASE		0x05000000
#define H6_PIO_BASE		0x0300B000
#define H6_CCM_BASE		0x03001000
#define H6_SRAMCTRL_BASE	0x03000000

#define R329_UART0_BASE		0x02500000
#define R329_PIO_BASE		0x02000400
#define R329_CCM_BASE		0x02001000

#define V853_PIO_BASE		0x02000000

#define SUNIV_UART0_BASE	0x01c25000
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
#define SUNXI_GPIO_INPUT        (0)
#define SUNXI_GPIO_OUTPUT       (1)
#define SUNIV_GPE_UART0         (5)
#define SUN4I_GPB_UART0         (2)
#define SUN5I_GPB_UART0         (2)
#define SUN6I_GPH_UART0         (2)
#define SUN8I_H3_GPA_UART0      (2)
#define SUN8I_R528_GPE_UART0	(6)
#define SUN8I_V3S_GPB_UART0	(3)
#define SUN8I_V5_GPB_UART0	(2)
#define SUN8I_V831_GPH_UART0	(5)
#define SUN8I_V853_GPH_UART0	(5)
#define SUN50I_H5_GPA_UART0     (2)
#define SUN50I_H6_GPH_UART0	(2)
#define SUN50I_H616_GPH_UART0	(2)
#define SUN50I_R329_GPB_UART0   (2)
#define SUN50I_A64_GPB_UART0    (4)
#define SUNXI_GPF_UART0         (4)

/* GPIO pin pull-up/down config */
#define SUNXI_GPIO_PULL_DISABLE (0)
#define SUNXI_GPIO_PULL_UP      (1)
#define SUNXI_GPIO_PULL_DOWN    (2)

static u32 pio_base;
static u32 pio_bank_size, pio_dat_off, pio_pull_off;

int sunxi_gpio_set_cfgpin(u32 pin, u32 val)
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

int sunxi_gpio_set_pull(u32 pin, u32 val)
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

int sunxi_gpio_output(u32 pin, u32 val)
{
	u32 dat;
	u32 bank = GPIO_BANK(pin);
	u32 num = GPIO_NUM(pin);
	u32 *addr = GPIO_DAT_BASE(bank);
	dat = readl(addr);
	if(val)
		dat |= 1 << num;
	else
		dat &= ~(1 << num);
	writel(dat, addr);
	return 0;
}

int sunxi_gpio_input(u32 pin)
{
	u32 dat;
	u32 bank = GPIO_BANK(pin);
	u32 num = GPIO_NUM(pin);
	u32 *addr = GPIO_DAT_BASE(bank);
	dat = readl(addr);
	dat >>= num;
	return (dat & 0x1);
}

int gpio_direction_input(unsigned gpio)
{
	sunxi_gpio_set_cfgpin(gpio, SUNXI_GPIO_INPUT);
	return sunxi_gpio_input(gpio);
}

int gpio_direction_output(unsigned gpio, int value)
{
	sunxi_gpio_set_cfgpin(gpio, SUNXI_GPIO_OUTPUT);
	return sunxi_gpio_output(gpio, value);
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
#define SUN4I_SID_BASE		0x01C23800
#define SUN8I_SID_BASE		0x01C14000

#define SID_PRCTL	0x40	/* SID program/read control register */
#define SID_RDKEY	0x60	/* SID read key value register */

#define SID_OP_LOCK	0xAC	/* Efuse operation lock value */
#define SID_READ_START	(1 << 1) /* bit 1 of SID_PRCTL, Software Read Start */

u32 sid_read_key(u32 sid_base, u32 offset)
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

static u32 soc_id;

void soc_detection_init(void)
{
	u32 midr;
	asm volatile("mrc p15, 0, %0, c0, c0, 0" : "=r" (midr));

	if (((midr >> 4) & 0xFFF) == 0xC0F) {
		soc_id = 0x1639; /* ARM Cortex-A15, so likely Allwinner A80 */
	} else {
		u32 reg;

		/*
		 * This register is GICD_IIDR on H6, but unmapped according to
		 * other known SoCs' user manuals.
		 */
		reg = readl(0x03021008);

		if ((reg & 0xfff) == 0x43b) /* Found GICv2 here, so it's a H6 */
			reg = H6_VER_REG;
		else
			reg = VER_REG;

		set_wbit(reg, 1 << 15);
		soc_id = readl(reg) >> 16;
	}
}

/* Most SoCs can reliably be distinguished by simply checking their ID value */

#define soc_is_a10()	(soc_id == 0x1623)
#define soc_is_a20()	(soc_id == 0x1651)
#define soc_is_a31()	(soc_id == 0x1633)
#define soc_is_a80()	(soc_id == 0x1639)
#define soc_is_a64()	(soc_id == 0x1689)
#define soc_is_h5()	(soc_id == 0x1718)
#define soc_is_a63()	(soc_id == 0x1719)
#define soc_is_h6()	(soc_id == 0x1728)
#define soc_is_h616()	(soc_id == 0x1823)
#define soc_is_r329()	(soc_id == 0x1851)
#define soc_is_r40()	(soc_id == 0x1701)
#define soc_is_v3s()	(soc_id == 0x1681)
#define soc_is_v831()	(soc_id == 0x1817)
#define soc_is_v853()	(soc_id == 0x1886)
#define soc_is_r528()	(soc_id == 0x1859)
#define soc_is_v5()	(soc_id == 0x1721)
#define soc_is_suniv()	(soc_id == 0x1663)

/* A10s and A13 share the same ID, so we need a little more effort on those */

int soc_is_a10s(void)
{
	return soc_id == 0x1625 &&
	       (readl(SUN4I_SID_BASE + 8) & 0xf000) == 0x7000;
}

int soc_is_a13(void)
{
	return soc_id == 0x1625 &&
	       (readl(SUN4I_SID_BASE + 8) & 0xf000) != 0x7000;
}

/* H2+ and H3 share the same ID, we can differentiate them by SID_RKEY0 */

int soc_is_h2_plus(void)
{
	if (soc_id != 0x1680) return 0;

	u32 sid0 = sid_read_key(SUN8I_SID_BASE, 0);
	return (sid0 & 0xff) == 0x42 || (sid0 & 0xff) == 0x83;
}

int soc_is_h3(void)
{
	if (soc_id != 0x1680) return 0;

	u32 sid0 = sid_read_key(SUN8I_SID_BASE, 0);
	/*
	 * Note: according to Allwinner sources, H3 is expected
	 * to show up as 0x00, 0x81 or ("H3D") 0x58 here.
	 */
	return (sid0 & 0xff) != 0x42 && (sid0 & 0xff) != 0x83;
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
#define APB2_CFG		(AW_CCM_BASE + 0x058)
#define APB1_GATE		(AW_CCM_BASE + 0x068)
#define APB2_GATE		(AW_CCM_BASE + 0x06C)
#define APB1_RESET		(AW_CCM_BASE + 0x2D0)
#define APB2_RESET		(AW_CCM_BASE + 0x2D8)
#define APB2_GATE_UART_SHIFT	(16)
#define APB1_GATE_UART_SHIFT	20
#define APB2_RESET_UART_SHIFT	(16)
#define APB1_RESET_UART_SHIFT	20

#define H6_UART_GATE_RESET	(H6_CCM_BASE + 0x90C)
#define R329_UART_GATE_RESET	(R329_CCM_BASE + 0x90C)
#define H6_UART_GATE_SHIFT	(0)
#define H6_UART_RESET_SHIFT	(16)

void clock_init_uart_legacy(void)
{
	/* Open the clock gate for UART0 */
	set_wbit(APB2_GATE, 1 << (APB2_GATE_UART_SHIFT + CONFIG_CONS_INDEX - 1));
	/* Deassert UART0 reset (only needed on A31/A64/H3) */
	set_wbit(APB2_RESET, 1 << (APB2_RESET_UART_SHIFT + CONFIG_CONS_INDEX - 1));
}

void clock_init_uart_suniv(void)
{
	/* open the clock for uart */
	set_wbit(APB1_GATE,
		 1U << (APB1_GATE_UART_SHIFT + CONFIG_CONS_INDEX - 1));

	/* deassert uart reset */
	set_wbit(APB1_RESET,
		 1U << (APB1_RESET_UART_SHIFT + CONFIG_CONS_INDEX - 1));
}

void clock_init_uart_h6(void)
{
	/* Open the clock gate for UART0 */
	set_wbit(H6_UART_GATE_RESET, 1 << (H6_UART_GATE_SHIFT + CONFIG_CONS_INDEX - 1));
	/* Deassert UART0 reset */
	set_wbit(H6_UART_GATE_RESET, 1 << (H6_UART_RESET_SHIFT + CONFIG_CONS_INDEX - 1));
}

void clock_init_uart_r329(void)
{
	/* Open the clock gate for UART0 */
	set_wbit(R329_UART_GATE_RESET, 1 << (H6_UART_GATE_SHIFT + CONFIG_CONS_INDEX - 1));
	/* Deassert UART0 reset */
	set_wbit(R329_UART_GATE_RESET, 1 << (H6_UART_RESET_SHIFT + CONFIG_CONS_INDEX - 1));
}

void clock_init_uart(void)
{
	if (soc_is_h6() || soc_is_v831() || soc_is_h616() || soc_is_v5() ||
	    soc_is_a63())
		clock_init_uart_h6();
	else if (soc_is_r329() || soc_is_v853() || soc_is_r528())
		clock_init_uart_r329();
	else if (soc_is_suniv())
		clock_init_uart_suniv();
	else
		clock_init_uart_legacy();
}

/*****************************************************************************
 * UART0 pins muxing is different for different SoC variants.                *
 * Allwinner A13 is a bit special, because there are no dedicated UART0 pins *
 * and they are shared with MMC0.                                            *
 *****************************************************************************/

void gpio_init(void)
{
	if (soc_is_v853() || soc_is_r528()) {
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

	if (soc_is_a10() || soc_is_a20() || soc_is_r40()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPB(22), SUN4I_GPB_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPB(23), SUN4I_GPB_UART0);
		sunxi_gpio_set_pull(SUNXI_GPB(23), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_a10s()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPB(19), SUN5I_GPB_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPB(20), SUN5I_GPB_UART0);
		sunxi_gpio_set_pull(SUNXI_GPB(20), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_a13()) {
		/* Disable PB19/PB20 as UART0 to avoid conflict */
		gpio_direction_input(SUNXI_GPB(19));
		gpio_direction_input(SUNXI_GPB(20));
		/* Use SD breakout board to access UART0 on MMC0 pins */
		sunxi_gpio_set_cfgpin(SUNXI_GPF(2), SUNXI_GPF_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPF(4), SUNXI_GPF_UART0);
		sunxi_gpio_set_pull(SUNXI_GPF(4), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_a31()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPH(20), SUN6I_GPH_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPH(21), SUN6I_GPH_UART0);
		sunxi_gpio_set_pull(SUNXI_GPH(21), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_a64()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPB(8), SUN50I_A64_GPB_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPB(9), SUN50I_A64_GPB_UART0);
		sunxi_gpio_set_pull(SUNXI_GPB(9), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_h3() || soc_is_h2_plus()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPA(4), SUN8I_H3_GPA_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPA(5), SUN8I_H3_GPA_UART0);
		sunxi_gpio_set_pull(SUNXI_GPA(5), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_h5()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPA(4), SUN50I_H5_GPA_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPA(5), SUN50I_H5_GPA_UART0);
		sunxi_gpio_set_pull(SUNXI_GPA(5), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_a63()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPB(9), SUN50I_A64_GPB_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPB(10), SUN50I_A64_GPB_UART0);
		sunxi_gpio_set_pull(SUNXI_GPB(10), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_h6()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPH(0), SUN50I_H6_GPH_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPH(1), SUN50I_H6_GPH_UART0);
		sunxi_gpio_set_pull(SUNXI_GPH(1), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_h616()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPH(0), SUN50I_H616_GPH_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPH(1), SUN50I_H616_GPH_UART0);
		sunxi_gpio_set_pull(SUNXI_GPH(1), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_r329()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPB(4), SUN50I_R329_GPB_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPB(5), SUN50I_R329_GPB_UART0);
		sunxi_gpio_set_pull(SUNXI_GPB(5), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_v3s()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPB(8), SUN8I_V3S_GPB_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPB(9), SUN8I_V3S_GPB_UART0);
		sunxi_gpio_set_pull(SUNXI_GPB(9), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_v831()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPH(9), SUN8I_V831_GPH_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPH(10), SUN8I_V831_GPH_UART0);
		sunxi_gpio_set_pull(SUNXI_GPH(10), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_v853()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPH(9), SUN8I_V853_GPH_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPH(10), SUN8I_V853_GPH_UART0);
		sunxi_gpio_set_pull(SUNXI_GPH(10), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_r528()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPE(2), SUN8I_R528_GPE_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPE(3), SUN8I_R528_GPE_UART0);
		sunxi_gpio_set_pull(SUNXI_GPE(3), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_v5()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPB(9), SUN8I_V5_GPB_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPB(10), SUN8I_V5_GPB_UART0);
		sunxi_gpio_set_pull(SUNXI_GPB(10), SUNXI_GPIO_PULL_UP);
	} else if (soc_is_suniv()) {
		sunxi_gpio_set_cfgpin(SUNXI_GPE(0), SUNIV_GPE_UART0);
		sunxi_gpio_set_cfgpin(SUNXI_GPE(1), SUNIV_GPE_UART0);
		sunxi_gpio_set_pull(SUNXI_GPE(1), SUNXI_GPIO_PULL_UP);
	} else {
		/* Unknown SoC */
		while (1) {}
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

void uart0_init(void)
{
	clock_init_uart();

	/* select dll dlh */
	writel(0x80, UART0_LCR);
	/* set baudrate */
	writel(0, UART0_DLH);
	if (soc_is_suniv())
		writel(BAUD_115200_SUNIV, UART0_DLL);
	else
		writel(BAUD_115200, UART0_DLL);
	/* set line control */
	writel(LC_8_N_1, UART0_LCR);
}

void uart0_putc(char c)
{
	while (!(readl(UART0_LSR) & (1 << 6))) {}
	writel(c, UART0_THR);
}

void uart0_puts(const char *s)
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

int get_boot_device(void)
{
	u32 *spl_signature = (void *)0x4;
	if (soc_is_a64() || soc_is_a80() || soc_is_h5())
		spl_signature = (void *)0x10004;
	if (soc_is_h6() || soc_is_v831() || soc_is_h616() || soc_is_v853() ||
	    soc_is_a63())
		spl_signature = (void *)0x20004;
	if (soc_is_r329())
		spl_signature = (void *)0x100004;

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

void bases_init(void)
{
	if (soc_is_h6() || soc_is_v831() || soc_is_h616() || soc_is_v5() ||
	    soc_is_a63()) {
		pio_base = H6_PIO_BASE;
		uart0_base = H6_UART0_BASE;
	} else if (soc_is_r329()) {
		pio_base = R329_PIO_BASE;
		uart0_base = R329_UART0_BASE;
	} else if (soc_is_v853() || soc_is_r528()) {
		pio_base = V853_PIO_BASE;
		uart0_base = R329_UART0_BASE;
	} else if (soc_is_suniv()) {
		pio_base = SUNXI_PIO_BASE;
		uart0_base = SUNIV_UART0_BASE;
	} else {
		pio_base = SUNXI_PIO_BASE;
		uart0_base = SUNXI_UART0_BASE;
	}
}

int main(void)
{
	soc_detection_init();
	bases_init();
	gpio_init();
	uart0_init();

	uart0_puts("\nHello from ");
	if (soc_is_a10())
		uart0_puts("Allwinner A10!\n");
	else if (soc_is_a10s())
		uart0_puts("Allwinner A10s!\n");
	else if (soc_is_a13())
		uart0_puts("Allwinner A13!\n");
	else if (soc_is_a20())
		uart0_puts("Allwinner A20!\n");
	else if (soc_is_a31())
		uart0_puts("Allwinner A31/A31s!\n");
	else if (soc_is_a64())
		uart0_puts("Allwinner A64!\n");
	else if (soc_is_h2_plus())
		uart0_puts("Allwinner H2+!\n");
	else if (soc_is_h3())
		uart0_puts("Allwinner H3!\n");
	else if (soc_is_h5())
		uart0_puts("Allwinner H5!\n");
	else if (soc_is_a63())
		uart0_puts("Allwinner A63!\n");
	else if (soc_is_h6())
		uart0_puts("Allwinner H6!\n");
	else if (soc_is_h616())
		uart0_puts("Allwinner H616!\n");
	else if (soc_is_r329())
		uart0_puts("Allwinner R329!\n");
	else if (soc_is_r40())
		uart0_puts("Allwinner R40!\n");
	else if (soc_is_v3s())
		uart0_puts("Allwinner V3s!\n");
	else if (soc_is_v831())
		uart0_puts("Allwinner V831!\n");
	else if (soc_is_v853())
		uart0_puts("Allwinner V853!\n");
	else if (soc_is_r528())
		uart0_puts("Allwinner R528/T113!\n");
	else if (soc_is_v5())
		uart0_puts("Allwinner V5!\n");
	else if (soc_is_suniv())
		uart0_puts("Allwinner F1C100s!\n");
	else
		uart0_puts("unknown Allwinner SoC!\n");

	switch (get_boot_device()) {
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
