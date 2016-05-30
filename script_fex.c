/*
 * Copyright (C) 2012  Alejandro Mery <amery@geeks.cl>
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
#include "common.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "script.h"
#include "script_fex.h"

#define MAX_LINE	255

#define pr_info(...)	errf("fexc-fex: " __VA_ARGS__)
#define pr_err(...)	errf("E: fexc-fex: " __VA_ARGS__)

#ifdef DEBUG
#define pr_debug(...)	errf("D: fexc-fex: " __VA_ARGS__)
#else
#define pr_debug(...)
#endif

/*
 * generator
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

int script_generate_fex(FILE *out, const char *UNUSED(filename),
			struct script *script)
{
	struct list_entry *ls, *le;
	struct script_section *section;
	struct script_entry *entry;

	for (ls = list_first(&script->sections); ls;
	     ls = list_next(&script->sections, ls)) {
		section = container_of(ls, struct script_section, sections);

		fprintf(out, "[%s]\n", section->name);
		for (le = list_first(&section->entries); le;
		     le = list_next(&section->entries, le)) {
			entry = container_of(le, struct script_entry, entries);

			switch(entry->type) {
			case SCRIPT_VALUE_TYPE_SINGLE_WORD: {
				int mode = decompile_single_mode(entry->name);
				struct script_single_entry *single;
				single = container_of(entry, struct script_single_entry, entry);

				fprintf(out, "%s = ", entry->name);
				if (mode < 0)
					fprintf(out, "%d", single->value);
				else if (mode > 0)
					fprintf(out, "0x%0*x", mode, single->value);
				else
					fprintf(out, "0x%x", single->value);
				fputc('\n', out);
				}; break;
			case SCRIPT_VALUE_TYPE_STRING: {
				struct script_string_entry *string;
				string = container_of(entry, struct script_string_entry, entry);
				fprintf(out, "%s = \"%.*s\"\n", entry->name,
					(int)string->l, string->string);
				}; break;
			case SCRIPT_VALUE_TYPE_MULTI_WORD:
				abort();
			case SCRIPT_VALUE_TYPE_GPIO: {
				char port = 'A'-1;
				struct script_gpio_entry *gpio;
				gpio = container_of(entry, struct script_gpio_entry, entry);

				if (gpio->port == 0xffff) {
					fprintf(out, "%s = port:power%u", entry->name,
						gpio->port_num);
				} else {
					port += gpio->port;
					fprintf(out, "%s = port:P%c%02u", entry->name,
						port, gpio->port_num);
				}
				for (const int *p = gpio->data, *pe = p+4; p != pe; p++) {
					if (*p == -1)
						fputs("<default>", out);
					else
						fprintf(out, "<%d>", *p);
				}
				fputc('\n', out);
				}; break;
			case SCRIPT_VALUE_TYPE_NULL:
				fprintf(out, "%s =\n", entry->name);
				break;
			}
		}
		fputc('\n', out);
	}
	return 1;
}

/*
 * parser
 */

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
int script_parse_fex(FILE *in, const char *filename, struct script *script)
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
		if (*s == ':') {
			/* see https://github.com/linux-sunxi/sunxi-boards/issues/50 */
			errf("Warning: %s:%zu: invalid line, suspecting typo/malformed comment.\n",
			     filename, line);
			continue; /* ignore this line */
		}
		if (*s == '[') {
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
					pr_debug("%s.%s = \"%.*s\"\n",
						 last_section->name, key,
						 (int)(pe-p), p);
					continue;
				}
				perror("malloc");
			} else if (memcmp("port:", p, 5) == 0) {
				/* GPIO */
				p += 5;
				if (p[0] == 'P' &&
				    (p[1] < 'A' || p[1] > ('A' + GPIO_BANK_MAX)))
					;
				else if (*p != 'P' &&
					 memcmp(p, "power", 5) != 0)
					;
				else {
					char *end;
					int port;
					long v;

					if (*p == 'P') {
						/* port:PXN */
						port = p[1] - 'A' + 1;
						p += 2;
					} else {
						/* port:powerN */
						port = 0xffff;
						p += 5;
					}

					v = strtol(p, &end, 10);
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
							pr_debug("%s.%s = GPIO %d.%d (%d,%d,%d,%d)\n",
								 last_section->name, key,
								 port, port_num,
								 data[0], data[1], data[2], data[3]);
							continue;
						}
						perror("malloc");
					}
				}
			} else if (isdigit(*p) || (*p == '-' && isdigit(*(p+1)))) {
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
					pr_debug("%s.%s = %lld\n",
						 last_section->name, key, v);
					continue;
				}
			} else {
				goto invalid_char_at_p;
			}
			errf("E: %s:%zu: parse error at %zu.\n",
			     filename, line, p-buffer+1);
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
