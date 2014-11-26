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

struct members {
	const char *name;
	const char *translation;
	int mode;
};
#define foreach_member(I, T) for (const struct members *I = T; \
	     I < T+ARRAY_SIZE(T); I++)

/*
 */
static inline void out_u32_member(FILE *out, const char *key, int hexa,
				  struct script_single_entry *val)
{
	const char *fmt;
	if (hexa)
		fmt = "\t.%s = %#x,\n";
	else
		fmt = "\t.%s = %u,\n";

	fprintf(out, fmt, key, val->value);
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

static inline void out_null_member(FILE *out, const char *key)
{
	fprintf(out, "\t/* %s is NULL */\n", key);
}

static inline int out_member(FILE *out, const char *key, int mode,
		      struct script_entry *ep)
{
	switch (ep->type) {
	case SCRIPT_VALUE_TYPE_SINGLE_WORD:
		out_u32_member(out, key, mode,
		       container_of(ep, struct script_single_entry, entry));
		break;
	case SCRIPT_VALUE_TYPE_NULL:
		out_null_member(out, key);
		break;
	case SCRIPT_VALUE_TYPE_GPIO:
		out_gpio_member(out, key,
			container_of(ep, struct script_gpio_entry, entry));
		break;
	default:
		return 0;
	}
	return 1;
}

/*
 * DRAM
 */
static struct members dram_members[] = {
	{ .name="dram_clock" },
	{ .name="dram_clk", .translation="clock" },
	{ .name="dram_type" },
	{ .name="dram_rank_num" },
	{ .name="dram_density" },
	{ .name="dram_chip_density", .translation="density" },
	{ .name="dram_io_width" },
	{ .name="dram_bus_width" },
	{ .name="dram_cas" },
	{ .name="dram_zq" },
	{ .name="dram_odt_en" },
	{ .name="dram_size" },
	{ .name="dram_tpr0", .mode=1 },
	{ .name="dram_tpr1", .mode=1 },
	{ .name="dram_tpr2", .mode=1 },
	{ .name="dram_tpr3", .mode=1 },
	{ .name="dram_tpr4", .mode=1 },
	{ .name="dram_tpr5", .mode=1 },
	{ .name="dram_emr1", .mode=1 },
	{ .name="dram_emr2", .mode=1 },
	{ .name="dram_emr3", .mode=1 },
};

static int generate_dram_struct(FILE *out, struct script_section *sp)
{
	struct script_entry *ep;
	const char *key;
	int ret = 1;

	fprintf(out, "static struct dram_para dram_para = {\n");
	foreach_member(mp, dram_members) {
		ep = script_find_entry(sp, mp->name);
		if (!ep)
			continue;

		key = (mp->translation) ? mp->translation : mp->name+5;
		if (!out_member(out, key, mp->mode, ep)) {
			pr_err("dram_para: %s: invalid field\n", ep->name);
			ret = 0;
		}

	}
	fprintf(out, "};\n");
	fputs("\nunsigned long sunxi_dram_init(void)\n"
	      "{\n\treturn dramc_init(&dram_para);\n}\n",
	      out);

	return ret;
}

#if 0
/*
 * PMU
 */
static struct members pmu_members[] = {
	{ .name = "pmu_used2" },
	{ .name = "pmu_para" },
	{ .name = "pmu_adpdet" },
	{ .name = "pmu_shutdown_chgcur" },
	{ .name = "pmu_shutdown_chgcur2" },
	{ .name = "pmu_pwroff_vol" },
	{ .name = "pmu_pwron_vol" },
};

static int generate_pmu_struct(FILE *out, struct script_section *target,
			       struct script_section *pmu_para)
{
	struct list_entry *le;
	struct script_section *sp;
	struct script_entry *ep;
	const char *key;
	int ret = 1;

	fputs("\nstatic struct pmu_para pmu_para = {\n", out);

	sp = target;
	for (le = list_first(&sp->entries); le;
	     le = list_next(&sp->entries, le)) {
		ep = container_of(le, struct script_entry, entries);

		if (!out_member(out, ep->name, 0, ep)) {
			pr_err("target: %s: invalid field\n", ep->name);
			ret = 0;
		}
	}

	foreach_member(mp, pmu_members) {
		ep = script_find_entry(pmu_para, mp->name);
		if (!ep)
			continue;

		key = (mp->translation) ? mp->translation : mp->name+4;
		if (!out_member(out, key, mp->mode, ep)) {
			pr_err("pmu_para: %s: invalid field\n", mp->name);
			ret = 0;
		}
	}

	fputs("};\n", out);
	fputs("\nint sunxi_pmu_init(void)\n"
	      "{\n\treturn PMU_init(&pmu_para);\n}\n",
	      out);
	return ret;

	(void) pmu_para;
}
#endif

int script_generate_uboot(FILE *out, const char *UNUSED(filename),
			  struct script *script)
{
	struct {
		const char *name;
		struct script_section *sp;
	} sections[] = {
		{ "dram_para", NULL },
#if 0
		{ "target", NULL },
		{ "pmu_para", NULL },
#endif
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
#if 0
	      "#include <asm/arch/pmu.h>\n"
#endif
	      "#include <asm/arch/dram.h>\n\n",
	      out);

	generate_dram_struct(out, sections[0].sp);
#if 0
	generate_pmu_struct(out, sections[1].sp, sections[2].sp);
#endif

	return 1;
}
