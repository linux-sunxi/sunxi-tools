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

static inline void out_u32_member(FILE *out, const char *key, int hexa, uint32_t val)
{
	const char *fmt;
	if (hexa)
		fmt = "\t.%s = %#x,\n";
	else
		fmt = "\t.%s = %u,\n";

	fprintf(out, fmt, key, val);
}

static inline void out_gpio_member(FILE *out, const char *key,
				   struct script_gpio_entry *gpio)
{
	fprintf(out, "\t.%s = ", key);

	if (gpio->port == 0xffff)
		fprintf(out, "GPIO_AXP_CFG(%u", gpio->port_num);
	else
		fprintf(out, "GPIO_CFG(%u, %u", gpio->port, gpio->port_num);

	for (const int *p = gpio->data, *pe = p+4; p != pe; p++) {
		if (*p == -1)
			fputs(", 0xff", out);
		else
			fprintf(out, ", %u", *p);
	}

	fputs("),\n", out);
}

/*
 * DRAM
 */
static int generate_dram_struct(FILE *out, struct script_section *sp)
{
	struct list_entry *le;
	struct script_entry *ep;
	struct script_single_entry *val;
	const char *key;
	int ret = 1, hexa;

	fprintf(out, "static struct dram_para dram_para = {\n");
	for (le = list_first(&sp->entries); le;
	     le = list_next(&sp->entries, le)) {
		ep = container_of(le, struct script_entry, entries);

		if (strncmp(ep->name, "dram_", 5) != 0)
			goto invalid_field;

		key = ep->name + 5;
		if (key[0] == '\0')
			goto invalid_field;
		else if (strcmp(key, "baseaddr") == 0)
			continue; /* skip */
		else if (strcmp(key, "clk") == 0)
			key = "clock";
		else if (strcmp(key, "chip_density") == 0)
			key = "density";

		if (strncmp(key, "tpr", 3) == 0 ||
		    strncmp(key, "emr", 3) == 0)
			hexa = 1;
		else
			hexa = 0;

		switch (ep->type) {
		case SCRIPT_VALUE_TYPE_SINGLE_WORD:
			val = container_of(ep, struct script_single_entry, entry);
			if (val->value > 0)
				out_u32_member(out, key, hexa, val->value);
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
	fputs("\nint sunxi_dram_init(void)\n"
	      "{\n\treturn DRAMC_init(&dram_para);\n}\n",
	      out);

	return ret;
}

/*
 * PMU
 */
static int generate_pmu_struct(FILE *out, struct script_section *target,
			       struct script_section *pmu_para)
{
	struct list_entry *le;
	struct script_section *sp;
	struct script_entry *ep;
	struct script_single_entry *val;
	const char *key;
	int ret = 1;

	fputs("\nstatic struct pmu_para pmu_para = {\n", out);

	sp = target;
	for (le = list_first(&sp->entries); le;
	     le = list_next(&sp->entries, le)) {
		ep = container_of(le, struct script_entry, entries);

		switch (ep->type) {
		case SCRIPT_VALUE_TYPE_SINGLE_WORD:
			val = container_of(ep, struct script_single_entry, entry);
			if (val->value > 0)
				out_u32_member(out, ep->name, 0, val->value);
			/* pass through */
		case SCRIPT_VALUE_TYPE_NULL:
			continue;
		default:
			pr_err("target: %s: invalid field\n", ep->name);
			ret = 0;
		}

	}

	sp = pmu_para;
	for (le = list_first(&sp->entries); le;
	     le = list_next(&sp->entries, le)) {
		ep = container_of(le, struct script_entry, entries);

		if (strncmp(ep->name, "pmu_", 4) != 0)
			continue;
		key = ep->name+4;

		if (strcmp(key, "used2") == 0 ||
		    strcmp(key, "para") == 0 ||
		    strcmp(key, "adpdet") == 0 ||
		    strcmp(key, "shutdown_chgcur") == 0 ||
		    strcmp(key, "shutdown_chgcur2") == 0 ||
		    strcmp(key, "pwroff_vol") == 0 ||
		    strcmp(key, "pwron_vol") == 0) {

			switch(ep->type) {
			case SCRIPT_VALUE_TYPE_SINGLE_WORD:
				val = container_of(ep, struct script_single_entry, entry);
				out_u32_member(out, key, 0, val->value);
				break;
			case SCRIPT_VALUE_TYPE_NULL:
				break;
			case SCRIPT_VALUE_TYPE_GPIO:
				out_gpio_member(out, key,
					container_of(ep, struct script_gpio_entry, entry));
				break;
			default:
				pr_err("pmu_para: %s: invalid field\n", ep->name);
			}
		}
	}

	fputs("};\n", out);
	fputs("\nint sunxi_pmu_init(void)\n"
	      "{\n\treturn PMU_init(&pmu_para);\n}\n",
	      out);
	return ret;

	(void) pmu_para;
}

int script_generate_uboot(FILE *out, const char *UNUSED(filename),
			  struct script *script)
{
	struct {
		const char *name;
		struct script_section *sp;
	} sections[] = {
		{ "dram_para", NULL },
		{ "target", NULL },
		{ "pmu_para", NULL },
	};

	for (unsigned i=0; i<ARRAY_SIZE(sections); i++) {
		struct script_section *sp;

		sp = script_find_section(script, sections[i].name);
		if (sp)
			sections[i].sp = sp;
		else {
			pr_err("%s: critical section missing",
			       sections[i].name);
			return 0;
		}
	}

	fputs("/* this file is generated, don't edit it yourself */\n\n"
	      "#include <common.h>\n"
	      "#include <asm/arch/pmu.h>\n"
	      "#include <asm/arch/dram.h>\n\n",
	      out);

	generate_dram_struct(out, sections[0].sp);
	generate_pmu_struct(out, sections[1].sp, sections[2].sp);

	return 1;
}
