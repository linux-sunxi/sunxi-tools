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

#include "bare-metal.h"

int main(void)
{
	const struct soc_info *soc = sunxi_detect_soc();

	if (soc == NULL)
		return 0;

	gpio_init(soc);
	uart0_init(soc);

	uart0_puts("\nHello from Allwinner ");
	uart0_puts(soc->name);
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
