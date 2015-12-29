/*
 * sunxi-tools/sunxi-nand-sun4i.c
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

#include <string.h>
#include <stdint.h>
#include "sunxi-nand-sun4i.h"
#include "sunxi-nand-common.h"

/**
 * \fn int sun4i_mbr_check(void *_mbr, int test)
 * \brief Validate fields in the MBR
 *
 * \param _mbr pointer to the copy of the MBR to validate
 * \param test What test to do.
 * \return 0 if the test fail, 1 if success.
 */
int sun4i_mbr_check(void *_mbr, int test)
{
    SUN4I_MBR *mbr = (SUN4I_MBR *)_mbr;

    switch(test) {
    case MBR_CHECK_MAGIC:
        if(!strncmp((char *)mbr -> magic, SUN4I_MBR_MAGIC, strlen(SUN4I_MBR_MAGIC)))
            return(1);
        break;
    case MBR_CHECK_VERSION:
        if(mbr -> version == SUN4I_MBR_VERSION)
            return(1);
        break;
    case MBR_CHECK_CRC32:
        if(*(unsigned int *)mbr == calc_crc32(mbr, SUN4I_MBR_SIZE))
            return(1);
        break;
    }
    return(0);
}


/**
 * \fn void *sun4i_mbr_get(void *_mbr, int info)
 * \brief Retreive information in a MBR record.
 *
 * \param _mbr pointer to the copy of the MBR to validate
 * \param info What information should be retreived.
 * \return a void pointer to the requested information.
 */
void *sun4i_mbr_get(void *_mbr, int info)
{
    SUN4I_MBR *mbr = (SUN4I_MBR *)_mbr;

    switch(info) {
    case SUNXI_MBR_CRC32:
        return(&mbr -> crc32);
        break;
    case SUNXI_MBR_VERSION:
        return(&mbr -> version);
        break;
    case SUNXI_MBR_MAGIC:
        return(&mbr -> magic);
        break;
    case SUNXI_MBR_COPY:
        return(&mbr -> copy);
        break;
    case SUNXI_MBR_INDEX:
        return(&mbr -> index);
        break;
    case SUNXI_MBR_PARTCOUNT:
        return(&mbr -> PartCount);
        break;
    case SUNXI_MBR_PARTINFO:
        return(&mbr -> array);
        break;
    case SUNXI_MBR_RESERVED:
        return(&mbr -> res);
        break;
    }
    return(NULL);
}


/**
 * \fn void sun4i_mbr_set(void *_mbr, int info, void *data)
 * \brief Set data to a copy of the MBR and update its CRC32.
 *
 * \param _mbr pointer to the copy of the MBR to update.
 * \param info What information should be updated.
 * \param data Void pointer to the data.
 * \return None..
 */
void sun4i_mbr_set(void *_mbr, int info, void *data)
{
    SUN4I_MBR *mbr = (SUN4I_MBR *)_mbr;

    switch(info) {
    case SUNXI_MBR_VERSION:
        mbr -> version = (intptr_t) data;
        break;
    case SUNXI_MBR_MAGIC:
        strncpy((char *)mbr -> magic, data, SUN4I_MBR_MAGIC_LEN);
        break;
    case SUNXI_MBR_COPY:
        mbr -> copy = (intptr_t) data;
        break;
    case SUNXI_MBR_INDEX:
        mbr -> index = (intptr_t) data;
        break;
    case SUNXI_MBR_PARTCOUNT:
        mbr -> PartCount = (intptr_t) data;
        break;
    case SUNXI_MBR_RESERVED:
        memcpy(mbr -> res, data, sizeof(char) * SUN4I_MBR_RESERVED);
        break;
    }
    mbr -> crc32 = calc_crc32(mbr, SUN4I_MBR_SIZE);
}

/**
 * \fn void *sun4i_part_get(void *_mbr, int partition, int info)
 * \brief Retreive information in a partition record.
 *
 * \param _mbr pointer to the copy of the MBR where the partition is.
 * \param partition ID of the partition.
 * \param info What information should be retreived.
 * \return a void pointer to the requested information.
 */
void *sun4i_part_get(void *_mbr, int partition, int info)
{
    SUN4I_MBR *mbr = (SUN4I_MBR *)_mbr;

    if((intptr_t)partition < mbr -> PartCount) {
        switch(info) {
        case SUNXI_PART_ADDRHI:
            return(&mbr -> array[partition].addrhi);
            break;
        case SUNXI_PART_ADDRLO:
            return(&mbr -> array[partition].addrlo);
            break;
        case SUNXI_PART_LENHI:
            return(&mbr -> array[partition].lenhi);
            break;
        case SUNXI_PART_LENLO:
            return(&mbr -> array[partition].lenlo);
            break;
        case SUNXI_PART_CLASSNAME:
            return(&mbr -> array[partition].classname);
            break;
        case SUNXI_PART_NAME:
            return(&mbr -> array[partition].name);
            break;
        case SUNXI_PART_USERTYPE:
            return(&mbr -> array[partition].user_type);
            break;
        case SUNXI_PART_RO:
            return(&mbr -> array[partition].ro);
            break;
        case SUNXI_PART_RESERVED:
            return(&mbr -> array[partition].res);
            break;
        }
    }
    return(NULL);
}

/**
 * \fn void sun4i_part_set(void *_mbr, int partition, int info, void *data)
 * \brief Set partition data to a copy of the MBR and update its CRC32.
 *
 * \param _mbr pointer to the copy of the MBR to update.
 * \param partition ID of the partition to update.
 * \param info What information should be updated.
 * \param data Void pointer to the data.
 * \return None..
 */
void sun4i_part_set(void *_mbr, int partition, int info, void *data)
{
    SUN4I_MBR *mbr = (SUN4I_MBR *)_mbr;

    if((intptr_t)partition < SUN4I_MBR_PART_COUNT) {
        switch(info) {
        case SUNXI_PART_ADDRHI:
            mbr -> array[partition].addrhi = (intptr_t) data;
            break;
        case SUNXI_PART_ADDRLO:
            mbr -> array[partition].addrlo = (intptr_t) data;
            break;
        case SUNXI_PART_LENHI:
            mbr -> array[partition].lenhi = (intptr_t) data;
            break;
        case SUNXI_PART_LENLO:
            mbr -> array[partition].lenlo = (intptr_t) data;
            break;
        case SUNXI_PART_CLASSNAME:
            strncpy((char *)mbr -> array[partition].classname, data, SUN4I_MBR_NAME_LEN);
            break;
        case SUNXI_PART_NAME:
            strncpy((char *)mbr -> array[partition].name, data, SUN4I_MBR_NAME_LEN);
            break;
        case SUNXI_PART_USERTYPE:
            mbr -> array[partition].user_type = (intptr_t) data;
            break;
        case SUNXI_PART_RO:
            mbr -> array[partition].ro = (intptr_t) data;
            break;
        case SUNXI_PART_RESERVED:
            memcpy(mbr -> array[partition].res, data, sizeof(char) * SUN4I_MBR_PART_RES);
            break;
        }
    }
    mbr -> crc32 = calc_crc32(mbr, SUN4I_MBR_SIZE);
}

/**
 * \fn void sun4i_part_copy(void *_mbr, int dst, int src)
 * \brief Copy a partition to another slot.
 *
 * \param _mbr pointer to the copy of the MBR to update.
 * \param dst ID of the partition where to copy the data.
 * \param src ID of the partition with the data to copy.
 * \return None..
 */
void sun4i_part_copy(void *_mbr, int dst, int src)
{
    SUN4I_MBR *mbr = (SUN4I_MBR *)_mbr;
    if( ((intptr_t)dst < SUN4I_MBR_PART_COUNT) && ((intptr_t)src < SUN4I_MBR_PART_COUNT)  ) {
        mbr -> array[dst].addrhi = mbr -> array[src].addrhi;
        mbr -> array[dst].addrlo = mbr -> array[src].addrlo;
        mbr -> array[dst].lenhi = mbr -> array[src].lenhi;
        mbr -> array[dst].lenlo = mbr -> array[src].lenlo;
        memcpy(mbr -> array[dst].classname, mbr -> array[src].classname, sizeof(char) * SUN4I_MBR_NAME_LEN);
        memcpy(mbr -> array[dst].name, mbr -> array[src].name, sizeof(char) * SUN4I_MBR_NAME_LEN);
        mbr -> array[dst].user_type = mbr -> array[src].user_type;
        mbr -> array[dst].ro = mbr -> array[src].ro;
        memcpy(mbr -> array[dst].res, mbr -> array[src].res, sizeof(char) * SUN4I_MBR_PART_RES);
        mbr -> crc32 = calc_crc32(mbr, SUN4I_MBR_SIZE);
    }
}
