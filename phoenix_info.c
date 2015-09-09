/*
 * Copyright (C) 2012  Henrik Nordstrom <henrik@henriknordstrom.net>
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

/* Needs _BSD_SOURCE for htole and letoh  */
/* glibc 2.20+ also requires _DEFAULT_SOURCE */
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "endian_compat.h"

struct phoenix_ptable {
	char signature[16];		/* "PHOENIX_CARD_IMG" */
	unsigned int unknown1;		/* 0x00200100 */
	unsigned short parts;		/* Number of partitions */
	unsigned short unknown2;	/* 0x0001 */
	unsigned char pad[8];
	struct phoenix_entry {
		unsigned int start;		/* 512 bytes blocks */
		unsigned int size;		/* bytes */
		unsigned int unknown;		/* ???? */
		unsigned int sig;		/* "add\0" */
	} part[62];
} ptable;

static int save_part(struct phoenix_ptable *ptable, int part, const char *dest, FILE *in)
{
	int l = strlen(dest) + 16;
	char outname[l];
	FILE *out;
	char *buf = NULL;
	int ret = 0;
	snprintf(outname, l, dest, part);
	if (part > ptable->parts) {
		fprintf(stderr, "ERROR: Part index out of range\n");
		return -1;
	}
	buf = malloc(ptable->part[part].size);
	if (!buf)
		goto err;
	if (strcmp(outname, "-") != 0)
		out = fopen(outname, "wb");
	else
		out = stdout;
	if (!out)
		goto err;
	if (fseek(in, ptable->part[part].start * 0x200, SEEK_SET) == -1)
		goto err;
	if (fread(buf, ptable->part[part].size, 1, in) != 1)
		goto err;
	if (fwrite(buf, ptable->part[part].size, 1, out) != 1)
		goto err;
	ret = 0;
_exit:
	if (buf)
		free(buf);
	if (out != stdout)
		fclose(out);
	return ret;
err:
	perror(NULL);
	ret = -1;
	goto _exit;
}

static void usage(char **argv)
{
	printf("Usage: %s [options] [phoenix_image]\n"
		"	-v	verbose\n"
		"	-q	quiet\n"
		"	-p N	part number\n"
		"	-o X	destination directory, file or pattern (%%d for part number)\n"
		"	-s	save all parts\n"
		, argv[0]
	);
}

int main(int argc, char **argv)
{
	int i;
	FILE *in = stdin;
	int verbose = 1;
	int save_parts = 0;
	int part = -1;
	int opt;
	const char *dest = "%d.img";
	
	while ((opt = getopt(argc, argv, "vqso:p:?")) != -1) {
		switch(opt) {
		case 'v':
			verbose++;
			break;
		case 'q':
			if (verbose)
				verbose--;
			break;
		case 'o':
			dest = optarg;
			save_parts = 1;
			break;
		case 'p':
			save_parts = 1;
			part = atoi(optarg);
			break;
		case 's':
			save_parts = 1;
			break;
		default:
			usage(argv);
			exit(1);
			break;
		}
	}
	if (save_parts && !strchr(dest, '%')) {
		const char *t = dest;
		if (!*t)
			t = "./";
		if (t[strlen(t)-1] == '/' || !part) {
			int l = strlen(t) + strlen("/%d.img") + 1;
			char *tmp = malloc(l);
			snprintf(tmp, l, "%s/%%d.img", optarg);
			t = tmp;
		}
		dest = t;
	}
	if (argc > optind + 1) {
		usage(argv);
		exit(1);
	}
	if (optind < argc ) {
		in = fopen(argv[optind], "rb");
	}
	fseek(in, 0x1C00, SEEK_CUR);
	fread(&ptable, 1, 0x400, in);
	if (strncmp(ptable.signature, "PHOENIX_CARD_IMG", 16) != 0) {
		fprintf(stderr, "ERROR: Not a phoenix image\n");
		exit(1);
	}
	if (verbose > 1) {
		printf("????  : %08x\n", le32toh(ptable.unknown1));
		printf("Parts : %d\n", le16toh(ptable.parts));
		printf("????  : %08x\n", le16toh(ptable.unknown2));
		printf("pad   : %02x%02x%02x%02x%02x%02x%02x%02x\n", ptable.pad[0], ptable.pad[1], ptable.pad[2], ptable.pad[3], ptable.pad[4], ptable.pad[5], ptable.pad[6], ptable.pad[7]);
		printf("\n");
	}
	for (i = 0; i < le16toh(ptable.parts); i++) {
		if (verbose && (part == -1 || part == i)) {
			printf("part %d:\n", i);
			printf("\tstart: 0x%08x (%u / 0x%08x)\n", le32toh(ptable.part[i].start)*512, le32toh(ptable.part[i].start), le32toh(ptable.part[i].start));
			printf("\tsize : %u\n", le32toh(ptable.part[i].size));
			printf("\t?????: %08x\n", le32toh(ptable.part[i].unknown));
			if (verbose > 1 || le32toh(ptable.part[i].sig) != 0x00646461)
				printf("\tsig??: %08x\n", le32toh(ptable.part[i].sig));
			printf("\n");
		}
		if (save_parts && (part == -1 || part == i)) {
			save_part(&ptable, i, dest, in);
		}
	}
}
