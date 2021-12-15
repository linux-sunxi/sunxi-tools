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

#include <stdio.h>
#include <stdint.h>
#include <libfdt.h>

#include "common.h"
#include "fel_lib.h"
#include "fit_image.h"

/* defined in fel.c */
extern bool verbose;

#define IH_ARCH_INVALID			0
#define IH_ARCH_ARM			2
#define IH_ARCH_ARM64			22

#define IH_OS_INVALID			0
#define IH_OS_LINUX			5
#define IH_OS_U_BOOT			17
#define IH_OS_ARM_TRUSTED_FIRMWARE	25
#define IH_OS_EFI			28

struct fit_image_info {
	const char *description;
	const char *data;
	uint32_t data_size;
	uint32_t load_addr;
	uint32_t entry_point;
	uint8_t os;
	uint8_t arch;
};

static int fit_parse_os(const char *value)
{
	if (!value || !*value)
		return IH_OS_INVALID;

	if (!strcmp(value, "u-boot"))
		return IH_OS_U_BOOT;
	if (!strcmp(value, "linux"))
		return IH_OS_LINUX;
	if (!strcmp(value, "arm-trusted-firmware"))
		return IH_OS_ARM_TRUSTED_FIRMWARE;
	if (!strcmp(value, "efi"))
		return IH_OS_EFI;

	return IH_OS_INVALID;
}

static int fit_parse_arch(const char *value)
{
	if (!value || !*value)
		return IH_ARCH_INVALID;

	if (!strcmp(value, "arm"))
		return IH_ARCH_ARM;
	if (!strcmp(value, "arm64"))
		return IH_ARCH_ARM64;

	return IH_ARCH_INVALID;
}

static uint32_t fdt_getprop_u32(const void *fdt, int node, const char *name)
{
	const fdt32_t *val;

	val = fdt_getprop(fdt, node, name, NULL);
	if (!val)
		return ~0U;

	return fdt32_to_cpu(*val);
}

static const char *fdt_getprop_str(const void *fdt, int node, const char *name)
{
	const struct fdt_property *prop;

	prop = fdt_get_property(fdt, node, name, NULL);
	if (!prop)
		return NULL;

	return prop->data;
}

/*
 * Find the image with the given name under the /images node, and parse
 * its information into the fit_image_info struct.
 * Returns 0 on success, and a negative error value otherwise.
 */
static int fit_get_image_info(const void *fit, const char *name,
			      struct fit_image_info *info)
{
	int node;
	const char *str;
	uint32_t data_offset;

	node = fdt_path_offset(fit, "/images");
	if (node < 0)
		return -1;
	node = fdt_subnode_offset(fit, node, name);
	if (node < 0)
		return -1;

	info->load_addr = fdt_getprop_u32(fit, node, "load");
	info->entry_point = fdt_getprop_u32(fit, node, "entry");
	info->description = fdt_getprop_str(fit, node, "description");
	/* properties used for FIT images with external data */
	info->data_size = fdt_getprop_u32(fit, node, "data-size");
	data_offset = fdt_getprop_u32(fit, node, "data-offset");

	/* check for embedded data (when invalid external data properties) */
	if (info->data_size == ~0U || data_offset == ~0U) {
		const struct fdt_property *prop;
		int len;

		prop = fdt_get_property(fit, node, "data", &len);
		info->data_size = len;
		info->data = prop->data;
	} else {
		/* external data is appended at the end of the FIT DTB blob */
		info->data = (const char *)fit + ((fdt_totalsize(fit) + 3) & ~3U);
		info->data += data_offset;
	}

	info->os = fit_parse_os(fdt_getprop_str(fit, node, "os"));
	info->arch = fit_parse_arch(fdt_getprop_str(fit, node, "arch"));

	str = fdt_getprop_str(fit, node, "compression");
	/* The current SPL does not support compression either. */
	if (str && strcmp(str, "none")) {
		printf("compression \"%s\" not supported for image \"%s\"\n",
		       str, info->description);
		return -2;
	}

	return 0;
}

static int entry_arch;
static uint32_t dtb_addr;

/*
 * Upload the image described by its fit_image_info struct to the board.
 * Detect if an image contains an entry point and return that.
 * Set entry_arch to arm or arm64 on the way. Also detect the image
 * containing U-Boot and record its end address, so that the DTB can be
 * appended later on.
 * Returns the entry point if any is specified, or 0 otherwise.
 */
static uint32_t fit_load_image(feldev_handle *dev, struct fit_image_info *img)
{
	uint32_t ret = 0;

	if (verbose)
		printf("loading image \"%s\" (%d bytes) to 0x%x\n",
		       img->description, img->data_size, img->load_addr);
	aw_fel_write_buffer(dev, img->data,
			    img->load_addr, img->data_size, true);

	if (img->entry_point != ~0U) {
		ret = img->entry_point;
		entry_arch = img->arch;
	}
	/* either explicitly marked as U-Boot, or the first invalid one */
	if (img->os == IH_OS_U_BOOT ||
	    (!dtb_addr && img->os == IH_OS_INVALID))
		dtb_addr = img->load_addr + img->data_size;

	return ret;
}

uint32_t load_fit_images(feldev_handle *dev, const void *fit,
			 const char *dt_name, bool *use_aarch64)
{
	const struct fdt_property *prop;
	struct fit_image_info img;
	const char *str;
	int node, len;
	uint32_t entry_point = 0;

	node = fdt_path_offset(fit, "/configurations");
	if (node < 0) {
		pr_error("invalid FIT image, no /configurations node\n");
		return 0;
	}

	/*
	 * Find the right configuration node, either by using the provided
	 * DT name as an identifier, falling back to the node titled "default",
	 * or by using just the first node.
	 */
	if (dt_name) {
		for (node = fdt_first_subnode(fit, node);
		     node >= 0;
		     node = fdt_next_subnode(fit, node)) {
			prop = fdt_get_property(fit, node, "description", NULL);
			if (prop && !strcmp(prop->data, dt_name))
				break;
		}
		if (node < 0) {
			pr_error("no matching FIT configuration node for \"%s\"\n",
			       dt_name);
			return 0;
		}
	} else {
		prop = fdt_get_property(fit, node, "default", NULL);
		if (!prop)
			node = fdt_first_subnode(fit, node);
		else
			node = fdt_subnode_offset(fit, node, prop->data);
		if (node < 0) {
			pr_error("no default FIT configuration node\n");
			return 0;
		}
	}

	entry_arch = IH_ARCH_INVALID;
	dtb_addr = 0;

	/* Load the image described as "firmware". */
	str = fdt_getprop_str(fit, node, "firmware");
	if (str && !fit_get_image_info(fit, str, &img)) {
		uint32_t addr = fit_load_image(dev, &img);

		if (addr != 0)
			entry_point = addr;
	} else {
		printf("WARNING: no valid \"firmware\" image entry in FIT\n");
	}

	/* load all loadables, at their respective load addresses */
	prop = fdt_get_property(fit, node, "loadables", &len);
	for (str = prop ? prop->data : NULL;
	     prop && (str - prop->data) < len && *str;
	     str += strlen(str) + 1) {
		uint32_t addr;

		if (fit_get_image_info(fit, str, &img)) {
			printf("Can't load loadable \"%s\", skipping.\n", str);
			continue;
		}
		addr = fit_load_image(dev, &img);
		if (addr != 0)
			entry_point = addr;
	}

	if (use_aarch64)
		*use_aarch64 = (entry_arch == IH_ARCH_ARM64);

	if (!dtb_addr) {
		printf("Warning: no U-Boot image found, not loading DTB\n");
		return entry_point;
	}

	/* load .dtb right after the U-Boot image (appended DTB) */
	str = fdt_getprop_str(fit, node, "fdt");
	if (!str || fit_get_image_info(fit, str, &img)) {
		printf("Warning: no FDT found in FIT image\n");
		return entry_point;
	}
	if (verbose)
		printf("loading DTB \"%s\" (%d bytes)\n", img.description,
		       img.data_size);
	aw_fel_write_buffer(dev, img.data, dtb_addr, img.data_size, false);

	return entry_point;
}
