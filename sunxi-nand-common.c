/*
 * sunxi-tools/sunxi-nand-common.c
 *
 * (C) Copyright 2015
 * Eddy Beaupre <eddy@beaupre.biz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <error.h>
#include <errno.h>

#include "sunxi-nand.h"
#include "sunxi-nand-common.h"

/**
 * \fn void *voidfree(void **ptr)
 * \brief Check if a void pointer was allocated and free it
 *
 * \param prt Pointer that was allocated.
 * \return The freed pointer (NULL);
 */
void *voidfree(void **ptr)
{
    if(*ptr) {
        free(*ptr);
        *ptr = NULL;
    }

    return(*ptr);
}

/**
 * \fn char *strfree(char **ptr)
 * \brief Check if a char pointer was allocated and free it
 *
 * \param prt Pointer that was allocated.
 * \return The freed pointer (NULL)
 */
char *strfree(char **ptr)
{
    if(*ptr) {
        free(*ptr);
        *ptr = NULL;
    }

    return(*ptr);
}

/**
 * \fn char *strduplicate(char **dest, const char *src)
 * \brief Duplicate a string, dealocated the memory of dest first if it was allocated.
 *
 * \param dest Destination pointer.
 * \param src Source pointer.
 * \return Pointer to the duplicated string.
 */
char *strduplicate(char **dest, const char *src)
{
    strfree(dest);

    if(src) {
        *dest = strdup(src);
    }

    return(*dest);
}

/**
 * \fn SUNXI_NAND *sunxi_nand_add(SUNXI_NAND **ni, char *type, char *magic, int version, int partitions, int copy, int len, int start, int size, void *mbr_check, void *mbr_get, void *mbr_set, void *part_get, void *part_set, int reserved)
 * \brief Add a SUNXI MBR definition to the MBR definition list.
 *
 * \param ni Current MBR definition list.
 * \param type Name of the MBR definition.
 * \param magic Magic ID
 * \param version Version ID
 * \param partitions Maximum number of supported partitions
 * \param copy Number of MBR copies.
 * \param len Maximum length of a device/partition name
 * \param size Size of the MBR
 * \param mbr_check Pointer to a function that validate MBR data.
 * \param mbr_get Pointer to a function to retreive MBR data.
 * \param mbr_set Pointer to a function to set MBR data.
 * \param part_get Pointer to a function to retreive partition data.
 * \param part_set Pointer to a function to set partition data.
 * \param reserved Size of the reserved MBR data.
 * \return Pointer to the updated definition list.
 */
SUNXI_NAND *sunxi_nand_add(SUNXI_NAND **ni, char *type, char *magic, int version, int partitions, int copy, int len, int start, int size, void *mbr_check, void *mbr_get, void *mbr_set, void *part_get, void *part_set, void *part_copy, int reserved)
{
    SUNXI_NAND *n;

    n = (SUNXI_NAND *) malloc_clean( sizeof(SUNXI_NAND) );

    n -> mbr_type = (unsigned char *) strdup(type);
    n -> mbr_magic = (unsigned char *) strdup(magic);
    n -> mbr_version = version;
    n -> mbr_max_partitions = partitions;
    n -> mbr_copy = copy;
    n -> mbr_name_len = len;
    n -> mbr_start = start;
    n -> mbr_size = size;
    n -> mbr_reserved = reserved;
    n -> mbr_check = mbr_check;
    n -> mbr_get = mbr_get;
    n -> mbr_set = mbr_set;
    n -> part_get = part_get;
    n -> part_set = part_set;
    n -> part_copy = part_copy;
    n -> next = *ni;
    *ni = n;

    return(*ni);
}

/**
 * \fn SUNXI_NAND *sunxi_nand_free(SUNXI_NAND *ni)
 * \brief Dealocate all SUNXI_NAND list.
 *
 * \param ni Pointer to the definition list.
 * \return Pointer to the empty SUNXI_NAND list.
 */
SUNXI_NAND *sunxi_nand_free(SUNXI_NAND **ni)
{
    SUNXI_NAND *n;
    while((*ni) != NULL) {
        n = (*ni) -> next;
        free((*ni) -> mbr_type);
        free((*ni) -> mbr_magic);
        free(*ni);
        (*ni) = n;
    }

    return(*ni);
}

/**
 * \fn SUNXI_PARTS *sunxi_parts_add(SUNXI_PARTS **sp, long id, int del, unsigned char *classname, unsigned char *name, long start, long len, long type, long ro) {
 * \brief Allocate memory and add an item to a SUNXI_PARTS linked list
 *
 * \param ni Pointer to the linked list
 * \param id partition id.
 * \param del delete partition.
 * \param name partition name.
 * \param start partition start.
 * \param len partition length.
 * \param type partition type.
 * \return Pointer to an empty SUNXI_PARTS table.
 */
SUNXI_PARTS *sunxi_parts_add(SUNXI_PARTS **sp, int cmd, long id, unsigned char *classname, unsigned char *name, long start, long len, long type, long ro)
{
    SUNXI_PARTS *s = (SUNXI_PARTS *) malloc_clean(sizeof(SUNXI_NAND));
    SUNXI_PARTS *s1 = *(sp);

    s -> cmd = cmd;
    s -> id = id;
    s -> classname = classname;
    s -> name = name;
    s -> start= start;
    s -> len = len;
    s -> type = type;
    s -> ro = ro;
    s -> next = *sp;
    if(s1) {
        s1 -> prev = s;
    }
    s -> prev = NULL;
    *sp = s;

    return(*sp);
}

/**
 * \fn void sunxi_parts_free(SUNXI_PARTS *part)
 * \brief Dealocate memory of a SUNXI_PARTS linked list.
 *
 * \param sp Pointer to the linked list.
 * \return Pointer to the SUNXI_PARTS linked list.
 */
SUNXI_PARTS *sunxi_parts_free(SUNXI_PARTS **sp)
{
    SUNXI_PARTS *s;

    while((*sp) != NULL) {
        s = (*sp) -> next;
        if((*sp) -> name) {
            free((*sp) -> name);
        }
        if((*sp) -> classname) {
            free((*sp) -> classname);
        }
        free(*sp);
        (*sp) = s;
    }

    return(*sp);


}

/**
 * \fn printNandInfo(SUNXI_NAND *ni)
 * \brief Print all definitition lists.
 *
 * \param ni Pointer to the definition list.
 * \return None
 */

void printNandInfo(SUNXI_NAND *ni)
{
    printf("                        Type : %s\n", ni -> mbr_type);
    printf("                    Magic ID : %s\n", ni -> mbr_magic);
    printf("                     Version : %#x\n", ni -> mbr_version);
    printf("Maximum number of partitions : %d\n", ni -> mbr_max_partitions);
    printf("                      Copies : %d\n", ni -> mbr_copy);
    printf("  Maximum Device name length : %d\n", ni -> mbr_name_len);
    printf("                Start offset : %d\n", ni -> mbr_start);
    printf("                        Size : %d\n", ni -> mbr_size);
    printf("              Reserved bytes : %d\n\n", ni -> mbr_reserved);
}

/**
 * \fn void usage(char *program)
 * \brief Print program usage and exit.
 *
 * \param program Program name
 * \return None
 */
void usage(char *program)
{
    printf("usage: %s [options]\n", program);
    printf("Options:\n");
    printf("  --help                   Display this information\n\n");
    printf("  --type=<TYPE>            Specify device type\n");
    printf("  --device                 Specify NAND device (default: /dev/nand)\n\n");
    printf("  --clean                  Create an empty MBR\n");
    printf("  --backup=<FILE>          Backup MBR to FILE\n");
    printf("  --restore=<FILE>         Restore MBR to FILE\n\n");
    printf("  --partition=<OPTIONS>    Modify a partition, <OPTIONS> are a comma-separated\n");
    printf("                           list made of the following items:\n");
    printf("    add=<PARAMS>           Add, a partition\n");
    printf("    insert=<ID>,<PARAMS>   Insert a partition at <ID>\n");
    printf("    delete=<ID>            Delete partition <ID>\n\n");
    printf("Parameters for add/insert:\n");
    printf("    start=<BLOCK>          Starting block of the partition\n");
    printf("    length=<BLOCKS>        Length of the partition in bocks (-1, use wathever is left)\n");
    printf("    class=<CLASS>          Class ID of the partition (default: DISK)\n");
    printf("    name=<NAME>            Name of the partition\n");
    printf("    ro                     Partition should be flagged Read-Only\n");
}

/**
 * \fn void **mbr_alloc(SUNXI_NAND *ni)
 * \brief Allocate memory to hold a complete MBR record.
 *
 * \param ni Pointer to the board definition.
 * \return Pointer to an empty MBR record.
 */
void **mbr_alloc(SUNXI_NAND *ni)
{
    void **mbr = malloc_clean( sizeof(void *) * ni -> mbr_copy);

    for(unsigned int i = 0; i < ni -> mbr_copy; i++) {
        mbr[i] = malloc_clean( ni -> mbr_size);
    }

    return(mbr);
}

/**
 * \fn void mbr_free(SUNXI_NAND *ni, void **mbr)
 * \brief Dealocate memory of a MBR record.
 *
 * \param ni Pointer to the board definition.
 * \param mbr Pointer to the MBR record.
 * \return None
 */
void mbr_free(SUNXI_NAND *ni, void **mbr)
{
    if(mbr) {
        for(int i = ni -> mbr_copy -1; i >= 0; i--) {
            if(mbr[i]) {
                free(mbr[i]);
            }
        }
        free(mbr);
    }
}


/**
 * \fn void mbr_compare(SUNXI_NAND *ni, void **mbr1, void **mbr2)
 * \brief Compare two MBRs copies.
 *
 * \param ni Pointer to the board definition.
 * \param mbr1 Pointer to the first MBR record.
 * \param mbr2 Pointer to the second MBR record.
 * \return 1 if match, 0 if it doesn't.
 */
int mbr_compare(SUNXI_NAND *ni, void **mbr1, void **mbr2)
{
    for(unsigned int i = 0; i < ni -> mbr_copy; i++) {
        if(calc_crc32(mbr1[i], ni -> mbr_size) != calc_crc32(mbr2[i], ni -> mbr_size)) {
            return(0);
        }
    }
    return(1);
}

static SUNXI_CRC32 crc32;

/**
 * \fn void init_crc32(void)
 * \brief Initialize the CRC32 table.
 *
 * \return None
 */

void init_crc32(void)
{
    crc32.CRC = 0;

    for(unsigned int i = 0; i < 256; ++i) {
        crc32.CRC = i;
        for(unsigned int j = 0; j < 8 ; ++j) {
            if(crc32.CRC & 1)
                crc32.CRC = (crc32.CRC >> 1) ^ 0xEDB88320;
            else
                crc32.CRC >>= 1;
        }
        crc32.CRC_32_Tbl[i] = crc32.CRC;
    }
}

/**
 * \fn unsigned int calc_crc32(void *buffer, unsigned int length)
 * \brief Calculate the CRC32 of a MBR
 *
 * \param mbr Pointer to the MBR record
 * \param size Size of the MBR record
 * \return CRC32 of the MBR Record.
 */
unsigned int calc_crc32(void *mbr, unsigned int size)
{
    unsigned int CRC32 = 0xffffffff;

    for( unsigned int i = sizeof(unsigned int); i < size; i++) {
        CRC32 = crc32.CRC_32_Tbl[(CRC32^((unsigned char *)mbr)[i]) & 0xff] ^ (CRC32>>8);
    }

    return CRC32^0xffffffff;
}

/**
 * \fn void *malloc_clean(size_t size)
 * \brief Allocate a buffer and clean it
 *
 * \param size Size of the buffer
 * \return pointer to the buffer.
 */
void *malloc_clean(size_t size)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return(ptr);
}

/**
 * \fn char *repeatchar(int c, int len)
 * \brief Return a pointer to a string of repeated characters.
 *
 * \param c character to repeat
 * \param len Length of the string
 * \return pointer to the buffer containing the resulting string.
 */
char *repeatchar(int c, int len)
{
    char *p = malloc_clean(len + 1);
    memset(p, c, len);
    return(p);
}

char *strchrnull(char *s, int c) {
    char *s1;
    s1 = strchr(s, c);
    if(s1 == NULL) {
        s1 = s + strlen(s);
    }
    return(s1);
}

/**
 * \fn void underline(const char *s, int c)
 * \brief Print a string underlined by character c, stop at the first \n or \0
 *
 * \param s String to underline
 * \param c Character to use for underline
 * \return None
 */
void underline(char *s, int c)
{
    if(s) {
        char *p = repeatchar(c, strchrnull(s, 10) - s);
        printf("\n%s\n%s\n\n", s, p);
        strfree(&p);
    }
}

/**
 * \fn char *printf_underline(int c, const char *format, ...)
 * \brief Print a string underlined by character c, stop at the first \n or \0
 *
 * \param c character to repeat
 * \param fmt Format string
 * \param ... Parameters
 * \return None
 */

void printf_underline(int c, const char *format, ...)
{
    int size=strlen(format) + 64;
    char *p = malloc_clean(size);

    if (p != NULL) {
        va_list args;
        int n = 0;
        char *nstr = NULL;

        while (1) {
            va_start(args, format);
            n = vsnprintf(p, size, format, args);
            va_end(args);

            if (n < 0) {
                strfree(&p);
                break;
            }

            if (n < (int) size)
                break;

            size = n + 1;

            nstr = realloc(p, size);
            if (nstr == NULL) {
                strfree(&p);
                break;
            } else {
                p = nstr;
            }
        }

        underline(p, c);
        strfree(&p);
    }
}

/**
 * \fn char *sprintf_alloc(char **str, const char *format, ...)
* \brief Like sprintf, but allocate a buffer.
 *
 * \param fmt Format string
 * \param ... Parameters
 * \return pointer to the buffer containing the resulting string.
 */
char *sprintf_alloc(char **str, const char *format, ...)
{
    size_t size=strlen(format) + 64;
    char *p = *str;
    if(!p) {
        p = malloc_clean(size);
    }

    if (p != NULL) {
        va_list args;
        int n = 0;
        char *nstr = NULL;

        while (1) {
            va_start(args, format);
            n = vsnprintf(p, size, format, args);
            va_end(args);

            if (n < 0) {
                strfree(&p);
                break;
            }

            if (n < (int) size)
                break;

            size = n + 1;

            nstr = realloc(p, size);
            if (nstr == NULL) {
                strfree(&p);
                break;
            } else {
                p = nstr;
            }
        }
        *str = realloc(p, strlen(p) + 1);

        return(*str);
    }
    return(NULL);
}
