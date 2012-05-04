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
#ifndef _SUNXI_TOOLS_H
#define _SUNXI_TOOLS_H

/** flat function argument as unused */
#ifdef UNUSED
#elif defined(__GNUC__)
#	define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#else
#	define UNUSED(x) UNUSED_ ## x
#endif

/** shortcut to printf to stderr */
#define errf(...)	fprintf(stderr, __VA_ARGS__)

/** a list hook */
struct list_entry {
	struct list_entry *prev;
	struct list_entry *next;
};

/** initialize an empty list hook */
static inline void list_init(struct list_entry *self)
{
	self->prev = self->next = self;
}

/** append a list hook @l1 at the end of the list @l0 */
static inline void list_append(struct list_entry *l0, struct list_entry *l1)
{
	l1->next = l0;
	l1->prev = l0->prev;
	l0->prev = l1;
}

/** is list empty? */
static inline int list_empty(struct list_entry *l)
{
	return (l->prev == l);
}

#endif
