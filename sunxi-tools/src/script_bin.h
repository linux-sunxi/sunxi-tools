#ifndef __SUNXI_TOOLS_SCRIPT_BIN_H__
#define __SUNXI_TOOLS_SCRIPT_BIN_H__

#include <copyright.h>

/** binary representation of the head of a section */
struct script_bin_section {
    char name[32];
    int32_t length;
    int32_t offset;
};

/** binary representation of the head of the script file */
struct script_bin_head {
    int32_t sections;
    int32_t version[3];
    struct script_bin_section section[];
};

/** binary representation of the head of an entry */
struct script_bin_entry {
    char name[32];
    int32_t offset;
    int32_t pattern;
};

/** binary representation of a GPIO */
struct script_bin_gpio_value {
    int32_t port;
    int32_t port_num;
    int32_t mul_sel;
    int32_t pull;
    int32_t drv_level;
    int32_t data;
};

size_t script_bin_size(struct script *script, size_t *sections, size_t *entries);

int script_generate_bin(void *bin, size_t bin_size, struct script *script, size_t sections, size_t entries);
int script_decompile_bin(void *bin, size_t bin_size, const char *filename, struct script *script);

#endif /* __SUNXI_TOOLS_SCRIPT_BIN_H__ */

