/*
 * Copyright (C) 2012 Alejandro Mery <amery@geeks.cl>
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "script.h"
#include "script_uboot.h"

#define pr_info(...)	errf("fexc-uboot: " __VA_ARGS__)
#define pr_err(...)	errf("E: fexc-uboot: " __VA_ARGS__)

#ifdef DEBUG
#define pr_debug(...)	errf("D: fexc-uboot: " __VA_ARGS__)
#else
#define pr_debug(...)
#endif

static inline void out_dram_member(FILE *out, const char *key, uint32_t val)
{
	const char *fmt;
	if (strncmp(key, "tpr", 3) == 0 ||
	    strncmp(key, "emr", 3) == 0)
		fmt = "\t.%s = %#x,\n";
	else
		fmt = "\t.%s = %u,\n";

	fprintf(out, fmt, key, val);
}

static int generate_dram_struct(FILE *out, struct script_section *sp)
{
	struct list_entry *le;
	struct script_entry *ep;
	struct script_single_entry *val;
	const char *key;
	int ret = 1;

	fprintf(out, "struct dram_para para = {\n");
	for (le = list_first(&sp->entries); le;
	     le = list_next(&sp->entries, le)) {
		ep = container_of(le, struct script_entry, entries);

		if (strncmp(ep->name, "dram_", 5) != 0)
			goto invalid_field;

		key = ep->name + 5;
		if (strcmp(key, "") == 0)
			goto invalid_field;
		else if (strcmp(key, "baseaddr") == 0)
			continue; /* skip */
		else if (strcmp(key, "clk") == 0)
			key = "clock";

		switch (ep->type) {
		case SCRIPT_VALUE_TYPE_SINGLE_WORD:
			val = container_of(ep, struct script_single_entry, entry);
			if (val->value > 0)
				out_dram_member(out, key, val->value);
			/* pass through */
		case SCRIPT_VALUE_TYPE_NULL:
			continue;
		default:
invalid_field:
			pr_err("dram_para: %s: invalid field\n", ep->name);
			ret = 0;
		}

	}
	fprintf(out, "};\n");
	return ret;
}

int script_generate_uboot(FILE *out, const char *UNUSED(filename),
			  struct script *script)
{
	struct script_section *section;
	const char *section_name;

	fprintf(out, "/* this file is generated, don't edit it yourself */\n\n");

	section_name = "dram_para";
	section = script_find_section(script, section_name);
	if (!section)
		goto missing_section;
	generate_dram_struct(out, section);

	return 1;
missing_section:
	pr_err("%s: critical section missing", section_name);
	return 0;
}
