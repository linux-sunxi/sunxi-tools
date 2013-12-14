#include <copyright.h>
#include <config.h>
#include <i18n.h>
#include <string.h>

int strcntchar(const char * s, char c)
{
        for (int i = 0; ; ++s, ++i)
        {
                if (!(s = strchr(s, c)))
                        return i;
        }
}

char * sgettext (const char *msgid)
{
        char *msgval = gettext(msgid);
        int pipeCount = strcntchar(msgid, '|');
        if (pipeCount && pipeCount == strcntchar(msgval, '|'))
                msgval = strchr(msgval, '|') + 1;
        return msgval;
}
