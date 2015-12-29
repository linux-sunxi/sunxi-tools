/*
 * sunxi-tools/sunxi-nand.c
 *
 * (C) Copyright 2015
 * Eddy Beaupre <eddy@beaupre.biz>
 *
 * Derived from work originally made by
 *
 * (C) Copyright 2013
 * Patrick H Wood, All rights reserved.
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

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <limits.h>
#include <error.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>

#include "sunxi-nand.h"
#include "sunxi-nand-common.h"
#include "sunxi-nand-read.h"
#include "sunxi-nand-write.h"
#include "sunxi-nand-sun4i.h"
#include "sunxi-nand-sun7i.h"

mode_t file_type(char *file)
{
    struct stat sb;
    if (stat(file, &sb) == 0) {
        return(sb.st_mode & S_IFMT);
    }
    perror("file_type");
    return(-1);
}

void sync_part(char *file)
{
    if(S_ISBLK(file_type(file))) {
        int fd = open(file, O_RDWR);
        if(fd == -1) {
            perror("sync_part, open");
        }
        if(ioctl(fd, BLKRRPART, NULL) == -1) {
            perror("sync_part, ioctl");
        }
        if(close(fd) == -1) {
            perror("sync_part, close");
        }
    }
}

unsigned long file_size(char *file)
{
    struct stat sb;
    if (stat(file, &sb) == 0) {
        return(sb.st_size);
    } else {
        return(UINT_MAX);
    }
}

unsigned long device_size(char *file)
{
    unsigned long numblocks = 0;
    if(S_ISBLK(file_type(file))) {
        int fd = open(file, O_RDWR);
        if(fd == -1) {
            perror("sync_part, open");
        }
        if(ioctl(fd, BLKGETSIZE, &numblocks) == -1) {
            perror("sync_part, ioctl");
        }
        if(close(fd) == -1) {
            perror("sync_part, close");
        }
    }  else {
        numblocks = file_size(file)/512;
    }
    return(numblocks);
}

int main (int argc, char **argv)
{
    SUNXI_NAND *sunxi_nand = NULL, *board_nand = NULL;
    SUNXI_PARTS *sunxi_parts = NULL;
    char *board_device = NULL;
    char *board_type = NULL;
    char *backup_file = NULL;
    char *restore_file = NULL;
    char *program_name = NULL;

    void **board_mbr;
    void **work_mbr;

    int cmd = 0;
    unsigned int nand_size = 0;

    sunxi_nand_add(&sunxi_nand, SUN4I_MBR_NAME, SUN4I_MBR_MAGIC, SUN4I_MBR_VERSION, SUN4I_MBR_PART_COUNT, SUN4I_MBR_COPY_NUM, SUN4I_MBR_NAME_LEN, SUN4I_MBR_START_ADDRESS, SUN4I_MBR_SIZE, &sun4i_mbr_check, &sun4i_mbr_get, &sun4i_mbr_set, &sun4i_part_get, &sun4i_part_set, &sun4i_part_copy, SUN4I_MBR_RESERVED);
    sunxi_nand_add(&sunxi_nand, SUN7I_MBR_NAME, SUN7I_MBR_MAGIC, SUN7I_MBR_VERSION, SUN7I_MBR_PART_COUNT, SUN7I_MBR_COPY_NUM, SUN7I_MBR_NAME_LEN, SUN7I_MBR_START_ADDRESS, SUN7I_MBR_SIZE, &sun7i_mbr_check, &sun7i_mbr_get, &sun7i_mbr_set, &sun7i_part_get, &sun7i_part_set, &sun7i_part_copy, SUN7I_MBR_RESERVED);

    strduplicate(&board_device, "/dev/nand");
    strduplicate(&backup_file, "sunxi_mbr.img");
    strduplicate(&restore_file, "sunxi_mbr.img");

    init_crc32();
    
    
    strduplicate(&program_name, basename(argv[0]));
    

    /*
     * Parse command line options
     */

    while (1) {
        int option_index = 0, opt = 0;
        static struct option long_options[] = {
            {"clean",     0,                 0, 'c' },
            {"partition", required_argument, 0, 'p' },
            {"device",    required_argument, 0, 'd' },
            {"backup",    optional_argument, 0, 'b' },
            {"restore",   optional_argument, 0, 'r' },
            {"type",      required_argument, 0, 't' },
            {"force",     0,                 0, 'f' },
            {"help",      0,                 0, 'h' },
            {0,           0,                 0,  0 }
        };

        opt = getopt_long(argc, argv, "?fchr:b:d:p:t:", long_options, &option_index);

        if (opt == -1) {
            break;
        }

        switch (opt) {
        case 'c':
            cmd |= CMD_CLEAN;
            break;
        case 'p': {
            cmd |= CMD_PART;
            int errfnd = 0;
            char *subopts = optarg;
            char *value = NULL;
            char *const opt_list[] = {
                [OPT_START] = "start",
                [OPT_LEN] = "length",
                [OPT_CLASS] = "class",
                [OPT_NAME] = "name",
                [OPT_TYPE] = "type",
                [OPT_RO] = "ro",
                [OPT_ADD] = "add",
                [OPT_DELETE] = "delete",
                [OPT_INSERT] = "insert",
                NULL
            };

            long opt_id = LONG_MIN;
            int opt_cmd = 0;
            long opt_start = LONG_MIN;
            long opt_len = LONG_MIN;
            long opt_type = LONG_MIN;
            long opt_ro = LONG_MIN;
            unsigned char *opt_class = NULL;
            unsigned char *opt_name = NULL;


            while (*subopts != '\0' && !errfnd) {
                switch (getsubopt(&subopts, opt_list, &value)) {
                case OPT_START:
                    if(value) {
                        if ( sscanf(value, "%lu", &opt_start) != 1) {
                            opt_start = LONG_MIN;
                        }
                    }
                    break;
                case OPT_LEN:
                    if(value) {
                        if ( sscanf(value, "%lu", &opt_len) != 1) {
                            opt_len = LONG_MIN;
                        }
                    }
                    break;
                case OPT_CLASS:
                    if(value) {
                        strduplicate((char **) &opt_class, value);
                    }
                    break;
                case OPT_NAME:
                    if(value) {
                        strduplicate((char **) &opt_name, value);
                    }
                    break;
                case OPT_TYPE:
                    if(value) {
                        if ( sscanf(value, "%lu", &opt_type) != 1) {
                            opt_type = LONG_MIN;
                        }
                    }
                    break;
                case OPT_RO:
                    if(value) {
                        if ( sscanf(value, "%lu", &opt_ro) != 1) {
                            opt_ro = LONG_MIN;
                        }
                    }
                    break;
                case OPT_ADD:
                    if(opt_cmd == 0) {
                        opt_cmd = OPT_ADD;
                        opt_id = LONG_MIN;
                    }
                    break;
                case OPT_DELETE:
                    if(value) {
                        if(opt_cmd == 0) {
                            opt_cmd = OPT_DELETE;
                        }
                        if ( sscanf(value, "%lu", &opt_id) != 1) {
                            opt_id = LONG_MAX;
                        } else {
                            opt_id -= 1;
                        }
                    }
                    break;
                case OPT_INSERT:
                    if(value) {
                        if(opt_cmd == 0) {
                            opt_cmd = OPT_INSERT;
                        }
                        if ( sscanf(value, "%lu", &opt_id) != 1) {
                            opt_id = LONG_MAX;
                        } else {
                            opt_id -= 1;
                        }
                    }
                    break;
                }
            }
            sunxi_parts_add(&sunxi_parts, opt_cmd, opt_id, opt_class, opt_name, opt_start, opt_len, opt_type, opt_ro);
        }
        break;
        case 'b':
            cmd |= CMD_BACKUP;
            strduplicate(&backup_file, optarg);
            break;
        case 'r':
            cmd |= CMD_RESTORE;
            strduplicate(&restore_file, optarg);
            break;
        case 'd':
            strduplicate(&board_device, optarg);
            break;
        case 't':
            if (strstr(optarg, "a10") || strstr(optarg, "A10") || strstr(optarg, "4i") || strstr(optarg, "4I")) {
                strduplicate(&board_type, SUN4I_MBR_NAME);
            } else if (strstr(optarg, "a20") || strstr(optarg, "A20") || strstr(optarg, "7i") || strstr(optarg, "7I")) {
                strduplicate(&board_type, SUN7I_MBR_NAME);
            } else {
                sunxi_parts_free(&sunxi_parts);
                sunxi_nand_free(&sunxi_nand);
                strfree(&board_device);
                strfree(&board_type);
                strfree(&backup_file);
                strfree(&restore_file);
                strfree(&program_name);
                error(EXIT_FAILURE, EINVAL, "--type, unsupported device (%s)", optarg);
            }
            break;
        case 'f':
            cmd |= CMD_FORCE;
            break;
        case '?':
        case 'h':
            sunxi_parts_free(&sunxi_parts);
            sunxi_nand_free(&sunxi_nand);
            strfree(&board_device);
            strfree(&board_type);
            strfree(&backup_file);
            strfree(&restore_file);
            usage(program_name);
            strfree(&program_name);
            exit(EXIT_SUCCESS);
            break;
        }
    }

    if(board_type == NULL) {
        board_nand = mbr_type_file(board_device, sunxi_nand);
        if(!board_nand) {
            sunxi_parts_free(&sunxi_parts);
            sunxi_nand_free(&sunxi_nand);
            strfree(&board_device);
            strfree(&board_type);
            strfree(&backup_file);
            strfree(&restore_file);
            strfree(&program_name);
            error(EXIT_FAILURE, ENODEV, "Unable to detect device type, use --type to specify device type.");
        }
    } else {
        board_nand = mbr_type(board_type, sunxi_nand);
        if(!board_nand) {
            sunxi_parts_free(&sunxi_parts);
            sunxi_nand_free(&sunxi_nand);
            strfree(&board_device);
            strfree(&board_type);
            strfree(&backup_file);
            strfree(&restore_file);
            strfree(&program_name);
            error(EXIT_FAILURE, EINVAL, "Unable to find device definition for board %s", board_type);
        }

    }

    board_mbr = mbr_read_file_all(board_device, board_nand);
    if(!board_mbr) {
        sunxi_parts_free(&sunxi_parts);
        sunxi_nand_free(&sunxi_nand);
        strfree(&board_device);
        strfree(&board_type);
        strfree(&backup_file);
        strfree(&restore_file);
        strfree(&program_name);
        error(EXIT_FAILURE, ENOENT, "Unable to read MBR from %s", board_device);
    }

    nand_size=device_size(board_device);

    /*
     * Keep a copy of the original MBR as reference.
     */

    printf_underline('-', "Current MBR for device %s", board_nand -> mbr_type);
    mbr_print(board_nand, board_mbr);

    /*
     * Copy the MBR to our work buffer, or create a new one if requested
     */

    if( (cmd & CMD_BACKUP) == CMD_BACKUP ) {
        mbr_write_file_all(backup_file, board_nand, board_mbr);
        printf("Backing up MBRs to %s\n", backup_file);
    }

    if( (cmd & CMD_RESTORE) == CMD_RESTORE ) {
        /*
         * Restore clean and read are mutually exclusives
         */
        work_mbr = mbr_read_file_all(restore_file,  board_nand);
        if(!work_mbr) {
            mbr_free(board_nand, board_mbr);
            sunxi_parts_free(&sunxi_parts);
            sunxi_nand_free(&sunxi_nand);
            strfree(&board_device);
            strfree(&board_type);
            strfree(&backup_file);
            strfree(&restore_file);
            strfree(&program_name);
            error(EXIT_FAILURE, ENOENT, "Unable to read MBR from %s", restore_file);
        }
        printf("Read MBRs backup from %s\n", restore_file);
    } else if( (cmd & CMD_CLEAN) == CMD_CLEAN ) {
        /*
         * Initialize a new copy.
         */
        work_mbr = mbr_alloc(board_nand);
        mbr_initialize(board_nand, work_mbr);
    } else {
        if(mbr_check_all(board_nand, board_mbr)) {
            /*
             * All are good, just use that as working copy.
             */
            work_mbr = mbr_copy_all(board_nand, board_mbr);
        } else {
            /*
             * One or all MBRs are bad, initialize a new copy.
             */
            printf("All MBRs are bad, initializing a new copy in RAM\n");
            work_mbr = mbr_alloc(board_nand);
            mbr_initialize(board_nand, work_mbr);
        }
    }

    /*
     * Now the fun part, edit the partitions
     */

    if(sunxi_parts) {

        SUNXI_PARTS *sp = sunxi_parts;

        /*
         * Rewind SP
         */

        while(sp -> next != NULL) {
            sp = sp -> next;
        }

        while(sp) {
            unsigned int last = *(unsigned int *)board_nand -> mbr_get(work_mbr[0], SUNXI_MBR_PARTCOUNT);
            unsigned int nx = 0, cu = 0;
            unsigned int start = 0, len = 0;
            void *p = NULL, *q = NULL;


            if((unsigned int)sp -> id > last) {
                sp -> id = last;
            }
            switch(sp -> cmd) {
            case OPT_INSERT:
                /*
                 * Frst move the partitions
                 */
                if(sp -> id < 0) {
                    sp -> id = 0;
                }

                if((unsigned int)sp -> id < last) {
                    /*
                     * No point trying to move partition after the last one...
                     */
                    for(int i = last; i > sp -> id; i--) {
                        for(unsigned int j = 0; j < board_nand -> mbr_copy; j++) {
                            board_nand -> part_copy(work_mbr[j], i, i - 1);
                        }
                    }
                    last++;
                    for(unsigned int i =0; i < board_nand -> mbr_copy; i++) {
                        mbr_part_count_set(board_nand, work_mbr, i, last);
                    }
                }

                if(sp -> start == LONG_MIN) {
                    p = board_nand -> part_get(work_mbr[0], (sp -> id) - 1, SUNXI_PART_ADDRLO);

                    if(p) {
                        sp -> start = *(unsigned int *)p;
                    }

                    p = board_nand -> part_get(work_mbr[0], (sp -> id) - 1, SUNXI_PART_LENLO);

                    if(p) {
                        sp -> start += *(unsigned int *)p + 1;
                    }

                }

                if(sp -> len == LONG_MIN) {
                    p = board_nand -> part_get(work_mbr[0], (sp -> id) + 1, SUNXI_PART_ADDRLO);

                    if(p) {
                        sp -> len = *(unsigned int *)p - (sp -> start + 1);
                    }
                }
                
                if(!sp -> classname) {
                    /*
                     * Need a class name
                     */
                    strduplicate((char **)&(sp -> classname), "DISK");
                }

                if(!sp -> name) {
                    /*
                     * Need a partition name
                     */
                    sprintf_alloc((char **)&(sp -> name), "Partition %d", last);
                }    
            case OPT_ADD:
                /*
                 * Add or modify a partition
                 */
                if(sp -> id == LONG_MIN) {
                    sp -> id = last;
                }
                /*
                 * Use wathever was there as default if no values supplied
                 */
                if(sp -> start == LONG_MIN) {
                    p = board_nand -> part_get(work_mbr[0], sp -> id, SUNXI_PART_ADDRLO);
                    if(p) {
                        sp -> start = *(unsigned int *)p;
                    } else {
                        sp -> start = 0;
                    }
                }

                if(sp -> len == LONG_MIN) {
                    p = board_nand -> part_get(work_mbr[0], sp -> id, SUNXI_PART_LENLO);
                    if(p) {
                        sp -> len = *(unsigned int *)p;
                    } else {
                        sp -> len = 0;
                    }
                }

                if(sp -> type == LONG_MIN) {
                    p = board_nand -> part_get(work_mbr[0], sp -> id, SUNXI_PART_USERTYPE);
                    if(p) {
                        sp -> type = *(unsigned int *)p;
                    } else {
                        sp -> type = 0;
                    }
                }

                if(sp -> ro == LONG_MIN) {
                    p = board_nand -> part_get(work_mbr[0], sp -> id, SUNXI_PART_RO);
                    if(p) {
                        sp -> ro = *(unsigned int *)p;
                    } else {
                        sp -> ro = 0;
                    }
                }

                if((unsigned int)sp -> id == last) {
                    /*
                     * Since we are adding a partition, we need to check a few things
                     */
                    if(!sp -> classname) {
                        /*
                         * Need a class name
                         */
                        strduplicate((char **)&(sp -> classname), "DISK");
                    }

                    if(!sp -> name) {
                        /*
                         * Need a partition name
                         */
                        sprintf_alloc((char **)&(sp -> name), "Partition %d", last);
                    }
                    if(sp -> id > 0) {
                        /*
                         * Start need to be after the last partition
                         */
                        p = board_nand -> part_get(work_mbr[0], (sp -> id) - 1, SUNXI_PART_ADDRLO);
                        start = 0;
                        len = 0;
                        if(p) {
                            start = *(unsigned int *)p;
                        }
                        p = board_nand -> part_get(work_mbr[0], (sp -> id) - 1, SUNXI_PART_LENLO);
                        if(p) {
                            len = *(unsigned int *)p;
                        }

                        if( (unsigned int) sp -> start <= (start + len)) {
                            sp -> start = start + len + 1;
                        }
                    }

                    /*
                     * Increment the partition count.
                     */
                    last++;
                    for(unsigned int i =0; i < board_nand -> mbr_copy; i++) {
                        mbr_part_count_set(board_nand, work_mbr, i, last);
                    }
                }
                /*
                 * Last minute checks
                 */

                if((unsigned int)sp -> start > nand_size) {
                    /*
                     * Partition can't start after NAND
                     */
                    sp -> start = nand_size;
                }

                if((unsigned int)sp -> len > nand_size) {
                    /*
                     * Partition can't be bigger than NAND - sp -> start
                     */
                    sp -> len = nand_size - (unsigned int) sp -> start;
                    if(sp -> len < 0) {
                        sp -> len = 0;
                    }
                }

                mbr_part_set_all(board_nand, work_mbr, sp -> id, sp -> start, sp -> len, (char *)sp -> classname, (char *)sp -> name, sp ->type);
                break;
            case OPT_DELETE:
                /*
                 * Delete a partition.
                 */
                if(sp -> id < 0) {
                    sp -> id = 0;
                }
                if(last > 0) {
                    if((unsigned int)sp -> id < last) {
                        /*
                         * No point trying to delete partitions after the last one...
                         */
                        for(unsigned int i = sp -> id; i <= last; i++) {
                            for(unsigned int j = 0; j < board_nand -> mbr_copy; j++) {
                                board_nand -> part_copy(work_mbr[j], i, i + 1);
                            }
                        }
                        last--;
                        for(unsigned int i =0; i < board_nand -> mbr_copy; i++) {
                            mbr_part_count_set(board_nand, work_mbr, i, last);
                        }
                    }


                }
                break;
            }
            /*
             * After moving things around, sanitize the result
             */

            nx = 0;

            for(unsigned int i = 0; i < last; i++) {

                /*
                 * Check if the partition start after the previous one
                 */
                q = board_nand -> part_get(work_mbr[0], i, SUNXI_PART_ADDRLO);

                if(q) {
                    cu = *(unsigned int *)q;
                }

                for(unsigned int j = 0; j < board_nand -> mbr_copy; j++) {
                    if(cu <= nx) {
                        /*
                         * If not, fix it.
                         */
                        board_nand -> part_set(work_mbr[j], i, SUNXI_PART_ADDRLO, (void **)(intptr_t)nx);
                    }
                }

                /*
                 * Find our next start position.
                 */
                p = board_nand -> part_get(work_mbr[0], i, SUNXI_PART_ADDRLO);

                if(p) {
                    nx = *(unsigned int *)p;
                }

                p = board_nand -> part_get(work_mbr[0], i, SUNXI_PART_LENLO);
                if(p) {
                    nx += *(unsigned int *)p;
                }

                nx++;
            }
            /*
             * Move to the next record and loop.
             */
            sp = sp -> prev;
        }
    }

    /*
     * If the copies are different, ask if we really want to write it back.
     */

    if(!mbr_compare(board_nand, board_mbr, work_mbr)) {
        printf_underline('-', "New MBR for device %s:", board_nand -> mbr_type);
        mbr_print(board_nand, work_mbr);

        if( (cmd & CMD_FORCE) !=  CMD_FORCE) {
            printf("MBR was modified, write MBR to device %s? (Y/N): ", board_device);
            if(tolower(fgetc(stdin)) == 'y') {
                cmd |= CMD_FORCE;
            }
        }
        /*
         * Write MBRs back
         */
        if( (cmd & CMD_FORCE) ==  CMD_FORCE ) {
            printf("\nWritting MBR to %s\n", board_device);
            mbr_write_file_all(board_device, board_nand, work_mbr);
            sync_part(board_device);
        }
    } else {
        printf("MBRs where not modified. Nothing to write back to device\n");
    }

    mbr_free(board_nand, work_mbr);
    mbr_free(board_nand, board_mbr);

    sunxi_parts_free(&sunxi_parts);
    sunxi_nand_free(&sunxi_nand);

    strfree(&board_device);
    strfree(&board_type);
    strfree(&backup_file);
    strfree(&restore_file);
    strfree(&program_name);

    exit(EXIT_SUCCESS);
}
