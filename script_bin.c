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

#include "sunxi-tools.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "script.h"
#include "script_bin.h"

#define pr_info(...)	fprintf(stderr, "fex2bin: " __VA_ARGS__)
#define pr_err(...)	pr_info("E: " __VA_ARGS__)

#define WORDS(S)	(((S)+(sizeof(uint32_t)-1))/(sizeof(uint32_t)))

/**
 */
size_t calculate_bin_size(struct script *script,
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
	pr_info("sections:%zu entries:%zu data:%zu/%zu -> %zu\n",
		*sections, *entries, words, words*sizeof(uint32_t),
		bin_size);
	return bin_size;
}

int generate_bin(void *bin, size_t UNUSED(bin_size), struct script *script,
		 size_t sections, size_t entries)
{
	struct script_bin_head *head;
	struct script_bin_section *section;
	struct script_bin_entry *entry;
	void *data;

	struct list_entry *ls, *le;

	pr_err("bin generation not yet implemented\n");

	head = bin;
	section = head->section;
	entry = (void*)section+sections*sizeof(*section);
	data = (void*)entry+entries*sizeof(*entry);

	pr_info("head....:%p\n", head);
	pr_info("section.:%p (offset:%zu, each:%zu)\n", section,
		(void*)section-bin,
		sizeof(*section));
	pr_info("entry...:%p (offset:%zu, each:%zu)\n", entry,
		(void*)entry-bin,
		sizeof(*entry));
	pr_info("data....:%p (offset:%zu)\n", data,
		(void*)data-bin);

	head->sections = sections;
	head->version[0] = 0;
	head->version[1] = 1;
	head->version[2] = 2;

	return 0;
}
