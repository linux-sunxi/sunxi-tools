#include "types.h"

extern int nand_part_a10 (int argc, char **argv, const char *cmd, int fd, int force);
extern int nand_part_a20 (int argc, char **argv, const char *cmd, int fd, int force);
extern int checkmbrs_a10 (int fd);
extern int checkmbrs_a20 (int fd);
extern void usage (const char *cmd);
extern __u32 calc_crc32(void * buffer, __u32 length);
