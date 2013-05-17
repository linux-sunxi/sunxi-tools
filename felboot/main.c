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

#include <stdio.h>

__attribute__ ((section (".text.start"))) void _start(void)
{
	clock_init();
	gpio_init();
	uart_init();
	timer_init();
	s_init();
}

int gdata;

void dcache_enable(void)
{
}

void sunxi_wemac_initialize(void)
{
}

void preloader_console_init(void)
{
}

void hang(void)
{
	printf("Please reset the board!");
}

void udelay(unsigned long usec)
{
	__udelay(usec);
}

int sunxi_mmc_init(void)
{
	return -1;
}

int status_led_set(void)
{
	return -1;
}


#ifndef NO_PRINTF
int putchar(int ch)
{
	return uart_putc(ch);
}

int puts(const char *str)
{
	return uart_puts(str);
}

#else
int putchar(int ch)
{
	return -1;
}

int puts(const char *str)
{
	return -1;
}

int printf(const char *fmt, ...)
{
	return -1;
}

void uart_init(void)
{
}

#endif
