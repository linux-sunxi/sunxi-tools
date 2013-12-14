#ifndef __I18N_H__
#define __I18N_H__

#include <config.h>
#include <gettext.h>

#define i18n(x) gettext(x)
#define i18nP(singular, plural, n) ngettext(singular, plural, n)
#define i18nM(x) x

char *sgettext (const char *msgid);
int strcntchar(const char * s, char c);

#endif /* __I18N_H__ */

