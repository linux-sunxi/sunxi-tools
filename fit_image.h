/*
 * Copyright (C) 2018-2020  Andre Przywara <osp@andrep.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; under version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FIT_IMAGE_H__
#define __FIT_IMAGE_H__

#include <stdint.h>
#include "fel_lib.h"

/*
 * Load all images referenced in the given U-Boot FIT image. @dt_name will
 * be used to select one of the configurations. @use_aarch64 contains the
 * target architecture of the entry point.
 * Returns the entry point address of the image to be started.
 */
uint32_t load_fit_images(feldev_handle *dev, const void *fit,
			 const char *dt_name, bool *use_aarch64);

#endif
