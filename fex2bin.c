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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE	255

/**
 */
static inline char *alltrim(char *s, size_t *l)
{
	char *p;
	while (isblank(*s))
		s++;
	p = s;
	while (*++p)
		; /* seek \0 */

	if (p>s+1 && p[-2] == '\r' && p[-1] == '\n')
		p -= 2;
	else if (p>s && p[-1] == '\n')
		p -= 1;
	*p-- = '\0';

	while (p>s && isblank(*p))
		*p-- = '\0';

	*l = p-s+1;
	return s;
}

/**
 */
static int parse_fex(FILE *in, const char *filename,
		     struct script *UNUSED(script))
{
	char buffer[MAX_LINE+1];
	int ok = 1;

	/* TODO: deal with longer lines correctly (specially in comments) */
	for(size_t line = 1; fgets(buffer, sizeof(buffer), in); line++) {
		size_t l;
		char *s = alltrim(buffer, &l);

		if (l == 0 || *s == ';' || *s == '#')
			;
		else if (*s == '[') {
			/* section */
			char *p = ++s;
			while (isalnum(*p) || *p == '_')
				p++;

			if (*p == ']' && *(p+1) == '\0') {
				*p = '\0';
				errf("I: %s:%zu: [%s]\n", filename, line, s);
			} else if (*p) {
				errf("E: %s:%zu: invalid character at %zu.\n",
				     filename, line, p-buffer+1);
				goto parse_error;
			}
		} else {
			/* key = value */
			fprintf(stdout, "%s:%zu: ", filename, line);
			fputs(s, stdout);
			fputc('\n', stdout);
		}
	};

	if (ferror(in)) {
parse_error:
		ok = 0;
	}
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
	errf("Usage: %s [<script.fex> [<script.bin>]]\n", argv[0]);

done:
	if (in && in != stdin) fclose(in);
	if (out && out != stdout) fclose(out);
	return ret;
}
