#include <copyright.h>
#include <config.h>
#include <gettext.h>
#include <string.h>
#include <common.h>

/*
 * i18n utilities
 */

int strcntchar(const char *s, char c) {
    int i;

    for (i = 0; ; ++s, ++i) {
        if (!(s = strchr(s, c))) {
            return i;
        }
    }
    return i;
}

char *sgettext (const char *msgid) {
    char *msgval = gettext(msgid);
    int pipeCount = strcntchar(msgid, '|');
    if (pipeCount && pipeCount == strcntchar(msgval, '|')) {
        msgval = strchr(msgval, '|') + 1;
    }
    return msgval;
}


/*
 * List utilities
 */

/** initialize an empty list hook */
void list_init(struct list_entry *self) {
    self->prev = self->next = self;
}

/** puts an entry between two other on a list */
void list_inject(struct list_entry *l, struct list_entry *prev, struct list_entry *next) {
    l->prev = prev;
    l->next = next;

    next->prev = l;
    prev->next = l;
}

/** removes an entry for the list where it's contained */
void list_remove(struct list_entry *l) {
    struct list_entry *prev = l->prev, *next = l->next;
    next->prev = prev;
    prev->next = next;
}

/** returns first element of a list */
struct list_entry *list_first(struct list_entry *l) {
    return (l->next == l) ? NULL : l->next;
}

/** returns last element of a list */
struct list_entry *list_last(struct list_entry *l) {
    return (l->prev == l) ? NULL : l->prev;
}

/** returns next element on a list */
struct list_entry *list_next(struct list_entry *l, struct list_entry *e) {
    return (e->next == l) ? NULL : e->next;
}

/** is list empty? */
int list_empty(struct list_entry *l) {
    return (l->prev == l);
}
