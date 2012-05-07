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
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "script_bin.h"

#define MAX_LINE	255

#define pr_info(...)	fprintf(stderr, "fex2bin: " __VA_ARGS__)
#define pr_err(...)	pr_info("E: " __VA_ARGS__)

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
				goto parse_error;
			};

			while (isalnum(*p) || *p == '_')
				p++;
			mark = p;
			p = skip_blank(p);
			if (*p != '=')
				goto invalid_char_at_p;
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
#ifdef VERBOSE
					errf("%s.%s = \"%.*s\"\n",
					     last_section->name, key,
					     (int)(pe-p), p);
#endif
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
					long v = strtol(p, &end, 10);
					if (end == p)
						goto invalid_char_at_p;
					else if (v<0 || v>255) {
						errf("E: %s:%zu: port out of range at %zu (%ld).\n",
						     filename, line, p-buffer+1, v);
					} else {
						int data[] = {-1,-1,-1,-1};
						int port_num = v;
						p = end;
						for (int i=0; *p && i<4; i++) {
							if (memcmp(p, "<default>", 9) == 0) {
								p += 9;
								continue;
							} else if (*p == '<') {
								v = strtol(++p, &end, 10);
								if (end == p) {
									;
								} else if (v<0 || v>INT32_MAX) {
									errf("E: %s:%zu: value out of range at %zu (%ld).\n",
									     filename, line, p-buffer+1, v);
									goto parse_error;
								} else if (*end != '>') {
									p = end;
								} else {
									p = end+1;
									data[i] = v;
									continue;
								}
							}
							break;
						}
						if (*p)
							goto invalid_char_at_p;
						if (script_gpio_entry_new(last_section, key,
									  port, port_num, data)) {
#ifdef VERBOSE
							errf("%s.%s = GPIO %d.%d (%d,%d,%d,%d)\n",
							     last_section->name, key,
							     port, port_num,
							     data[0], data[1], data[2], data[3]);
#endif
							continue;
						}
						perror("malloc");
					}
				}
			} else if (isdigit(*p)) {
				long long v = 0;
				char *end;
				v = strtoll(p, &end, 0);
				p = end;
				if (p != pe) {
					goto invalid_char_at_p;
				} else if (v > UINT32_MAX) {
					errf("E: %s:%zu: value out of range %lld.\n",
					     filename, line, v);
				} else if (script_single_entry_new(last_section, key, v)) {
#ifdef VERBOSE
					errf("%s.%s = %lld\n",
					     last_section->name, key, v);
#endif
					continue;
				}
			}
			goto parse_error;
invalid_char_at_p:
			errf("E: %s:%zu: invalid character at %zu.\n",
			     filename, line, p-buffer+1);
parse_error:
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
	int ret = 1;
	FILE *in = stdin;
	int out = 1;
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

			if ((out = open(fn[1], O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) {
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
		size_t sections, entries, bin_size;
		void *bin;

		bin_size = calculate_bin_size(script, &sections, &entries);
		bin = calloc(1, bin_size);
		if (!bin)
			perror("malloc");
		else if (generate_bin(bin, bin_size, script, sections, entries)) {

			while(bin_size) {
				ssize_t wc = write(out, bin, bin_size);

				if (wc>0) {
					bin += wc;
					bin_size -= wc;
				} else if (wc < 0 && errno != EINTR) {
					pr_err("%s: write: %s\n", fn[2],
					       strerror(errno));
					break;
				}
			}
			if (bin_size == 0)
				ret = 0;
		}
	}

	script_delete(script);
	goto done;
usage:
	errf("Usage: %s [<script.fex> [<script.bin>]]\n", argv[0]);

done:
	if (in && in != stdin) fclose(in);
	if (out > 2) close(out);
	return ret;
}
