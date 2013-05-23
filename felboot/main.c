/*
 * (C) Copyright 2013 Henrik Nordstrom <henrik@henriknordstrom.net>
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

#include <common.h>
#include <version.h>

__attribute__ ((section (".text.start"))) void _start(void)
{
	s_init();
}

gd_t gdata __attribute__ ((section(".data")));

void dcache_enable(void)
{
}

void sunxi_wemac_initialize(void)
{
}

void preloader_console_init(void)
{
	uart_init();
	puts("\nU-Boot FEL " PLAIN_VERSION " (" U_BOOT_DATE " - " \
                        U_BOOT_TIME ")\n");
}

void hang(void)
{
	printf("Please reset the board!");
	while(1);
}

void udelay(unsigned long usec)
{
	__udelay(usec);
}

int sunxi_mmc_init(void)
{
	return -1;
}

void status_led_set(int led, int state)
{
	return;
}

#ifndef NO_PRINTF
void putchar(int ch)
{
	if (ch == '\n')
		uart_putc('\r');
	uart_putc(ch);
}

void puts(const char *str)
{
	while(*str)
		putchar(*str++);
}

#else
void putchar(int ch)
{
}

void puts(const char *str)
{
}

int printf(const char *fmt, ...)
{
	return -1;
}

void uart_init(void)
{
}

#endif
