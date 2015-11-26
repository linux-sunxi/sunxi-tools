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

/* Calculate transfer rate (in bytes per second) */
inline double rate(size_t transferred, double elapsed)
{
	if (elapsed > 0)
		return (double)transferred / elapsed;
	return 0.;
}

/* Estimate remaining time ("ETA") for given transfer rate */
inline double estimate(size_t remaining, double rate)
{
	if (rate > 0)
		return (double)remaining / rate;
	return 0.;
}

/* Return ETA (in seconds) as string, formatted to minutes and seconds */
const char *format_ETA(double remaining)
{
	static char result[6] = "";

	int seconds = remaining + 0.5; /* simplistic round() */
	if (seconds >= 0 && seconds < 6000) {
		snprintf(result, sizeof(result),
			 "%02d:%02d", seconds / 60, seconds % 60);
		return result;
	}
	return "--:--";
}

/* Private progress state variable */

typedef struct {
	progress_cb_t callback;
	size_t total;
	size_t done;
	double start; /* start point (timestamp) for rate and ETA calculation */
} progress_private_t;

static progress_private_t progress = {
	.callback = NULL,
	.start = 0.
};

/* 'External' API */

void progress_start(progress_cb_t callback, size_t expected_total)
{
	progress.callback = callback;
	progress.total = expected_total;
	progress.done = 0;
	progress.start = gettime(); /* reset start time */
}

/* Update progress status, passing information to the callback function. */
void progress_update(size_t bytes_done)
{
	progress.done += bytes_done;
	if (progress.callback)
		progress.callback(progress.total, progress.done);
}

/* Return relative / "elapsed" time, since progress_start() */
static inline double progress_elapsed(void)
{
	if (progress.start != 0.)
		return gettime() - progress.start;
	return 0.;
}

/* Callback function implementing a simple progress bar written to stdout */
void progress_bar(size_t total, size_t done)
{
	static const int WIDTH = 48; /* # of characters to use for progress bar */

	float ratio = total > 0 ? (float)done / total : 0;
	int i, pos = WIDTH * ratio;
	double speed = rate(done, progress_elapsed());
	double eta = estimate(total - done, speed);

	printf("\r%3.0f%% [", ratio * 100); /* current percentage */
	for (i = 0; i < pos; i++) putchar('=');
	for (i = pos; i < WIDTH; i++) putchar(' ');
	if (done < total)
		printf("]%6.1f kB/s, ETA %s ", kilo(speed), format_ETA(eta));
	else
		/* transfer complete, output totals plus a newline */
		printf("] %5.0f kB, %6.1f kB/s\n", kilo(done), kilo(speed));

	fflush(stdout);
}

/*
 * Progress callback that emits percentage numbers, each on a separate line.
 * The output is suitable for piping it into "dialog --gauge".
 *
 * sunxi-fel multiwrite-with-gauge <...> \
 *	| dialog --title "FEL upload progress" \
 *		 --gauge "" 5 70
 */
void progress_gauge(size_t total, size_t done)
{
	if (total > 0) {
		printf("%.0f\n", (float)done / total * 100);
		fflush(stdout);
	}
}

/*
 * A more sophisticated version of progress_gauge() that also updates the
 * prompt (caption) with additional information. This uses a feature of
 * the dialog utility that parses "XXX" delimiters - see 'man dialog'.
 *
 * sunxi-fel multiwrite-with-xgauge <...> \
 *	| dialog --title "FEL upload progress" \
 *		 --backtitle "Please wait..." \
 *		 --gauge "" 6 70
 */
void progress_gauge_xxx(size_t total, size_t done)
{
	if (total > 0) {
		double speed = rate(done, progress_elapsed());
		double eta = estimate(total - done, speed);
		printf("XXX\n");
		printf("%.0f\n", (float)done / total * 100);
		if (done < total)
			printf("%zu of %zu, %.1f kB/s, ETA %s\n",
				done, total, kilo(speed), format_ETA(eta));
		else
			printf("Done: %.1f kB, at %.1f kB/s\n",
				kilo(done), kilo(speed));
		printf("XXX\n");
		fflush(stdout);
	}
}
