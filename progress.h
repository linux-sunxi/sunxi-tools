/*
 * Copyright (C) 2015  Bernhard Nortmann <bernhard.nortmann@web.de>
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
#ifndef _SUNXI_TOOLS_PROGRESS_H
#define _SUNXI_TOOLS_PROGRESS_H

#include <stddef.h>

/* function pointer type for a progress callback / notification */
typedef void (*progress_cb_t)(size_t total, size_t done);

/* conversion helper macros */
#define kilo(value)	((double)(value) / 1000.) /* SI prefix "k" */
#define kibi(value)	((double)(value) / 1024.) /* binary prefix "Ki", "K" */

double gettime(void);
double rate(size_t transferred, double elapsed);
double estimate(size_t remaining, double rate);

void progress_start(progress_cb_t callback, size_t expected_total);
void progress_update(size_t bytes_done);

/* progress callback implementations for various display styles */
void progress_bar(size_t total, size_t done);
void progress_gauge(size_t total, size_t done);
void progress_gauge_xxx(size_t total, size_t done);

#endif /* _SUNXI_TOOLS_PROGRESS_H */
