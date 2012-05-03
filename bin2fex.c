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
#define pr_info(F, ...)	fprintf(out, "; bin2fex: " F, __VA_ARGS__)
#define pr_err(F, ...)	pr_info("ERROR: " F, __VA_ARGS__)

#define PTR(B, OFF)	(void*)((char*)(B)+(OFF))

/**
 */
static inline size_t strlen2(const char *s)
{
	size_t l = strlen(s);
	const char *p = &s[l-1];
	while (l && *p >= '0' && *p <= '9') {
		l--;
		p--;
	}
	return l;
}

static int find_full_match(const char *s, size_t l, const char **list)
{
	while (*list) {
		if (memcmp(s, *list, l) == 0)
			return 1;
		list++;
	}

	return 0;
}

/**
 */
static inline int decompile_gpio(struct script_section *section,
				 struct script_section_entry *entry,
				 struct script_gpio_value *gpio,
				 int length, FILE *out)
{
	int ok = 1;
	char port = '?';

	if (length != 6) {
		pr_err("%s.%s: invalid length %d (assuming %d)\n",
		       section->name, entry->name, length, 6);
		ok = 0;
	}

	if (gpio->port < 1 || gpio->port > 10) {
		pr_err("%s.%s: unknown GPIO port type %d\n",
		       section->name, entry->name, gpio->port);
		ok = 0;
	} else {
		port = 'A' + (gpio->port-1);
	}

	fprintf(out, "%s\t= port:P%c%02d", entry->name, port, gpio->port_num);
	for (const int *p = &gpio->mul_sel, *pe = p+4; p != pe; p++) {
		if (*p == -1)
			fputs("<default>", out);
		else
			fprintf(out, "<%d>", *p);
	}
	fputc('\n', out);

	return ok;
}

/**
 */
static int decompile_single_mode(const char *name)
{
	static const char *hexa_entries[] = {
		"dram_baseaddr", "dram_zq", "dram_tpr", "dram_emr",
		"g2d_size",
		"rtp_press_threshold", "rtp_sensitive_level",
		"ctp_twi_addr", "csi_twi_addr", "csi_twi_addr_b", "tkey_twi_addr",
		"lcd_gamma_tbl_",
		"gsensor_twi_addr",
		NULL };
	size_t l = strlen2(name);

	if (find_full_match(name, l, hexa_entries))
		return 0;
	else
		return -1;
}
static inline int decompile_single(struct script_section *section,
				   struct script_section_entry *entry,
				   int32_t *d,
				   int length, FILE *out)
{
	int ok = 1;
	int mode;

	if (length != 1) {
		pr_err("%s.%s: invalid length %d (assuming %d)\n",
		section->name, entry->name, length, 1);
		ok = 0;
	}

	mode = decompile_single_mode(entry->name);

	fprintf(out, "%s\t= ", entry->name);
	if (mode < 0)
		fprintf(out, "%d", *d);
	else if (mode > 0)
		fprintf(out, "0x%0*x", mode, *d);
	else
		fprintf(out, "0x%x", *d);

	fputc('\n', out);

	return ok;
}

/**
 */
static int decompile_section(void *bin, size_t bin_size,
			     struct script_section *section,
			     FILE *out)
{
	struct script_section_entry *entry = PTR(bin,  section->offset<<2);
	int i = section->length;
	int ok = 1;

	fprintf(out, "[%s]\n", section->name);
	for (; i--; entry++) {
		void *data = PTR(bin, entry->offset<<2);
		unsigned type, length;
		type	= (entry->pattern >> 16) & 0xffff;
		length	= (entry->pattern >>  0) & 0xffff;

		switch(type) {
		case SCRIPT_VALUE_TYPE_SINGLE_WORD:
			if (!decompile_single(section, entry, data, length, out))
				ok = 0;
			break;
		case SCRIPT_VALUE_TYPE_STRING: {
			size_t bytes = length << 2;
			const char *p, *pe, *s = data;

			for(p=s, pe=s+bytes; *p && p!=pe; p++)
				; /* seek end-of-string */

			fprintf(out, "%s\t= \"%.*s\"\n", entry->name,
				(int)(p-s), s);
			}; break;
		case SCRIPT_VALUE_TYPE_GPIO:
			if (!decompile_gpio(section, entry, data, length, out))
			    ok = 0;
			break;
		case SCRIPT_VALUE_TYPE_NULL:
			fprintf(out, "%s\t=\n", entry->name);
			break;
		default:
			pr_err("%s.%s: unknown type %d\n",
			       section->name, entry->name, type);
			fprintf(out, "%s\t=\n", entry->name);
			ok = 0;
			break;
		}
	}
	fputc('\n', out);

	return ok;
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
