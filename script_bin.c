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
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "script.h"
#include "script_bin.h"

#define pr_info(...)	errf("fexc-bin: " __VA_ARGS__)
#define pr_err(...)	errf("E: fexc-bin: " __VA_ARGS__)

#ifdef DEBUG
#define pr_debug(...)	errf("D: fexc-bin: " __VA_ARGS__)
#else
#define pr_debug(...)
#endif

#define PTR(B, OFF)	(void*)((char*)(B)+(OFF))
#define WORDS(S)	(((S)+(sizeof(uint32_t)-1))/(sizeof(uint32_t)))

/*
 * generator
 */
size_t script_bin_size(struct script *script,
		       size_t *sections, size_t *entries)
{
	size_t words = 0, bin_size = 0;
	struct list_entry *ls, *le;
	struct script_section *section;
	struct script_entry *entry;
	struct script_string_entry *string;

	*sections = *entries = 0;

	/* count */
	for (ls = list_first(&script->sections); ls;
	     ls = list_next(&script->sections, ls)) {
		section = container_of(ls, struct script_section, sections);
		size_t c = 0;

		for (le = list_first(&section->entries); le;
		     le = list_next(&section->entries, le)) {
			size_t size = 0;
			entry = container_of(le, struct script_entry, entries);
			c++;

			switch(entry->type) {
			case SCRIPT_VALUE_TYPE_NULL:
			case SCRIPT_VALUE_TYPE_SINGLE_WORD:
				size = sizeof(uint32_t);
				break;
			case SCRIPT_VALUE_TYPE_STRING:
				string = container_of(entry, struct script_string_entry,
						      entry);
				size = string->l;
				break;
			case SCRIPT_VALUE_TYPE_GPIO:
				size = sizeof(struct script_bin_gpio_value);
				break;
			default:
				abort();
			}
			words += WORDS(size);
		}
		*sections += 1;
		*entries += c;
	}

	bin_size = sizeof(struct script_bin_head) +
		(*sections)*sizeof(struct script_bin_section) +
		(*entries)*sizeof(struct script_bin_entry) +
		words*sizeof(uint32_t);
	pr_debug("sections:%zu entries:%zu data:%zu/%zu -> %zu\n",
		 *sections, *entries, words, words*sizeof(uint32_t),
		 bin_size);
	return bin_size;
}

int script_generate_bin(void *bin, size_t UNUSED(bin_size),
			struct script *script,
			size_t sections, size_t entries)
{
	struct script_bin_head *head;
	struct script_bin_section *section;
	struct script_bin_entry *entry;
	void *data;

	struct list_entry *ls, *le;

	head = bin;
	section = head->section;
	entry = (void*)section+sections*sizeof(*section);
	data = (void*)entry+entries*sizeof(*entry);

	pr_debug("head....:%p\n", head);
	pr_debug("section.:%p (offset:%zu, each:%zu)\n", section,
		 (void*)section-bin, sizeof(*section));
	pr_debug("entry...:%p (offset:%zu, each:%zu)\n", entry,
		 (void*)entry-bin, sizeof(*entry));
	pr_debug("data....:%p (offset:%zu)\n", data,
		 (void*)data-bin);

	head->sections = sections;
	head->version[0] = 0;
	head->version[1] = 1;
	head->version[2] = 2;

	for (ls = list_first(&script->sections); ls;
	     ls = list_next(&script->sections, ls)) {
		struct script_section *s;
		size_t c = 0;
		s = container_of(ls, struct script_section, sections);

		memcpy(section->name, s->name, strlen(s->name));
		section->offset = ((void*)entry-bin)>>2;

		for (le = list_first(&s->entries); le;
		     le = list_next(&s->entries, le)) {
			struct script_entry *e;
			e = container_of(le, struct script_entry, entries);
			size_t size = 0;

			memcpy(entry->name, e->name, strlen(e->name));
			entry->offset = ((void*)data-bin)>>2;
			entry->pattern = (e->type<<16);

			switch(e->type) {
			case SCRIPT_VALUE_TYPE_SINGLE_WORD: {
				struct script_single_entry *single;
				int32_t *bdata = data;
				single = container_of(e, struct script_single_entry, entry);

				*bdata = single->value;
				size = sizeof(*bdata);
				}; break;
			case SCRIPT_VALUE_TYPE_STRING: {
				struct script_string_entry *string;
				string = container_of(e, struct script_string_entry, entry);
				size = string->l;
				memcpy(data, string->string, size);
				/* align */
				size += sizeof(uint32_t)-1;
				size /= sizeof(uint32_t);
				size *= sizeof(uint32_t);
				}; break;
			case SCRIPT_VALUE_TYPE_MULTI_WORD:
				abort();
			case SCRIPT_VALUE_TYPE_GPIO: {
				struct script_gpio_entry *gpio;
				struct script_bin_gpio_value *bdata = data;
				gpio = container_of(e, struct script_gpio_entry, entry);
				bdata->port = gpio->port;
				bdata->port_num = gpio->port_num;
				bdata->mul_sel = gpio->data[0];
				bdata->pull = gpio->data[1];
				bdata->drv_level = gpio->data[2];
				bdata->data = gpio->data[3];
				size = sizeof(*bdata);
				}; break;
			case SCRIPT_VALUE_TYPE_NULL:
				size = sizeof(uint32_t);
				break;
			}

			data += size;
			entry->pattern |= (size>>2);
			pr_debug("%s.%s <%p> (type:%d, words:%d (%zu), offset:%d)\n",
				 section->name, entry->name, entry,
				 (entry->pattern>>16) & 0xffff,
				 (entry->pattern>>0) & 0xffff, size,
				 entry->offset);
			c++;
			entry++;
		}

		section->length = c;
		pr_debug("%s <%p> (length:%d, offset:%d)\n",
			 section->name, section, section->length, section->offset);

		section++;
	}
	return 1;
}

/*
 * decompiler
 */
static int decompile_section(void *bin, size_t bin_size,
			     const char *filename,
			     struct script_bin_section *section,
			     struct script *script)
{
	struct script_bin_entry *entry;
	struct script_section *s;
	int size;

	if ((section->offset < 0) || (section->offset > (int)(bin_size / 4))) {
		pr_err("Malformed data: invalid section offset: %d\n",
		       section->offset);
		return 0;
	}

	size = bin_size - 4 * section->offset;

	if ((section->length < 0) ||
	    (section->length > (size / (int)sizeof(struct script_bin_entry)))) {
		pr_err("Malformed data: invalid section length: %d\n",
		       section->length);
		return 0;
	}

	if ((s = script_section_new(script, section->name)) == NULL)
		goto malloc_error;

	entry = PTR(bin, section->offset<<2);

	for (int i = section->length; i--; entry++) {
		void *data = PTR(bin, entry->offset<<2);
		unsigned type, words;
		type	= (entry->pattern >> 16) & 0xffff;
		words	= (entry->pattern >>  0) & 0xffff;

		for (char *p = entry->name; *p; p++)
			if (!(isalnum(*p) || *p == '_')) {
				pr_info("Warning: Malformed entry key \"%s\"\n",
					entry->name);
				break;
			}

		switch(type) {
		case SCRIPT_VALUE_TYPE_SINGLE_WORD: {
			uint32_t *v = data;
			if (words != 1) {
				pr_err("%s: %s.%s: invalid length %d (assuming %d)\n",
				filename, section->name, entry->name, words, 1);
			}
			if (!script_single_entry_new(s, entry->name, *v))
				goto malloc_error;
			}; break;
		case SCRIPT_VALUE_TYPE_STRING: {
			size_t bytes = words << 2;
			const char *p, *pe, *v = data;

			for(p=v, pe=v+bytes; *p && p!=pe; p++)
				; /* seek end-of-string */

			if (!script_string_entry_new(s, entry->name, p-v, v))
				goto malloc_error;
			}; break;
		case SCRIPT_VALUE_TYPE_GPIO: {
			struct script_bin_gpio_value *gpio = data;
			int32_t v[4];
			if (words != 6) {
				pr_err("%s: %s.%s: invalid length %d (assuming %d)\n",
				       filename, section->name, entry->name, words, 6);
			} else if (gpio->port == 0xffff) {
				; /* port:power */
			} else if (gpio->port < 1 || gpio->port > GPIO_BANK_MAX) {
				pr_err("%s: %s.%s: unknown GPIO port bank ",
				       filename, section->name, entry->name);
				char c = 'A' + gpio->port - 1;
				if (c >= 'A' && c <= 'Z')
					pr_err("%c ", c);
				pr_err("(%u)\n", gpio->port);
				goto failure;
			}
			v[0] = gpio->mul_sel;
			v[1] = gpio->pull;
			v[2] = gpio->drv_level;
			v[3] = gpio->data;

			if (!script_gpio_entry_new(s, entry->name,
						   gpio->port, gpio->port_num,
						   v))
				goto malloc_error;
			}; break;
		case SCRIPT_VALUE_TYPE_NULL:
			if (!*entry->name) {
				pr_err("%s: empty entry in section: %s\n", filename, section->name);
			} else if (!script_null_entry_new(s, entry->name)) {
				goto malloc_error;
			}
			break;
		default:
			pr_err("%s: %s.%s: unknown type %d\n",
			       filename, section->name, entry->name, type);
			goto failure;
		}
	}
	return 1;

malloc_error:
	pr_err("%s: %s\n", "malloc", strerror(errno));
failure:
	return 0;
}

#define SCRIPT_BIN_VERSION_LIMIT 0x10
#define SCRIPT_BIN_SECTION_LIMIT 0x100

int script_decompile_bin(void *bin, size_t bin_size,
			 const char *filename,
			 struct script *script)
{
	unsigned int i;
	struct script_bin_head *head = bin;

	if (((head->version[0] & 0x3FFF) > SCRIPT_BIN_VERSION_LIMIT) ||
	    (head->version[1] > SCRIPT_BIN_VERSION_LIMIT) ||
	    (head->version[2] > SCRIPT_BIN_VERSION_LIMIT)) {
		pr_err("Malformed data: version %u.%u.%u.\n",
		       head->version[0], head->version[1], head->version[2]);
		return 0;
	}

	if (head->sections > SCRIPT_BIN_SECTION_LIMIT) {
		pr_err("Malformed data: too many sections (%u).\n",
		       head->sections);
		return 0;
	}

	pr_info("%s: version: %u.%u.%u\n", filename,
		head->version[0] & 0x3FFF, head->version[1], head->version[2]);
	pr_info("%s: size: %zu (%u sections)\n", filename,
		bin_size, head->sections);

	/* TODO: SANITY: compare head.sections with bin_size */
	for (i=0; i < head->sections; i++) {
		struct script_bin_section *section = &head->section[i];

		if (!decompile_section(bin, bin_size, filename,
				       section, script))
			return 0;
	}
	return 1;
}
