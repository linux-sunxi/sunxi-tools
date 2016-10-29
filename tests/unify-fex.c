/*
 * Copyright (C) 2016  Bernhard Nortmann <bernhard.nortmann@web.de>
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

/*
 * unify-fex.c
 *
 * A utility program to do some standard transformations on .fex files,
 * to allow simpler (diff) comparison with the output of bin2fex.
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* string macro to determine if str starts with a given literal */
#define starts(str, literal) \
	(strncmp(str, "" literal, sizeof(literal) - 1) == 0)

int main(int argc, char **argv)
{
	FILE *input = stdin;
	char line[1024];
	char *c, *p;
	int64_t num;

	if (argc >= 2) {
		input = fopen(argv[1], "r");
		if (!input) {
			perror("failed to open input file");
			exit(EXIT_FAILURE);
		}
	}

	/* loop over all input lines, output goes to stdout */
	while (fgets(line, sizeof(line), input)) {

		/* strip all whitespace (even CR/LF) from the input line */
		for (c = p = line; *p; p++) {
			if (!isspace(*p))
				*c++ = *p;
		}
		*c = '\0';

		if (*line == ';' || *line == '#')
			/* This is a comment line, simply ignore it */
			continue;
		if (*line == ':')
			continue; /* suspect malformed comment, ignore */

		if ((p = strchr(line, '='))) {
			/* This is a <key> = <value> line, reformat it */
			c = strdup(p + 1);
			sprintf(p, " = %s", c);
			free(c);
			p += 3; /* have p point to the beginning of <value> */

			if (starts(p, "port:")) {
				if (p[5] == 'P') { /* port:P... */
					/* get pin number (including bank) */
					num = ((p[6] - 'A') << 5) + strtoll(p + 7, &c, 10);
					c = strdup(c);
					sprintf(p, "port:P%c%02" PRId64 "%s", 'A' + (int)(num >> 5), num & 0x1F, c);
					free(c);

					/* check angle brackets to determine options count */
					num = 0;
					for (c = p + 9; *c; c++) {
						if (*c == '<')
							num++;
					}
					/* append "<default>" for missing options */
					c = strrchr(p, '\0');
					while (num < 4) {
						c += sprintf(c, "<default>");
						num++;
					}
				}
			} else {
				/*
				 * fix formatting of numeric values, depending on the keyword
				 * these are a bit nasty, since they vary wildly between hex
				 * and decimal - see decompile_single_mode() in script_fex.c
				 */
				num = strtoll(p, NULL, 0);
				if (num || *p == '0') {
					int hex = starts(line, "csi_twi_addr");
					hex |= starts(line, "ctp_twi_addr");
					hex |= starts(line, "dram_baseaddr");
					hex |= starts(line, "dram_emr");
					hex |= starts(line, "dram_tpr");
					hex |= starts(line, "dram_zq");
					hex |= starts(line, "g2d_size");
					hex |= starts(line, "gsensor_twi_addr");
					hex |= starts(line, "lcd_gamma_tbl_");
					hex |= starts(line, "rtp_press_threshold ");
					hex |= starts(line, "rtp_sensitive_level");
					hex |= starts(line, "tkey_twi_addr");

					/* large decimals will be decompiled as negative */
					if (!hex && num >= 2147483648LL)
						num -= 4294967296LL;

					sprintf(p, hex ? "0x%" PRIx64 : "%" PRId64, num);
				} else {
					/*
					 * We expect all other (= non-numeric) values
					 * to be strings, always quote them.
					 */
					if (*p && (*p != '"')) {
						c = strdup(p);
						sprintf(p, "\"%s\"", c);
						free(c);
					}
				}
			}
		}

		puts(line);
	}

	if (ferror(input)) {
		perror("file read error");
		exit(EXIT_FAILURE);
	}

	fclose(input);
	return EXIT_SUCCESS;
}
