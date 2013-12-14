#ifndef __SUNXI_TOOLS_COMMON_H__
#define __SUNXI_TOOLS_COMMON_H__

#include <copyright.h>

#include <gettext.h>
#include <stddef.h> 

/** flat function argument as unused */
#ifdef UNUSED
#elif defined(__GNUC__)
#	define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#else
#	define UNUSED(x) UNUSED_ ## x
#endif

/** finds the parent of an struct member */
#ifndef container_of
#define container_of(P,T,M)		(T *)((char *)(P) - offsetof(T, M))
#endif

/** calculate number of elements of an array */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(A)			(sizeof(A)/sizeof((A)[0]))
#endif

/** shortcut to printf to stderr */
#define errf(...)			fprintf(stderr, __VA_ARGS__)

/*
 * i18n utilities
 */

#define i18n(x)				gettext(x)
#define i18nP(singular, plural, n)	ngettext(singular, plural, n)
#define i18nM(x)			x

int strcntchar(const char *, char);
char *sgettext (const char *);

/*
 * list
 */

/** a list hook */
struct list_entry {
    struct list_entry *prev;
    struct list_entry *next;
};

#define list_insert(H, E)	list_inject((E), (H), (H)->next)
#define list_append(H, E)	list_inject((E), (H)->prev, (H))

void list_init(struct list_entry *);
void list_inject(struct list_entry *, struct list_entry *, struct list_entry *);
void list_remove(struct list_entry *l);
struct list_entry *list_first(struct list_entry *);
struct list_entry *list_last(struct list_entry *);
struct list_entry *list_next(struct list_entry *, struct list_entry *);
int list_empty(struct list_entry *);

#endif /* __SUNXI_TOOLS_COMMON_H__ */
