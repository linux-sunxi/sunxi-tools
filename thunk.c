/*
 * Copyright (C) 2017 Icenowy Zheng <icenowy@aosc.io>
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
#include "thunk.h"

#include <stddef.h>

static uint32_t fel_to_spl_thunk_code_v7[] = {
	#include "thunks/fel-to-spl-thunk.h"
};

static thunk_t fel_to_spl_thunk_v7 = {
	.code = fel_to_spl_thunk_code_v7,
	.size = sizeof(fel_to_spl_thunk_code_v7),
};

static uint32_t fel_to_spl_thunk_code_v5[] = {
	#include "thunks/fel-to-spl-thunk-armv5.h"
};

static thunk_t fel_to_spl_thunk_v5 = {
	.code = fel_to_spl_thunk_code_v5,
	.size = sizeof(fel_to_spl_thunk_code_v5),
};

thunk_t *fel_to_spl_thunk(soc_info_t *soc_info)
{
	if (soc_info->arch_version >= 7)
		return &fel_to_spl_thunk_v7;
	if (soc_info->arch_version == 5)
		return &fel_to_spl_thunk_v5;
	return NULL;
}
