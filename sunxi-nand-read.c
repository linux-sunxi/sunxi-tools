/*
 * sunxi-tools/sunxi-nand-read.c
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

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "sunxi-nand-common.h"

/**
 * \fn ssize_t mbr_read_fd(int fd, SUNXI_NAND *ni, void **mbr, unsigned int mbr_id)
 * \brief Read one of the MBR from a file descriptor.
 *
 * \param fd File descriptor to read the MBR from.
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param mbr_id MBR record to read.
 * \return -1 if an error occured or the number of bytes written.
 */
ssize_t mbr_read_fd(int fd, SUNXI_NAND *ni, void *mbr, int mbr_id)
{
    if(mbr) {
        if(lseek(fd, ni -> mbr_start + (ni -> mbr_size * mbr_id), SEEK_SET) != -1) {
            return(read(fd, mbr, ni -> mbr_size));
        }
    }
    return(-1);
}

/**
 * \fn void **mbr_read_fd_all(int fd, SUNXI_NAND *ni)
 * \brief Allocate a MBR record and read all the MBRs from a file descriptor.
 *
 * \param fd File descriptor to read the MBR from.
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \return NULL if an error occured or a pointer to the MBRs record.
 */
void **mbr_read_fd_all(int fd, SUNXI_NAND *ni)
{
    void **mbr = mbr_alloc(ni);

    if(mbr) {
        for(unsigned int i=0; i < ni -> mbr_copy; i++) {
            if(mbr_read_fd(fd, ni, mbr[i], i) == -1) {
                mbr_free(ni, mbr);
                return(NULL);
            }
        }
        return(mbr);
    } else {
        return(NULL);
    }

}

/**
 * \fn void **mbr_copy_all(SUNXI_NAND *ni, void *mbr)
 * \brief Duplicate MBRs record.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBRs record.
 * \return NULL if an error occured or a pointer to the duplicated MBRs record.
 */
void **mbr_copy_all(SUNXI_NAND *ni, void **mbr)
{
    void **mbr2 = mbr_alloc(ni);

    if(mbr && mbr2) {
        for(unsigned int i=0; i < ni -> mbr_copy; i++) {
            memcpy(mbr2[i], mbr[i], ni -> mbr_size);
        }
        return(mbr2);
    } else {
        mbr_free(ni, mbr2);
    }

    return(mbr2);
}

/**
 * \fn void **mbr_read_file(char *file, SUNXI_NAND *ni, void *mbr, unsigned int mbr_id)
 * \brief Read one of the MBRs from a file.
 *
 * \param file File to read the MBR from.
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record
 * \param mbr_id MBR record to read.
 * \return NULL if an error occured or a pointer to the MBRs record.
 */
ssize_t mbr_read_file(char *file, SUNXI_NAND *ni, void *mbr, unsigned int mbr_id)
{
    int fd, i;

    fd = open(file, O_RDONLY);

    if( fd == -1) {
        close(fd);
        return(-1);
    }

    i = mbr_read_fd(fd, ni, mbr, mbr_id);
    close(fd);
    return(i);
}

/**
 * \fn void **mbr_read_file_all(char *file, SUNXI_NAND *ni)
 * \brief Allocate a MBR record and read all the MBRs from a file.
 *
 * \param file File to read the MBR from.
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \return NULL if an error occured or a pointer to the MBRs record.
 */
void **mbr_read_file_all(char *file, SUNXI_NAND *ni)
{
    int fd;
    void **mbr;

    fd = open(file, O_RDONLY);

    if( fd == -1) {
        return(NULL);
    }

    mbr = mbr_read_fd_all(fd, ni);
    close(fd);
    return(mbr);
}

/**
 * \fn int mbr_check(SUNXI_NAND *ni, void *mbr)
 * \brief Check if a MBR copie is good.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record
 * \return 1 if good, 0 if bad.
 */
int mbr_check(SUNXI_NAND *ni, void *mbr)
{
    if(ni->mbr_check(mbr, MBR_CHECK_MAGIC)) {
        if(ni->mbr_check(mbr, MBR_CHECK_VERSION)) {
            if(ni->mbr_check(mbr, MBR_CHECK_CRC32)) {
                return(1);
            }
        }
    }
    return(0);
}

/**
 * \fn int mbr_check_all(SUNXI_NAND *ni, void **mbr)
 * \brief Check if all MBR copies are good.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBRs record
 * \return 1 of all are good, 0 if at least one is bad.
 */
int mbr_check_all(SUNXI_NAND *ni, void **mbr)
{
    for(unsigned int id = 0; id < ni -> mbr_copy; id++) {
        if(!mbr_check(ni, mbr[id])) {
            return(0);
        }
    }
    return(1);
}

/**
 * \fn void mbr_print(SUNXI_NAND *ni, void **mbr)
 * \brief Print the MBR and partition tables.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record
 * \return none.
 */
void mbr_print(SUNXI_NAND *ni, void **mbr)
{

    if(ni && mbr) {
        printf("MBR Copy       Version         Magic ID      CRC32           Status\n");
        printf("-------------- --------------- ------------- --------------- ------\n");
        for(unsigned int id = 0; id < ni -> mbr_copy; id++) {
            //unsigned int pc = *(unsigned int *)ni->mbr_get(mbr[id], SUNXI_MBR_PARTCOUNT);    
            printf("%-8d            %#010x      %8.8s      %#010x", id, *(int *)ni->mbr_get(mbr[id], SUNXI_MBR_VERSION), (char *)ni->mbr_get(mbr[id], SUNXI_MBR_MAGIC), *(int *)ni->mbr_get(mbr[id], SUNXI_MBR_CRC32));
            if(mbr_check(ni, mbr[id])) {
                printf("     Ok\n");
            } else {
                printf("    Bad\n");
            }
        }
        
        printf("\nPartition  Class            Name             Start      Size     Type\n");
        printf("---------- ---------------- ---------------- ---------- ---------- ----\n");
        for(unsigned int i = 0; i < *(unsigned int *)ni->mbr_get(mbr[0], SUNXI_MBR_PARTCOUNT); i++) {
            printf("%-10u %-16s %-16s %10u %10u %4u\n",
                i+1,
                (char *)ni->part_get(mbr[0], i, SUNXI_PART_CLASSNAME),
                (char *)ni->part_get(mbr[0], i, SUNXI_PART_NAME),
                *(unsigned int *)ni->part_get(mbr[0], i, SUNXI_PART_ADDRLO),
                *(unsigned int *)ni->part_get(mbr[0], i, SUNXI_PART_LENLO),
                *(unsigned int *)ni->part_get(mbr[0], i, SUNXI_PART_USERTYPE));
        }
        printf("\n");
    }
}


/**
 * \fn SUNXI_NAND *mbr_type_fd(int fd, SUNXI_NAND *ni)
 * \brief Read a file descriptor and return the SUNXI board definition of its MBR
 *
 * \param fd file descriptor to read
 * \param ni Pointer to the SUNXI_NAND definitions linked list
 * \return Pointer to the SUNXI_NAND, or NULL if none found.
 */
SUNXI_NAND *mbr_type_fd(int fd, SUNXI_NAND *ni)
{
    SUNXI_NAND *n = ni;

    while(n != NULL) {
        unsigned int found = 0;
        void *mbr = malloc_clean(n -> mbr_size);

        for(unsigned int i = 0; i < n -> mbr_copy; i++) {
            mbr_read_fd(fd, n, mbr, i);
            if(n -> mbr_check(mbr, MBR_CHECK_MAGIC)) {
                found++;
            }
        }

        free(mbr);
        if(found == n -> mbr_copy) {
            return(n);
        }
        n = n -> next;
    }
    return(NULL);
}

/**
 * \fn SUNXI_NAND *mbr_type_file(char * file, SUNXI_NAND *ni)
 * \brief Read a file and return the SUNXI board definition of its MBR
 *
 * \param file file to read
 * \param ni Pointer to the SUNXI_NAND definitions linked list
 * \return Pointer to the SUNXI_NAND, or NULL if none found.
 */
SUNXI_NAND *mbr_type_file(char * file, SUNXI_NAND *ni)
{
    SUNXI_NAND *n;
    int fd;

    fd = open(file, O_RDONLY);

    if( fd == -1) {
        return(NULL);
    }

    n = mbr_type_fd(fd, ni);

    close(fd);
    return(n);
}


/**
 * \fn SUNXI_NAND *mbr_type(char *name, SUNXI_NAND *ni)
 * \brief Find the SUNXI board definition from its name
 *
 * \param name Name of the board definition
 * \param ni Pointer to the SUNXI_NAND definitions linked list
 * \return Pointer to the SUNXI_NAND, or NULL if none found.
 */
SUNXI_NAND *mbr_type(char *name, SUNXI_NAND *ni)
{
    SUNXI_NAND *n = ni;

    while(n != NULL) {
        if(strcasecmp(name, (char *)n -> mbr_type) == 0) {
            return(n);
        } else {
            n = n -> next;
        }
    }

    return(NULL);
}
