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
#define pr_info(F, ...)	fprintf(stderr, "bin2fex: " F, __VA_ARGS__)

#define PTR(B, OFF)	(void*)((char*)(B)+(OFF))

/**
 */
static int decompile_section(void *bin, size_t bin_size,
			     struct script_section *section,
			     FILE *out)
{
	struct script_section_entry *entry = PTR(bin,  section->offset<<2);
	int i = section->length;
	for (; i--; entry++) {
		unsigned type, length;
		type	= (entry->pattern >> 16) & 0xffff;
		length	= (entry->pattern >>  0) & 0xffff;

		pr_info("%s.%s\t(offset:%d, type:%d, length:%d)\n",
			section->name, entry->name,
			entry->offset, type, length);
	}

	return 1; /* success */
}
/**
 */
static int decompile(void *bin, size_t bin_size, FILE *out)
{
	int i;
	struct {
		struct script_head head;
		struct script_section sections[];
	} *script = bin;

	pr_info("version: %d.%d.%d\n", script->head.version[0],
		script->head.version[1], script->head.version[2]);
	pr_info("size: %zu (%d sections)\n", bin_size,
		script->head.sections);

	/* TODO: SANITY: compare head.sections with bin_size */
	for (i=0; i < script->head.sections; i++) {
		struct script_section *section = &script->sections[i];

		pr_info("%s:\t(section:%d, length:%d, offset:%d)\n",
			section->name, i+1, section->length, section->offset);

		if (!decompile_section(bin, bin_size, section, out))
			return 1; /* failure */
	}
	return 0; /* success */
}

/**
 */
int main(int argc, char *argv[])
{
	struct stat sb;
	int ret = -1;
	int in = 0;
	FILE *out = stdout;
	const char *filename[] = {"stdin", "stdout"};
	void *p;

	/* open */
	if (argc>1) {
		filename[0] = argv[1];

		if ((in = open(filename[0], O_RDONLY)) < 0) {
			errf("%s: %s\n", filename[0], strerror(errno));
			goto usage;
		}
		if (argc > 2) {
			filename[1] = argv[2];

			if ((out = fopen(filename[1], "w")) == NULL) {
				errf("%s: %s\n", filename[1], strerror(errno));
				goto usage;
			}
		}
	}

	/* mmap input */
	if (fstat(in, &sb) == -1)
		errf("fstat: %s: %s\n", filename[0], strerror(errno));
	else if (!S_ISREG(sb.st_mode))
		errf("%s: not a regular file (mode:%d).\n", filename[0], sb.st_mode);
	else if ((p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, in, 0)) == MAP_FAILED)
		errf("mmap: %s: %s\n", filename[0], strerror(errno));
	else {
		/* close and decompile mmap */
		close(in);

		ret = decompile(p, sb.st_size, out);
		if (munmap(p, sb.st_size) == -1)
			errf("munmap: %s: %s\n", filename[0], strerror(errno));

		goto done;
	}

usage:
	errf("Usage: %s [<script.bin> [<script.fex>]]\n", argv[0]);

	if (in > 2) close(in);
done:
	if (out != stdout) fclose(out);
	return ret;
}
