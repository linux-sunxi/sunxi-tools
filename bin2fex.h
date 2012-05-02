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
#ifndef _SUNXI_TOOLS_BIN2FEX_H
#define _SUNXI_TOOLS_BIN2FEX_H

#include <stdint.h>

struct script_head {
	int32_t sections;
	int32_t version[3];
};

struct script_section {
	char name[32];
	int32_t length;
	int32_t offset;
};

struct script_section_entry {
	char name[32];
	int32_t offset;
	int32_t pattern;
};

#endif
