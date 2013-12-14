#ifndef __SUNXI_TOOLS_FEXC_H__
#define __SUNXI_TOOLS_FEXC_H__

#include <copyright.h>

#include "common.h"
#include "script.h"
#include "script_bin.h"
#include "script_fex.h"
#include "script_uboot.h"

enum script_format {
    FEX_SCRIPT_FORMAT,
    BIN_SCRIPT_FORMAT,
    UBOOT_HEADER_FORMAT,
};

char *read_all(int, const char *, size_t *);
int script_parse(enum script_format, const char *, struct script *);
int script_generate(enum script_format, const char *, struct script *);
void app_usage(const char *, int);
int app_choose_mode(char *);
int fexc(int, char **);

#endif /* __SUNXI_TOOLS_FEXC_H__ */
