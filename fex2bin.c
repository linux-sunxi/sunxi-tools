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
#include "fex2bin.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE	127

/**
 */
static inline char *alltrim(char *s, size_t *l)
{
	char *p;
	while (*s == ' ' || *s == '\t')
		s++;
	p = s;
	while (*++p)
		; /* seek \0 */

	if (p>s && p[-1] == '\n') {
		if (p>s+1 && p[-2] == '\r')
			p-=2;
		else
			p-=1;
	}

	while (p>s) {
		if (*p == ' ' || *p == '\t')
			p--;
		else
			break;
	}

	*p = '\0';
	*l = p-s;
	return s;
}

/**
 */
static int parse_fex(FILE *in, const char *UNUSED(filename),
		     struct script *UNUSED(script))
{
	char buffer[MAX_LINE+1];
	int ok = 1;

	for(size_t line = 1; fgets(buffer, sizeof(buffer), in); line++) {
		size_t l, col;
		char *p = alltrim(buffer, &l);
		col = p-buffer+1;

		fputs(p, stdout);
		fputc('\n', stdout);

		(void)col;
	};

	if (ferror(in))
		ok = 0;
	return ok;
}

/**
 */
int main(int argc, char *argv[])
{
	int ret = -1;
	FILE *in = stdin, *out = stdout;
	const char *fn[] = {"stdin", "stdout"};
	struct script *script;

	if (argc>1) {
		if (strcmp(argv[1],"-") == 0)
			; /* we are using stdin anyway */
		else if ((fn[0] = argv[1]) &&
			 (in = fopen(fn[0], "r")) == NULL) {
			errf("%s: %s\n", fn[0], strerror(errno));
			goto usage;
		}

		if (argc>2) {
			fn[1] = argv[2];

			if ((out = fopen(fn[1], "w")) == NULL) {
				errf("%s: %s\n", fn[1], strerror(errno));
				goto usage;
			}
		}
	}

	if ((script = script_new()) == NULL) {
		errf("malloc: %s\n", strerror(errno));
		goto done;
	}

	if (parse_fex(in, fn[0], script)) {
		ret = 0;
	}
	script_delete(script);
	goto done;
usage:
	errf("Usage: %s [<script.fex> [<script.bin>]\n", argv[0]);

done:
	if (in && in != stdin) fclose(in);
	if (out && out != stdout) fclose(out);
	return ret;
}
