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
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>

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

int main(int argc, char **argv)
{
	int i;
	FILE *in = stdin;
	if (argc > 1) {
		in = fopen(argv[1], "r");
	}
	fseek(in, 0x1C00, SEEK_CUR);
	fread(&ptable, 1, 0x400, in);
	if (strncmp(ptable.signature, "PHOENIX_CARD_IMG", 16) != 0) {
		printf("ERROR: Not a phoenix image\n");
		exit(1);
	}
	printf("????  : %08x\n", le32toh(ptable.unknown1));
	printf("Parts : %d\n", le16toh(ptable.parts));
	printf("????  : %08x\n", le16toh(ptable.unknown2));
	printf("pad   : %02x%02x%02x%02x%02x%02x%02x%02x\n", ptable.pad[0], ptable.pad[1], ptable.pad[2], ptable.pad[3], ptable.pad[4], ptable.pad[5], ptable.pad[6], ptable.pad[7]);
	printf("\n");
	for (i = 0; i < le16toh(ptable.parts); i++) {
		printf("part %d:\n", i);
		printf("\tstart: 0x%08x (%u / 0x%08x)\n", le32toh(ptable.part[i].start)*512, le32toh(ptable.part[i].start), le32toh(ptable.part[i].start));
		printf("\tsize : %u\n", le32toh(ptable.part[i].size));
		printf("\t?????: %08x\n", le32toh(ptable.part[i].unknown));
		if (le32toh(ptable.part[i].sig) != 0x00646461)
			printf("\tsig??: %08x\n", le32toh(ptable.part[i].sig));
		printf("\n");
	}
}
