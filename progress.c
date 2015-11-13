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
#include "progress.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

/* Less reliable than clock_gettime, but does not require linking with -lrt */
inline double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + (double)tv.tv_usec / 1000000.;
}

/* Private progress state variable */

typedef struct {
	progress_cb_t callback;
	size_t total;
	size_t done;
} progress_private_t;

static progress_private_t progress = {
	.callback = NULL,
};

/* 'External' API */

void progress_start(progress_cb_t callback, size_t expected_total)
{
	progress.callback = callback;
	progress.total = expected_total;
	progress.done = 0;
}

/* Update progress status, passing information to the callback function. */
void progress_update(size_t bytes_done)
{
	progress.done += bytes_done;
	if (progress.callback)
		progress.callback(progress.total, progress.done);
}

/* Callback function implementing a simple progress bar written to stdout */
void progress_bar(size_t total, size_t done)
{
	static const int WIDTH = 60; /* # of characters to use for progress bar */

	float ratio = total > 0 ? (float)done / total : 0;
	int i, pos = WIDTH * ratio;

	printf("\r%3.0f%% [", ratio * 100); /* current percentage */
	for (i = 0; i < pos; i++) putchar('=');
	for (i = pos; i < WIDTH; i++) putchar(' ');
	printf("] ");

	if (done >= total) putchar('\n'); /* output newline when complete */
	fflush(stdout);
}
