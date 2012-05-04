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
#ifndef _SUNXI_TOOLS_SCRIPT_H
#define _SUNXI_TOOLS_SCRIPT_H

/** head of the data tree */
struct script {
	struct list_entry sections;
};

/** head of each section */
struct script_section {
	char name[32];

	struct list_entry sections;
	struct list_entry entries;
};

/** types of values */
enum script_value_type {
	SCRIPT_VALUE_TYPE_SINGLE_WORD = 1,
	SCRIPT_VALUE_TYPE_STRING,
	SCRIPT_VALUE_TYPE_MULTI_WORD,
	SCRIPT_VALUE_TYPE_GPIO,
	SCRIPT_VALUE_TYPE_NULL,
};

/** generic entry */
struct script_entry {
	char name[32];
	enum script_value_type type;

	struct list_entry entries;
};

/** null entry */
struct script_null_entry {
	struct script_entry entry;
};

/** create a new script tree */
struct script *script_new(void);
/** deletes a tree recursively */
void script_delete(struct script *);

/** create a new section appended to a given tree */
struct script_section *script_section_append(struct script *script,
					     const char *name);
/** deletes a section recursvely and removes it from the script */
void script_section_delete(struct script_section *section);

/** deletes an entry and removes it from the section */
void script_entry_delete(struct script_entry *entry);

/** create a new empty/null entry appended to the last section of a tree */
struct script_null_entry *script_null_entry_append(struct script *script,
						   const char *name);

#endif
