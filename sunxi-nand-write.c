/*
 * sunxi-tools/sunxi-nand-write.c
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
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>

#include "sunxi-nand-common.h"


/**
 * \fn void mbr_initialize(SUNXI_NAND *ni, void **mbr)
 * \brief Initialize a new MBR record.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to an empty MBR record.
 * \return none.
 */
void mbr_initialize(SUNXI_NAND *ni, void **mbr)
{
    for(unsigned int i = 0; i < ni -> mbr_copy; i++) {
        ni -> mbr_set(mbr[i], SUNXI_MBR_VERSION, (void **)(intptr_t)ni -> mbr_version);
        ni -> mbr_set(mbr[i], SUNXI_MBR_MAGIC, (void **)(intptr_t)ni -> mbr_magic);
        ni -> mbr_set(mbr[i], SUNXI_MBR_COPY, (void **)(intptr_t)ni -> mbr_copy);
        ni -> mbr_set(mbr[i], SUNXI_MBR_INDEX, (void **)(intptr_t)i);
    }
}

/**
 * \fn void mbr_part_count_set(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int count)
 * \brief Update the partition count of one MBR record
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param mbr_id Pointer to the MBR record.
 * \param count Partition count of this MBR.
 * \return none.
 */
void mbr_part_count_set(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int count)
{
    ni -> mbr_set(mbr[mbr_id], SUNXI_MBR_PARTCOUNT, (void **)(intptr_t)count);
}

/*
 * \fn void void mbr_part_count_set_all(SUNXI_NAND *ni, void **mbr, unsigned int count)
 * \brief Update the partition count of every MBR record
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param count Partition count of the MBRs.
 * \return none.
 */
void mbr_part_count_set_all(SUNXI_NAND *ni, void **mbr, unsigned int count)
{
    for(unsigned int i = 0; i < ni -> mbr_copy; i++) {
        mbr_part_count_set(ni, mbr, i, count);
    }
}

/**
 * \fn void mbr_part_set(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int part_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype)
 * \brief Modify a partition to one of the copies of the MBR.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param mbr_id MBR record to update.
 * \param part_id Partition id to update.
 * \param addlo Start position of the partition.
 * \param lenlo Length of the partition.
 * \param classname Class of the partition.
 * \param name Name of the partition.
 * \param usertype Type of the partition.
 * \return none.
 */
void mbr_part_set(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int part_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype)
{
    if(mbr_id < ni -> mbr_copy ) {
        ni -> part_set(mbr[mbr_id], part_id, SUNXI_PART_ADDRLO, (void **)(intptr_t)addrlo);
        ni -> part_set(mbr[mbr_id], part_id, SUNXI_PART_LENLO, (void **)(intptr_t)lenlo);
        if(classname != NULL) {
            ni -> part_set(mbr[mbr_id], part_id, SUNXI_PART_CLASSNAME, (void **)classname);
        } else {
            ni -> part_set(mbr[mbr_id], part_id, SUNXI_PART_CLASSNAME, (void **)"");
        }
        if(name != NULL) {
            ni -> part_set(mbr[mbr_id], part_id, SUNXI_PART_NAME, (void **)name);
        } else {
            ni -> part_set(mbr[mbr_id], part_id, SUNXI_PART_NAME, (void **)"");
        }
        ni -> part_set(mbr[mbr_id], part_id, SUNXI_PART_USERTYPE, (void **)(intptr_t)usertype);
    }
}

/**
 * \fn void mbr_part_set_all(SUNXI_NAND *ni, void **mbr, unsigned int part_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype)
 * \brief Modify a partition to every copies of the MBR.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param part_id Partition id to update.
 * \param addlo Start position of the partition.
 * \param lenlo Length of the partition.
 * \param classname Class of the partition.
 * \param name Name of the partition.
 * \param usertype Type of the partition.
 * \return none.
 */
void mbr_part_set_all(SUNXI_NAND *ni, void **mbr, unsigned int part_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype)
{
    for(unsigned int i = 0; i < ni -> mbr_copy; i++) {
        mbr_part_set(ni, mbr, i, part_id, addrlo, lenlo, classname, name, usertype);
    }
}

/**
 * \fn void mbr_part_add(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype) {
 * \brief Add a partition to one of the copies of the MBR.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param mbr_id MBR record to update.
 * \param addlo Start position of the partition.
 * \param lenlo Length of the partition.
 * \param classname Class of the partition.
 * \param name Name of the partition.
 * \param usertype Type of the partition.
 * \return none.
 */
void mbr_part_add(SUNXI_NAND *ni, void **mbr, unsigned int mbr_id, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype)
{
    if(mbr_id < ni -> mbr_copy ) {
        int count = *(int *)ni->mbr_get(mbr[mbr_id], SUNXI_MBR_PARTCOUNT);
        mbr_part_set(ni, mbr, mbr_id, count, addrlo, lenlo, classname, name, usertype);
        mbr_part_count_set(ni, mbr, mbr_id, count + 1 );
    }
}

/**
 * \fn void mbr_part_add_all(SUNXI_NAND *ni, void **mbr, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype)
 * \brief Add a partition to every copies of the MBR.
 *
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param addlo Start position of the partition.
 * \param lenlo Length of the partition.
 * \param classname Class of the partition.
 * \param name Name of the partition.
 * \param usertype Type of the partition.
 * \return none.
 */
void mbr_part_add_all(SUNXI_NAND *ni, void **mbr, unsigned int addrlo, unsigned int lenlo, char *classname, char *name, unsigned int usertype)
{
    for(unsigned int i = 0; i < ni -> mbr_copy; i++) {
        mbr_part_add(ni, mbr, i, addrlo, lenlo, classname, name, usertype);
    }
}

/**
 * \fn  ssize_t mbr_write_df(int fd, SUNXI_NAND *ni, void **mbr, unsigned int mbr_id)
 * \brief Write a MBR to a file descriptor.
 *
 * \param fd File descriptor for the file / device to write to.
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param mbr_id MBR record to write.
 * \return -1 if an error occured or the number of bytes written.
 */
ssize_t mbr_write_fd(int fd, SUNXI_NAND *ni, void **mbr, unsigned int mbr_id)
{
    if(lseek(fd,  ni -> mbr_start + (ni -> mbr_size * mbr_id), SEEK_SET) != -1) {
        return(write(fd, mbr[mbr_id], ni -> mbr_size));
    }
    return(-1);
}

/**
 * \fn  ssize_t mbr_write_fd_all(int fd, SUNXI_NAND *ni, void **mbr)
 * \brief Write all mbr to a file descriptor.
 *
 * \param fd File descriptor for the file / device to write to.
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \return -1 if an error occured or the number of bytes written.
 */
ssize_t mbr_write_fd_all(int fd, SUNXI_NAND *ni, void **mbr)
{
    int j = 0;

    for(unsigned int i = 0; i < ni -> mbr_copy; i++) {
        int k = mbr_write_fd(fd, ni, mbr, i);
        if( k != -1) {
            j+=k;
        } else {
            return (-1);
        }

    }

    return(j);
}

/**
 * \fn  mbr_write_file(char *file, SUNXI_NAND *ni, void **mbr, unsigned int mbr_id)
 * \brief Write a mbr to a file.
 *
 * \param file File to write the MBR.
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \param mbr_id MBR record to write.
 * \return -1 if an error occured or the number of bytes written.
 */
ssize_t mbr_write_file(char *file, SUNXI_NAND *ni, void **mbr, unsigned int mbr_id)
{
    int f;
    ssize_t j;

    f = open(file, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    if (f < 0) {
        return(-1);
    }

    j = mbr_write_fd(f, ni, mbr, mbr_id);

    close(f);

    return(j);
}

/**
 * \fn  mbr_write_file(char *file, SUNXI_NAND *ni, void **mbr, unsigned int mbr_id)
 * \brief Write all mbr to a file.
 *
 * \param file File to write the MBR.
 * \param ni Pointer to the SUNXI_NAND definition of the board.
 * \param mbr Pointer to the MBR record.
 * \return -1 if an error occured or the number of bytes written.
 */
ssize_t mbr_write_file_all(char *file, SUNXI_NAND *ni, void **mbr)
{
    int f;
    ssize_t j;

    f = open(file, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    if (f < 0) {
        return(-1);
    }

    j = mbr_write_fd_all(f, ni, mbr);

    close(f);

    return(j);
}

