/*
 * (C) Copyright 2016 Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SUNXI_TOOLS_FEL_SPIFLASH_H
#define _SUNXI_TOOLS_FEL_SPIFLASH_H

#include "fel_lib.h"
#include "progress.h"

void aw_fel_spiflash_read(feldev_handle *dev,
			  uint32_t offset, void *buf, size_t len,
			  progress_cb_t progress);
void aw_fel_spiflash_write(feldev_handle *dev,
			   uint32_t offset, void *buf, size_t len,
			   progress_cb_t progress);
void aw_fel_spiflash_info(feldev_handle *dev);
void aw_fel_spiflash_help(void);
void aw_fel_spi0_init(feldev_handle *dev);

#endif
