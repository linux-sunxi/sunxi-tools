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

#include "common.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "script.h"
#include "script_bin.h"

#define pr_info(...)	fprintf(stderr, "fex2bin: " __VA_ARGS__)
#define pr_err(...)	pr_info("E: " __VA_ARGS__)

#define WORDS(S)	(((S)+(sizeof(uint32_t)-1))/(sizeof(uint32_t)))

/**
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
		if (c>0) {
			*sections += 1;
			*entries += c;
		}
	}

	bin_size = sizeof(struct script_bin_head) +
		(*sections)*sizeof(struct script_bin_section) +
		(*entries)*sizeof(struct script_bin_entry) +
		words*sizeof(uint32_t);
#ifdef VERBOSE
	pr_info("sections:%zu entries:%zu data:%zu/%zu -> %zu\n",
		*sections, *entries, words, words*sizeof(uint32_t),
		bin_size);
#endif
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

#ifdef VERBOSE
	pr_info("head....:%p\n", head);
	pr_info("section.:%p (offset:%zu, each:%zu)\n", section,
		(void*)section-bin,
		sizeof(*section));
	pr_info("entry...:%p (offset:%zu, each:%zu)\n", entry,
		(void*)entry-bin,
		sizeof(*entry));
	pr_info("data....:%p (offset:%zu)\n", data,
		(void*)data-bin);
#endif

	head->sections = sections;
	head->version[0] = 0;
	head->version[1] = 1;
	head->version[2] = 2;

	for (ls = list_first(&script->sections); ls;
	     ls = list_next(&script->sections, ls)) {
		struct script_section *s;
		size_t c = 0;
		s = container_of(ls, struct script_section, sections);

		/* skip empty sections */
		if (list_empty(&s->entries))
			continue;
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
#ifdef VERBOSE
			pr_info("%s.%s <%p> (type:%d, words:%d (%zu), offset:%d)\n",
				section->name, entry->name,
				entry,
				(entry->pattern>>16) & 0xffff,
				(entry->pattern>>0) & 0xffff, size,
				entry->offset);
#endif
			c++;
			entry++;
		}

		section->length = c;
#ifdef VERBOSE
		pr_info("%s <%p> (length:%d, offset:%d)\n",
			section->name, section, section->length, section->offset);
#endif

		section++;
	}
	return 1;
}

int script_decompile_bin(void *UNUSED(bin), size_t UNUSED(bin_size),
			 const char *UNUSED(filename),
			 struct script *UNUSED(script))
{
	return 0;
}
