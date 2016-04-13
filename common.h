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
#ifndef _SUNXI_TOOLS_COMMON_H
#define _SUNXI_TOOLS_COMMON_H

#include <stddef.h> /* offsetof */

/** flag function argument as unused */
#ifdef UNUSED
#elif defined(__GNUC__)
#	define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#else
#	define UNUSED(x) UNUSED_ ## x
#endif

/** finds the parent of an struct member */
#ifndef container_of
#define container_of(P,T,M)	(T *)((char *)(P) - offsetof(T, M))
#endif

/** calculate number of elements of an array */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(A)		(sizeof(A)/sizeof((A)[0]))
#endif

/** shortcut to printf to stderr */
#define errf(...)	fprintf(stderr, __VA_ARGS__)

#endif /* _SUNXI_TOOLS_COMMON_H */
