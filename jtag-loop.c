/*
 * (C) Copyright 2012 Jens Andersen <jens.andersen@gmail.com>
 * (C) Copyright 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "bare-metal.h"

int main(void)
{
	const struct soc_info *soc = sunxi_detect_soc();

	if (soc == NULL)
		return 0;

	gpio_init(soc);
	jtag_init(soc);
	uart0_init(soc);

	uart0_puts("\nJTAG loop for Allwinner ");
	uart0_puts(soc->name);
	uart0_puts("!\n");

	if (!soc->jtag.pinmux) {
		uart0_puts("No JTAG pins defined for this SoC!\n");
		uart0_puts("Returning back to FEL.\n");
		return 0;
	}

	uart0_puts("JTAG pinmux set, entering loop.\n");

	while(1);

	return 0;
}
