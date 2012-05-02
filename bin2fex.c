/*
 * Copyright (C) 2012  Alejandro Mery <amery@geeks.cl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
#include "sunxi-tools.h"
#include "bin2fex.h"

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define errf(...)	fprintf(stderr, __VA_ARGS__)

static int decompile(void *bin, size_t bin_size, int out, const char *out_name);
static int decompile_section(void *bin, size_t bin_size,
			     struct script_section *section,
			     int out, const char* out_named);
/**
 */
static int decompile(void *bin, size_t bin_size, int out, const char *out_name)
{
	int i;
	struct {
		struct script_head head;
		struct script_section sections[];
	} *script = bin;

	errf("bin format: %d.%d.%d\n", script->head.version[0],
	     script->head.version[1], script->head.version[2]);
	errf("bin size: %zu (%d sections)\n", bin_size,
	     script->head.sections);

	/* TODO: SANITY: compare head.sections with bin_size */
	for (i=0; i < script->head.sections; i++) {
		struct script_section *section = &script->sections[i];

		errf("%s: (section:%d, length:%d, offset:%d)\n",
		     section->name, i+1, section->length, section->offset);

		if (!decompile_section(bin, bin_size, section, out, out_name))
			return 1; /* failure */
	}
	return 0; /* success */
}

/**
 */
static int decompile_section(void *bin, size_t bin_size,
			     struct script_section *section,
			     int out, const char* out_named)
{
	return 1; /* success */
}

/**
 */
int main(int argc, char *argv[])
{
	struct stat sb;
	int ret = -1;
	int fd[] = {0, 1};
	const char *filename[] = { "stdin", "stdout" };
	void *p;

	/* open */
	if (argc>1) {
		filename[0] = argv[1];

		if ((fd[0] = open(filename[0], O_RDONLY)) < 0) {
			errf("%s: %s\n", filename[0], strerror(errno));
			goto usage;
		}
		if (argc > 2) {
			filename[1] = argv[2];

			if ((fd[1] = open(filename[1], O_WRONLY|O_CREAT, 0666)) < 0) {
				errf("%s: %s\n", filename[1], strerror(errno));
				goto usage;
			}
		}
	}

	/* mmap input */
	if (fstat(fd[0], &sb) == -1)
		errf("fstat: %s: %s\n", filename[0], strerror(errno));
	else if (!S_ISREG(sb.st_mode))
		errf("%s: not a regular file (mode:%d).\n", filename[0], sb.st_mode);
	else if ((p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd[0], 0)) == MAP_FAILED)
		errf("mmap: %s: %s\n", filename[0], strerror(errno));
	else {
		/* decompile mmap */
		close(fd[0]);

		ret = decompile(p, sb.st_size, fd[1], filename[1]);
		if (munmap(p, sb.st_size) == -1)
			errf("munmap: %s: %s\n", filename[0], strerror(errno));

		goto done;
	}

usage:
	errf("Usage: %s [<script.bin> [<script.fex>]]\n", argv[0]);

	if (fd[0] > 2) close(fd[0]);
done:
	if (fd[1] > 2) close(fd[1]);
	return ret;
}
