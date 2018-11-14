/*
 * Copyright (C) 2016 Bernhard Nortmann <bernhard.nortmann@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _SUNXI_TOOLS_FEL_LIB_H
#define _SUNXI_TOOLS_FEL_LIB_H

#include <stdbool.h>
#include <stdint.h>
#include "progress.h"
#include "soc_info.h"

/* USB identifiers for Allwinner device in FEL mode */
#define AW_USB_VENDOR_ID	0x1F3A
#define AW_USB_PRODUCT_ID	0xEFE8

typedef struct _felusb_handle felusb_handle; /* opaque data type */

/* More general FEL "device" handle, including version data and SoC info */
typedef struct {
	felusb_handle *usb;
	struct aw_fel_version soc_version;
	soc_name_t soc_name;
	soc_info_t *soc_info;
} feldev_handle;

/* list_fel_devices() will return an array of this type */
typedef struct {
	int busnum, devnum;
	struct aw_fel_version soc_version;
	soc_name_t soc_name;
	soc_info_t *soc_info;
	uint32_t SID[4];
} feldev_list_entry;

/* FEL device management */

void feldev_init(void);
void feldev_done(feldev_handle *dev);

feldev_handle *feldev_open(int busnum, int devnum,
			   uint16_t vendor_id, uint16_t product_id);
void feldev_close(feldev_handle *dev);

feldev_list_entry *list_fel_devices(size_t *count);

/* FEL functions */

void aw_fel_read(feldev_handle *dev, uint32_t offset, void *buf, size_t len);
void aw_fel_write(feldev_handle *dev, void *buf, uint32_t offset, size_t len);
void aw_fel_write_buffer(feldev_handle *dev, void *buf, uint32_t offset,
			 size_t len, bool progress);
void aw_fel_execute(feldev_handle *dev, uint32_t offset);

void fel_readl_n(feldev_handle *dev, uint32_t addr, uint32_t *dst, size_t count);
void fel_writel_n(feldev_handle *dev, uint32_t addr, uint32_t *src, size_t count);

void fel_memmove(feldev_handle *dev,
		 uint32_t dst_addr, uint32_t src_addr, size_t size);

void fel_clrsetbits_le32(feldev_handle *dev,
			 uint32_t addr, uint32_t clrbits, uint32_t setbits);
#define fel_clrbits_le32(dev, addr, value) \
	fel_clrsetbits_le32(dev, addr, value, 0)
#define fel_setbits_le32(dev, addr, value) \
	fel_clrsetbits_le32(dev, addr, 0, value)

/* retrieve SID root key */
bool fel_get_sid_root_key(feldev_handle *dev, uint32_t *result,
			  bool force_workaround);

bool aw_fel_remotefunc_prepare(feldev_handle *dev,
			       size_t                stack_size,
			       void                 *arm_code,
			       size_t                arm_code_size,
			       size_t                num_args,
			       uint32_t             *args);
bool aw_fel_remotefunc_execute(feldev_handle *dev, uint32_t *result);

#endif /* _SUNXI_TOOLS_FEL_LIB_H */
