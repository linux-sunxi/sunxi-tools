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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "script.h"

/*
 */
struct script *script_new(void)
{
	struct script *script;
	if ((script = malloc(sizeof(*script))))
		list_init(&script->sections);
	return script;
}

/*
 */
struct script_section *script_section_append(struct script *script,
					     const char *name)
{
	struct script_section *section;

	assert(script != NULL);
	assert(name != NULL);

	if ((section = malloc(sizeof(*section)))) {
		size_t l = strlen(name);
		if (l>31) /* truncate */
			l=31;
		memcpy(section->name, name, l);
		section->name[l] = '\0';

		list_init(&section->entries);
		list_append(&script->sections, &section->sections);
	}
	return section;
}

/*
 */
static inline void script_entry_append(struct script *script,
				       struct script_entry *entry,
				       enum script_value_type type,
				       const char *name)
{
	size_t l;
	struct script_section *section;

	assert(script != NULL);
	assert(!list_empty(&script->sections));
	assert(entry != NULL);
	assert(name != NULL);

	section = container_of(list_last(&script->sections),
			       struct script_section, sections);

	l = strlen(name);
	if (l>31) /* truncate */
		l=31;
	memcpy(entry->name, name, l);
	entry->name[l] = '\0';

	entry->type = type;

	list_append(&section->entries, &entry->entries);
}

struct script_null_entry *script_null_entry_append(struct script *script,
						   const char *name)
{
	struct script_null_entry *entry;

	assert(script != NULL);
	assert(!list_empty(&script->sections));
	assert(name != NULL);

	if ((entry = malloc(sizeof(*entry)))) {
		script_entry_append(script, &entry->entry,
				    SCRIPT_VALUE_TYPE_NULL, name);
	}

	return entry;
}
