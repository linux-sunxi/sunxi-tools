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
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE	255

/** find first not blank char */
static inline char *skip_blank(char *p)
{
	while(isblank(*p))
		p++;
	return p;
}

/** trim out blank chars at the end of a string */
static inline char *rtrim(const char *s, char *p)
{
	if (p>s) {
		while (p!=s && isblank(*--p))
			;
		*++p='\0';
	}
	return p;
}

/**
 */
static int parse_fex(FILE *in, const char *filename, struct script *script)
{
	char buffer[MAX_LINE+1];
	int ok = 1;
	struct script_section *last_section = NULL;

	/* TODO: deal with longer lines correctly (specially in comments) */
	for(size_t line = 1; ok && fgets(buffer, sizeof(buffer), in); line++) {
		char *s = skip_blank(buffer); /* beginning */
		char *pe = s; /* \0... to be found */

		if (*pe) while (*++pe)
			;

		if (pe>s && pe[-1] == '\n') {
			if (pe>s+1 && pe[-2] == '\r')
				pe -= 2;
			else
				pe -= 1;
			*pe = '\0';
		}

		pe = rtrim(s, pe);

		if (pe == s || *s == ';' || *s == '#')
			continue; /* empty */
		else if (*s == '[') {
			/* section */
			char *p = ++s;
			while (isalnum(*p) || *p == '_')
				p++;

			if (*p == ']' && *(p+1) == '\0') {
				*p = '\0';
				if ((last_section = script_section_new(script, s)))
					continue;

				perror("malloc");
			} else if (*p) {
				errf("E: %s:%zu: invalid character at %zu.\n",
				     filename, line, p-buffer+1);
			} else {
				errf("E: %s:%zu: incomplete section declaration.\n",
				     filename, line);
			}
			ok = 0;
		} else {
			/* key = value */
			const char *key = s;
			char *mark, *p = s;

			if (!last_section) {
				errf("E: %s:%zu: data must follow a section.\n",
				     filename, line);
				ok = 0;
				break;
			};

			while (isalnum(*p) || *p == '_')
				p++;
			mark = p;
			p = skip_blank(p);
			if (*p != '=') {
				errf("E: %s:%zu: invalid character at %zu.\n",
				     filename, line, p-buffer+1);
				ok = 0;
				break;
			}
			*mark = '\0'; /* truncate key */
			p = skip_blank(p+1);

			if (*p == '\0') {
				/* NULL */
				if (script_null_entry_new(last_section, key))
					continue;
				perror("malloc");
			} else if (pe > p+1 && *p == '"' && pe[-1] == '"') {
				/* string */
				p++; *--pe = '\0';
				if (script_string_entry_new(last_section, key, pe-p, p)) {
					errf("%s.%s = \"%.*s\"\n",
					     last_section->name, key,
					     (int)(pe-p), p);
					continue;
				}

				perror("malloc");
			} else if (memcmp("port:P", p, 6) == 0) {
				/* GPIO */
				p += 6;
				if (*p < 'A' || *p > 'Z')
					;
				else {
					char *end;
					int port = *p++ - 'A';
					long port_num = strtol(p, &end, 10);
					if (end == p)
						;
					else if (port_num<0 || port_num>255) {
						errf("E: %s:%zu: port out of range at %zu.\n",
						     filename, line, p-buffer+1);
						ok = 0;
						break;
					} else {
						p = end;
						errf("%s.%s = GPIO %d.%ld (%s)\n",
						     last_section->name, key,
						     port, port_num, p);
						continue;
					}
				}
				errf("E: %s:%zu: invalid character at %zu.\n",
				     filename, line, p-buffer+1);
			} else if (isdigit(*p)) {
				long long v = 0;
				char *end;
				v = strtoll(p, &end, 0);
				if (end != pe) {
					errf("E: %s:%zu: invalid character at %zu.\n",
					     filename, line, end-buffer+1);
				} else if (v > UINT32_MAX) {
					errf("E: %s:%zu: value out of range %lld.\n",
					     filename, line, v);
				} else if (script_single_entry_new(last_section, key, v)) {
					errf("%s.%s = %lld\n",
					     last_section->name, key, v);
					continue;
				}
			} else {
				errf("E: %s:%zu: invalid character at %zu.\n",
				     filename, line, p-buffer+1);
			}
			ok = 0;
		}
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

	errf("WARNING: this tool is still not functional, sorry\n");

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
