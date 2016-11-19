/*
 * Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 * Copyright (C) 2015 Siarhei Siamashka <siarhei.siamashka@gmail.com>
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

/**********************************************************************
 * USB library and helper functions for the FEL utility
 **********************************************************************/

#include "portable_endian.h"
#include "fel_lib.h"
#include <libusb.h>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USB_TIMEOUT	10000 /* 10 seconds */

static bool fel_lib_initialized = false;

/* This is out 'private' data type that will be part of a "FEL device" handle */
struct _felusb_handle {
	libusb_device_handle *handle;
	int endpoint_out, endpoint_in;
	bool iface_detached;
};

/* a helper function to report libusb errors */
void usb_error(int rc, const char *caption, int exitcode)
{
	if (caption)
		fprintf(stderr, "%s ", caption);

#if defined(LIBUSBX_API_VERSION) && (LIBUSBX_API_VERSION >= 0x01000102)
	fprintf(stderr, "ERROR %d: %s\n", rc, libusb_strerror(rc));
#else
	/* assume that libusb_strerror() is missing in the libusb API */
	fprintf(stderr, "ERROR %d\n", rc);
#endif

	if (exitcode != 0)
		exit(exitcode);
}

/*
 * AW_USB_MAX_BULK_SEND and the timeout constant USB_TIMEOUT are related.
 * Both need to be selected in a way that transferring the maximum chunk size
 * with (SoC-specific) slow transfer speed won't time out.
 *
 * The 512 KiB here are chosen based on the assumption that we want a 10 seconds
 * timeout, and "slow" transfers take place at approx. 64 KiB/sec - so we can
 * expect the maximum chunk being transmitted within 8 seconds or less.
 */
static const int AW_USB_MAX_BULK_SEND = 512 * 1024; /* 512 KiB per bulk request */

void usb_bulk_send(libusb_device_handle *usb, int ep, const void *data,
		   size_t length, bool progress)
{
	/*
	 * With no progress notifications, we'll use the maximum chunk size.
	 * Otherwise, it's useful to lower the size (have more chunks) to get
	 * more frequent status updates. 128 KiB per request seem suitable.
	 * (Worst case of "slow" transfers -> one update every two seconds.)
	 */
	size_t max_chunk = progress ? 128 * 1024 : AW_USB_MAX_BULK_SEND;

	size_t chunk;
	int rc, sent;
	while (length > 0) {
		chunk = length < max_chunk ? length : max_chunk;
		rc = libusb_bulk_transfer(usb, ep, (void *)data, chunk,
					  &sent, USB_TIMEOUT);
		if (rc != 0)
			usb_error(rc, "usb_bulk_send()", 2);
		length -= sent;
		data += sent;

		if (progress)
			progress_update(sent); /* notification after each chunk */
	}
}

void usb_bulk_recv(libusb_device_handle *usb, int ep, void *data, int length)
{
	int rc, recv;
	while (length > 0) {
		rc = libusb_bulk_transfer(usb, ep, data, length,
					  &recv, USB_TIMEOUT);
		if (rc != 0)
			usb_error(rc, "usb_bulk_recv()", 2);
		length -= recv;
		data += recv;
	}
}

struct aw_usb_request {
	char signature[8];
	uint32_t length;
	uint32_t unknown1;	/* 0x0c000000 */
	uint16_t request;
	uint32_t length2;	/* Same as length */
	char     pad[10];
}  __attribute__((packed));

#define AW_USB_READ	0x11
#define AW_USB_WRITE	0x12

struct aw_fel_request {
	uint32_t request;
	uint32_t address;
	uint32_t length;
	uint32_t pad;
};

/* FEL request types */
#define AW_FEL_VERSION	0x001
#define AW_FEL_1_WRITE	0x101
#define AW_FEL_1_EXEC	0x102
#define AW_FEL_1_READ	0x103

static void aw_send_usb_request(feldev_handle *dev, int type, int length)
{
	struct aw_usb_request req = {
		.signature = "AWUC",
		.request = htole16(type),
		.length = htole32(length),
		.unknown1 = htole32(0x0c000000)
	};
	req.length2 = req.length;
	usb_bulk_send(dev->usb->handle, dev->usb->endpoint_out,
		      &req, sizeof(req), false);
}

static void aw_read_usb_response(feldev_handle *dev)
{
	char buf[13];
	usb_bulk_recv(dev->usb->handle, dev->usb->endpoint_in,
		      buf, sizeof(buf));
	assert(strcmp(buf, "AWUS") == 0);
}

static void aw_usb_write(feldev_handle *dev, const void *data, size_t len,
			 bool progress)
{
	aw_send_usb_request(dev, AW_USB_WRITE, len);
	usb_bulk_send(dev->usb->handle, dev->usb->endpoint_out,
		      data, len, progress);
	aw_read_usb_response(dev);
}

static void aw_usb_read(feldev_handle *dev, const void *data, size_t len)
{
	aw_send_usb_request(dev, AW_USB_READ, len);
	usb_bulk_send(dev->usb->handle, dev->usb->endpoint_in,
		      data, len, false);
	aw_read_usb_response(dev);
}

void aw_send_fel_request(feldev_handle *dev, int type,
			 uint32_t addr, uint32_t length)
{
	struct aw_fel_request req = {
		.request = htole32(type),
		.address = htole32(addr),
		.length = htole32(length)
	};
	aw_usb_write(dev, &req, sizeof(req), false);
}

void aw_read_fel_status(feldev_handle *dev)
{
	char buf[8];
	aw_usb_read(dev, buf, sizeof(buf));
}

/* AW_FEL_VERSION request */
static void aw_fel_get_version(feldev_handle *dev, struct aw_fel_version *buf)
{
	aw_send_fel_request(dev, AW_FEL_VERSION, 0, 0);
	aw_usb_read(dev, buf, sizeof(*buf));
	aw_read_fel_status(dev);

	buf->soc_id = (le32toh(buf->soc_id) >> 8) & 0xFFFF;
	buf->unknown_0a = le32toh(buf->unknown_0a);
	buf->protocol = le32toh(buf->protocol);
	buf->scratchpad = le16toh(buf->scratchpad);
	buf->pad[0] = le32toh(buf->pad[0]);
	buf->pad[1] = le32toh(buf->pad[1]);
}

/* AW_FEL_1_READ request */
void aw_fel_read(feldev_handle *dev, uint32_t offset, void *buf, size_t len)
{
	aw_send_fel_request(dev, AW_FEL_1_READ, offset, len);
	aw_usb_read(dev, buf, len);
	aw_read_fel_status(dev);
}

/* AW_FEL_1_WRITE request */
void aw_fel_write(feldev_handle *dev, void *buf, uint32_t offset, size_t len)
{
	aw_send_fel_request(dev, AW_FEL_1_WRITE, offset, len);
	aw_usb_write(dev, buf, len, false);
	aw_read_fel_status(dev);
}

/* AW_FEL_1_EXEC request */
void aw_fel_execute(feldev_handle *dev, uint32_t offset)
{
	aw_send_fel_request(dev, AW_FEL_1_EXEC, offset, 0);
	aw_read_fel_status(dev);
}

/*
 * This function is a higher-level wrapper for the FEL write functionality.
 * Unlike aw_fel_write() above - which is reserved for internal use - this
 * routine optionally allows progress callbacks.
 */
void aw_fel_write_buffer(feldev_handle *dev, void *buf, uint32_t offset,
			 size_t len, bool progress)
{
	aw_send_fel_request(dev, AW_FEL_1_WRITE, offset, len);
	aw_usb_write(dev, buf, len, progress);
	aw_read_fel_status(dev);
}

/*
 * We don't want the scratch code/buffer to exceed a maximum size of 0x400 bytes
 * (256 32-bit words) on readl_n/writel_n transfers. To guarantee this, we have
 * to account for the amount of space the ARM code uses.
 */
#define LCODE_ARM_WORDS  12 /* word count of the [read/write]l_n scratch code */
#define LCODE_ARM_SIZE   (LCODE_ARM_WORDS << 2) /* code size in bytes */
#define LCODE_MAX_TOTAL  0x100 /* max. words in buffer */
#define LCODE_MAX_WORDS  (LCODE_MAX_TOTAL - LCODE_ARM_WORDS) /* data words */

/* multiple "readl" from sequential addresses to a destination buffer */
static void aw_fel_readl_n(feldev_handle *dev, uint32_t addr,
			   uint32_t *dst, size_t count)
{
	if (count == 0) return;
	if (count > LCODE_MAX_WORDS) {
		fprintf(stderr,
			"ERROR: Max. word count exceeded, truncating aw_fel_readl_n() transfer\n");
		count = LCODE_MAX_WORDS;
	}

	assert(LCODE_MAX_WORDS < 256); /* protect against corruption of ARM code */
	uint32_t arm_code[] = {
		htole32(0xe59f0020), /* ldr  r0, [pc, #32] ; ldr r0,[read_addr]  */
		htole32(0xe28f1024), /* add  r1, pc, #36   ; adr r1, read_data   */
		htole32(0xe59f201c), /* ldr  r2, [pc, #28] ; ldr r2,[read_count] */
		htole32(0xe3520000 + LCODE_MAX_WORDS), /* cmp	r2, #LCODE_MAX_WORDS */
		htole32(0xc3a02000 + LCODE_MAX_WORDS), /* movgt	r2, #LCODE_MAX_WORDS */
		/* read_loop: */
		htole32(0xe2522001), /* subs r2, r2, #1    ; r2 -= 1             */
		htole32(0x412fff1e), /* bxmi lr            ; return if (r2 < 0)  */
		htole32(0xe4903004), /* ldr  r3, [r0], #4  ; load and post-inc   */
		htole32(0xe4813004), /* str  r3, [r1], #4  ; store and post-inc  */
		htole32(0xeafffffa), /* b    read_loop                           */
		htole32(addr),       /* read_addr */
		htole32(count)       /* read_count */
		/* read_data (buffer) follows, i.e. values go here */
	};
	assert(sizeof(arm_code) == LCODE_ARM_SIZE);

	/* scratch buffer setup: transfers ARM code, including addr and count */
	aw_fel_write(dev, arm_code, dev->soc_info->scratch_addr, sizeof(arm_code));
	/* execute code, read back the result */
	aw_fel_execute(dev, dev->soc_info->scratch_addr);
	uint32_t buffer[count];
	aw_fel_read(dev, dev->soc_info->scratch_addr + LCODE_ARM_SIZE,
		    buffer, sizeof(buffer));
	/* extract values to destination buffer */
	uint32_t *val = buffer;
	while (count-- > 0)
		*dst++ = le32toh(*val++);
}

/*
 * aw_fel_readl_n() wrapper that can handle large transfers. If necessary,
 * those will be done in separate 'chunks' of no more than LCODE_MAX_WORDS.
 */
void fel_readl_n(feldev_handle *dev, uint32_t addr, uint32_t *dst, size_t count)
{
	while (count > 0) {
		size_t n = count > LCODE_MAX_WORDS ? LCODE_MAX_WORDS : count;
		aw_fel_readl_n(dev, addr, dst, n);
		addr += n * sizeof(uint32_t);
		dst += n;
		count -= n;
	}
}

/* multiple "writel" from a source buffer to sequential addresses */
static void aw_fel_writel_n(feldev_handle *dev, uint32_t addr,
			    uint32_t *src, size_t count)
{
	if (count == 0) return;
	if (count > LCODE_MAX_WORDS) {
		fprintf(stderr,
			"ERROR: Max. word count exceeded, truncating aw_fel_writel_n() transfer\n");
		count = LCODE_MAX_WORDS;
	}

	assert(LCODE_MAX_WORDS < 256); /* protect against corruption of ARM code */
	/*
	 * We need a fixed array size to allow for (partial) initialization,
	 * so we'll claim the maximum total number of words (0x100) here.
	 */
	uint32_t arm_code[LCODE_MAX_TOTAL] = {
		htole32(0xe59f0020), /* ldr  r0, [pc, #32] ; ldr r0,[write_addr] */
		htole32(0xe28f1024), /* add  r1, pc, #36   ; adr r1, write_data  */
		htole32(0xe59f201c), /* ldr  r2, [pc, #28] ; ldr r2,[write_count]*/
		htole32(0xe3520000 + LCODE_MAX_WORDS), /* cmp	r2, #LCODE_MAX_WORDS */
		htole32(0xc3a02000 + LCODE_MAX_WORDS), /* movgt	r2, #LCODE_MAX_WORDS */
		/* write_loop: */
		htole32(0xe2522001), /* subs r2, r2, #1    ; r2 -= 1             */
		htole32(0x412fff1e), /* bxmi lr            ; return if (r2 < 0)  */
		htole32(0xe4913004), /* ldr  r3, [r1], #4  ; load and post-inc   */
		htole32(0xe4803004), /* str  r3, [r0], #4  ; store and post-inc  */
		htole32(0xeafffffa), /* b    write_loop                          */
		htole32(addr),       /* write_addr */
		htole32(count)       /* write_count */
		/* write_data (buffer) follows, i.e. values taken from here */
	};

	/* copy values from source buffer */
	size_t i;
	for (i = 0; i < count; i++)
		arm_code[LCODE_ARM_WORDS + i] = htole32(*src++);
	/* scratch buffer setup: transfers ARM code and data */
	aw_fel_write(dev, arm_code, dev->soc_info->scratch_addr,
	             (LCODE_ARM_WORDS + count) * sizeof(uint32_t));
	/* execute, and we're done */
	aw_fel_execute(dev, dev->soc_info->scratch_addr);
}

/*
 * aw_fel_writel_n() wrapper that can handle large transfers. If necessary,
 * those will be done in separate 'chunks' of no more than LCODE_MAX_WORDS.
 */
void fel_writel_n(feldev_handle *dev, uint32_t addr, uint32_t *src, size_t count)
{
	while (count > 0) {
		size_t n = count > LCODE_MAX_WORDS ? LCODE_MAX_WORDS : count;
		aw_fel_writel_n(dev, addr, src, n);
		addr += n * sizeof(uint32_t);
		src += n;
		count -= n;
	}
}

/* general functions, "FEL device" management */

static int feldev_get_endpoint(feldev_handle *dev)
{
	struct libusb_device *usb = libusb_get_device(dev->usb->handle);
	struct libusb_config_descriptor *config;
	int if_idx, set_idx, ep_idx, ret;
	const struct libusb_interface *iface;
	const struct libusb_interface_descriptor *setting;
	const struct libusb_endpoint_descriptor *ep;

	ret = libusb_get_active_config_descriptor(usb, &config);
	if (ret)
		return ret;

	for (if_idx = 0; if_idx < config->bNumInterfaces; if_idx++) {
		iface = config->interface + if_idx;

		for (set_idx = 0; set_idx < iface->num_altsetting; set_idx++) {
			setting = iface->altsetting + set_idx;

			for (ep_idx = 0; ep_idx < setting->bNumEndpoints; ep_idx++) {
				ep = setting->endpoint + ep_idx;

				/* Test for bulk transfer endpoint */
				if ((ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK)
				    != LIBUSB_TRANSFER_TYPE_BULK)
					continue;

				if ((ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
				    == LIBUSB_ENDPOINT_IN)
					dev->usb->endpoint_in = ep->bEndpointAddress;
				else
					dev->usb->endpoint_out = ep->bEndpointAddress;
			}
		}
	}

	libusb_free_config_descriptor(config);
	return LIBUSB_SUCCESS;
}

/* claim USB interface associated with the libusb handle for a FEL device */
void feldev_claim(feldev_handle *dev)
{
	int rc = libusb_claim_interface(dev->usb->handle, 0);
#if defined(__linux__)
	if (rc != LIBUSB_SUCCESS) {
		libusb_detach_kernel_driver(dev->usb->handle, 0);
		dev->usb->iface_detached = true;
		rc = libusb_claim_interface(dev->usb->handle, 0);
	}
#endif
	if (rc)
		usb_error(rc, "libusb_claim_interface()", 1);

	rc = feldev_get_endpoint(dev);
	if (rc)
		usb_error(rc, "FAILED to get FEL mode endpoint addresses!", 1);
}

/* release USB interface associated with the libusb handle for a FEL device */
void feldev_release(feldev_handle *dev)
{
	libusb_release_interface(dev->usb->handle, 0);
#if defined(__linux__)
	if (dev->usb->iface_detached)
		libusb_attach_kernel_driver(dev->usb->handle, 0);
#endif
}

/* open handle to desired FEL device */
feldev_handle *feldev_open(int busnum, int devnum,
			   uint16_t vendor_id, uint16_t product_id)
{
	if (!fel_lib_initialized) /* if not already done: auto-initialize */
		feldev_init();
	feldev_handle *result = calloc(1, sizeof(feldev_handle));
	if (!result) {
		fprintf(stderr, "FAILED to allocate feldev_handle memory.\n");
		exit(1);
	}
	result->usb = calloc(1, sizeof(felusb_handle));
	if (!result->usb) {
		fprintf(stderr, "FAILED to allocate felusb_handle memory.\n");
		free(result);
		exit(1);
	}

	if (busnum < 0 || devnum < 0) {
		/* With the default values (busnum -1, devnum -1) we don't care
		 * for a specific USB device; so let libusb open the first
		 * device that matches VID/PID.
		 */
		result->usb->handle = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
		if (!result->usb->handle) {
			switch (errno) {
			case EACCES:
				fprintf(stderr, "ERROR: You don't have permission to access Allwinner USB FEL device\n");
				break;
			default:
				fprintf(stderr, "ERROR: Allwinner USB FEL device not found!\n");
				break;
			}
			exit(1);
		}
	} else {
		/* look for specific bus and device number */
		bool found = false;
		ssize_t rc, i;
		libusb_device **list;

		rc = libusb_get_device_list(NULL, &list);
		if (rc < 0)
			usb_error(rc, "libusb_get_device_list()", 1);
		for (i = 0; i < rc; i++) {
			if (libusb_get_bus_number(list[i]) == busnum
			    && libusb_get_device_address(list[i]) == devnum) {
				found = true; /* bus:devnum matched */
				struct libusb_device_descriptor desc;
				libusb_get_device_descriptor(list[i], &desc);
				if (desc.idVendor != vendor_id
				    || desc.idProduct != product_id) {
					fprintf(stderr, "ERROR: Bus %03d Device %03d not a FEL device "
						"(expected %04x:%04x, got %04x:%04x)\n", busnum, devnum,
						vendor_id, product_id, desc.idVendor, desc.idProduct);
					exit(1);
				}
				/* open handle to this specific device (incrementing its refcount) */
				rc = libusb_open(list[i], &result->usb->handle);
				if (rc != 0)
					usb_error(rc, "libusb_open()", 1);
				break;
			}
		}
		libusb_free_device_list(list, true);

		if (!found) {
			fprintf(stderr, "ERROR: Bus %03d Device %03d not found in libusb device list\n",
				busnum, devnum);
			exit(1);
		}
	}

	feldev_claim(result); /* claim interface, detect USB endpoints */

	/* retrieve BROM version and SoC information */
	aw_fel_get_version(result, &result->soc_version);
	result->soc_info = get_soc_info_from_version(&result->soc_version);

	return result;
}

/* close FEL device (optional, dev may be NULL) */
void feldev_close(feldev_handle *dev)
{
	if (dev) {
		if (dev->usb->handle) {
			feldev_release(dev);
			libusb_close(dev->usb->handle);
		}
		free(dev->usb); /* release memory allocated for felusb_handle */
	}
}

void feldev_init(void)
{
	int rc = libusb_init(NULL);
	if (rc != 0)
		usb_error(rc, "libusb_init()", 1);
	fel_lib_initialized = true;
}

void feldev_done(feldev_handle *dev)
{
	feldev_close(dev);
	free(dev);
	if (fel_lib_initialized) libusb_exit(NULL);
}
