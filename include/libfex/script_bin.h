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
#ifndef _SUNXI_TOOLS_SCRIPT_BIN_H
#define _SUNXI_TOOLS_SCRIPT_BIN_H

/** binary representation of the head of a section */
struct script_bin_section {
	char name[32];
	int32_t length;
	int32_t offset;
};

/** binary representation of the head of the script file */
struct script_bin_head {
	int32_t sections;
	int32_t version[3];
	struct script_bin_section section[];
};

/** binary representation of the head of an entry */
struct script_bin_entry {
	char name[32];
	int32_t offset;
	int32_t pattern;
};

/** binary representation of a GPIO */
struct script_bin_gpio_value {
	int32_t port;
	int32_t port_num;
	int32_t mul_sel;
	int32_t pull;
	int32_t drv_level;
	int32_t data;
};

size_t script_bin_size(struct script *script,
		       size_t *sections, size_t *entries);

int script_generate_bin(void *bin, size_t bin_size, struct script *script,
			size_t sections, size_t entries);
int script_decompile_bin(void *bin, size_t bin_size,
			 const char *filename,
			 struct script *script);
#endif
