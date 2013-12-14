#ifndef __SUBXI_TOOLS_SCRIPT_FEX_H__
#define __SUBXI_TOOLS_SCRIPT_FEX_H__

#include <copyright.h>

int script_parse_fex(FILE *in, const char *filename, struct script *script);
int script_generate_fex(FILE *out, const char *filename, struct script *script);

#endif /* __SUBXI_TOOLS_SCRIPT_FEX_H__ */
