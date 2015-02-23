/*
 * Copyright (C) 2012  Henrik Nordstrom <henrik@henriknordstrom.net>
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

/* Needs _BSD_SOURCE for htole and letoh  */
#define _BSD_SOURCE
#define _NETBSD_SOURCE

#include <libusb.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include "endian_compat.h"

struct  aw_usb_request {
	char signature[8];
	uint32_t length;
	uint32_t unknown1;	/* 0x0c000000 */
	uint16_t request;
	uint32_t length2;	/* Same as length */
	char	pad[10];
}  __attribute__((packed));

struct aw_fel_version {
	char signature[8];
	uint32_t soc_id;	/* 0x00162300 */
	uint32_t unknown_0a;	/* 1 */
	uint16_t protocol;	/* 1 */
	uint8_t  unknown_12;	/* 0x44 */
	uint8_t  unknown_13;	/* 0x08 */
	uint32_t scratchpad;	/* 0x7e00 */
	uint32_t pad[2];	/* unused */
} __attribute__((packed));

static const int AW_USB_READ = 0x11;
static const int AW_USB_WRITE = 0x12;

static int AW_USB_FEL_BULK_EP_OUT;
static int AW_USB_FEL_BULK_EP_IN;
static int timeout = 60000;
static int verbose = 0; /* Makes the 'fel' tool more talkative if non-zero */

static void pr_info(const char *fmt, ...)
{
	va_list arglist;
	if (verbose) {
		va_start(arglist, fmt);
		vprintf(fmt, arglist);
		va_end(arglist);
	}
}

void usb_bulk_send(libusb_device_handle *usb, int ep, const void *data, int length)
{
	int rc, sent;
	while (length > 0) {
		rc = libusb_bulk_transfer(usb, ep, (void *)data, length, &sent, timeout);
		if (rc != 0) {
			fprintf(stderr, "libusb usb_bulk_send error %d\n", rc);
			exit(2);
		}
		length -= sent;
		data += sent;
	}
}

void usb_bulk_recv(libusb_device_handle *usb, int ep, void *data, int length)
{
	int rc, recv;
	while (length > 0) {
		rc = libusb_bulk_transfer(usb, ep, data, length, &recv, timeout);
		if (rc != 0) {
			fprintf(stderr, "usb_bulk_recv error %d\n", rc);
			exit(2);
		}
		length -= recv;
		data += recv;
	}
}

void aw_send_usb_request(libusb_device_handle *usb, int type, int length)
{
	struct aw_usb_request req;
	memset(&req, 0, sizeof(req));
	strcpy(req.signature, "AWUC");
	req.length = req.length2 = htole32(length);
	req.request = htole16(type);
	req.unknown1 = htole32(0x0c000000);
	usb_bulk_send(usb, AW_USB_FEL_BULK_EP_OUT, &req, sizeof(req));
}

void aw_read_usb_response(libusb_device_handle *usb)
{
	char buf[13];
	usb_bulk_recv(usb, AW_USB_FEL_BULK_EP_IN, &buf, sizeof(buf));
	assert(strcmp(buf, "AWUS") == 0);
}

void aw_usb_write(libusb_device_handle *usb, const void *data, size_t len)
{
	aw_send_usb_request(usb, AW_USB_WRITE, len);
	usb_bulk_send(usb, AW_USB_FEL_BULK_EP_OUT, data, len);
	aw_read_usb_response(usb);
}

void aw_usb_read(libusb_device_handle *usb, const void *data, size_t len)
{
	aw_send_usb_request(usb, AW_USB_READ, len);
	usb_bulk_send(usb, AW_USB_FEL_BULK_EP_IN, data, len);
	aw_read_usb_response(usb);
}

struct aw_fel_request {
	uint32_t request;
	uint32_t address;
	uint32_t length;
	uint32_t pad;
};

static const int AW_FEL_VERSION = 0x001;
static const int AW_FEL_1_WRITE = 0x101;
static const int AW_FEL_1_EXEC  = 0x102;
static const int AW_FEL_1_READ  = 0x103;

void aw_send_fel_request(libusb_device_handle *usb, int type, uint32_t addr, uint32_t length)
{
	struct aw_fel_request req;
	memset(&req, 0, sizeof(req));
	req.request = htole32(type);
	req.address = htole32(addr);
	req.length = htole32(length);
	aw_usb_write(usb, &req, sizeof(req));
}

void aw_read_fel_status(libusb_device_handle *usb)
{
	char buf[8];
	aw_usb_read(usb, &buf, sizeof(buf));
}

void aw_fel_get_version(libusb_device_handle *usb, struct aw_fel_version *buf)
{
	aw_send_fel_request(usb, AW_FEL_VERSION, 0, 0);
	aw_usb_read(usb, buf, sizeof(*buf));
	aw_read_fel_status(usb);

	buf->soc_id = (le32toh(buf->soc_id) >> 8) & 0xFFFF;
	buf->unknown_0a = le32toh(buf->unknown_0a);
	buf->protocol = le32toh(buf->protocol);
	buf->scratchpad = le16toh(buf->scratchpad);
	buf->pad[0] = le32toh(buf->pad[0]);
	buf->pad[1] = le32toh(buf->pad[1]);
}

void aw_fel_print_version(libusb_device_handle *usb)
{
	struct aw_fel_version buf;
	aw_fel_get_version(usb, &buf);

	const char *soc_name="unknown";
	switch (buf.soc_id) {
	case 0x1623: soc_name="A10";break;
	case 0x1625: soc_name="A13";break;
	case 0x1633: soc_name="A31";break;
	case 0x1651: soc_name="A20";break;
	case 0x1650: soc_name="A23";break;
	case 0x1639: soc_name="A80";break;
	}

	printf("%.8s soc=%08x(%s) %08x ver=%04x %02x %02x scratchpad=%08x %08x %08x\n",
		buf.signature, buf.soc_id, soc_name, buf.unknown_0a,
		buf.protocol, buf.unknown_12, buf.unknown_13,
		buf.scratchpad, buf.pad[0], buf.pad[1]);
}

void aw_fel_read(libusb_device_handle *usb, uint32_t offset, void *buf, size_t len)
{
	aw_send_fel_request(usb, AW_FEL_1_READ, offset, len);
	aw_usb_read(usb, buf, len);
	aw_read_fel_status(usb);
}

void aw_fel_write(libusb_device_handle *usb, void *buf, uint32_t offset, size_t len)
{
	aw_send_fel_request(usb, AW_FEL_1_WRITE, offset, len);
	aw_usb_write(usb, buf, len);
	aw_read_fel_status(usb);
}

void aw_fel_execute(libusb_device_handle *usb, uint32_t offset)
{
	aw_send_fel_request(usb, AW_FEL_1_EXEC, offset, 0);
	aw_read_fel_status(usb);
}

void hexdump(void *data, uint32_t offset, size_t size)
{
	size_t j;
	unsigned char *buf = data;
	for (j = 0; j < size; j+=16) {
		size_t i;
		printf("%08lx: ",(long int)offset + j);
		for (i = 0; i < 16; i++) {
			if ((j+i) < size) {
				printf("%02x ", buf[j+i]);
			} else {
				printf("__ ");
			}
		}
		printf(" ");
		for (i = 0; i < 16; i++) {
			if (j+i >= size) {
				printf(".");
			} else if (isprint(buf[j+i])) {
				printf("%c", buf[j+i]);
			} else {
				printf(".");
			}
		}
		printf("\n");
	}
}

int save_file(const char *name, void *data, size_t size)
{
	FILE *out = fopen(name, "wb");
	int rc;
	if (!out) {
		perror("Failed to open output file: ");
		exit(1);
	}
	rc = fwrite(data, size, 1, out);
	fclose(out);
	return rc;
}

void *load_file(const char *name, size_t *size)
{
	size_t bufsize = 8192;
	size_t offset = 0;
	char *buf = malloc(bufsize);
	FILE *in;
	if (strcmp(name, "-") == 0)
		in = stdin;
	else
		in = fopen(name, "rb");
	if (!in) {
		perror("Failed to open input file: ");
		exit(1);
	}
	
	while(1) {
		ssize_t len = bufsize - offset;
		ssize_t n = fread(buf+offset, 1, len, in);
		offset += n;
		if (n < len)
			break;
		bufsize <<= 1;
		buf = realloc(buf, bufsize);
	}
	if (size) 
		*size = offset;
	if (in != stdin)
		fclose(in);
	return buf;
}

void aw_fel_hexdump(libusb_device_handle *usb, uint32_t offset, size_t size)
{
	unsigned char buf[size];
	aw_fel_read(usb, offset, buf, size);
	hexdump(buf, offset, size);
}

void aw_fel_dump(libusb_device_handle *usb, uint32_t offset, size_t size)
{
	unsigned char buf[size];
	aw_fel_read(usb, offset, buf, size);
	fwrite(buf, size, 1, stdout);
}
void aw_fel_fill(libusb_device_handle *usb, uint32_t offset, size_t size, unsigned char value)
{
	unsigned char buf[size];
	memset(buf, value, size);
	aw_fel_write(usb, buf, offset, size);
}

/*
 * The 'sram_swap_buffers' structure is used to describe information about
 * two buffers in SRAM, the content of which needs to be exchanged before
 * calling the U-Boot SPL code and then exchanged again before returning
 * control back to the FEL code from the BROM.
 */

typedef struct {
	uint32_t buf1; /* BROM buffer */
	uint32_t buf2; /* backup storage location */
	uint32_t size; /* buffer size */
} sram_swap_buffers;

/*
 * Each SoC variant may have its own list of memory buffers to be exchanged
 * and the information about the placement of the thunk code, which handles
 * the transition of execution from the BROM FEL code to the U-Boot SPL and
 * back.
 *
 * Note: the entries in the 'swap_buffers' tables need to be sorted by 'buf1'
 * addresses. And the 'buf1' addresses are the BROM data buffers, while 'buf2'
 * addresses are the intended backup locations.
 */
typedef struct {
	uint32_t           soc_id;     /* ID of the SoC */
	uint32_t           thunk_addr; /* Address of the thunk code */
	uint32_t           thunk_size; /* Maximal size of the thunk code */
	sram_swap_buffers *swap_buffers;
} soc_sram_info;

/*
 * The FEL code from BROM in A10/A13/A20 sets up two stacks for itself. One
 * at 0x2000 (and growing down) for the IRQ handler. And another one at 0x7000
 * (and also growing down) for the regular code. In order to use the whole
 * 32 KiB in the A1/A2 sections of SRAM, we need to temporarily move these
 * stacks elsewhere. And the addresses above 0x7000 are also a bit suspicious,
 * so it might be safer to backup the 0x7000-0x8000 area too. On A10/A13/A20
 * we can use the SRAM section A3 (0x8000) for this purpose.
 */
sram_swap_buffers a10_a13_a20_sram_swap_buffers[] = {
	{ .buf1 = 0x01800, .buf2 = 0x8000, .size = 0x800 },
	{ .buf1 = 0x05C00, .buf2 = 0x8800, .size = 0x8000 - 0x5C00 },
	{ 0 }  /* End of the table */
};

/*
 * A31 is very similar to A10/A13/A20, except that it has no SRAM at 0x8000.
 * So we use the SRAM section at 0x44000 instead. This is the memory, which
 * is normally shared with the OpenRISC core (should we do an extra check to
 * ensure that this core is powered off and can't interfere?).
 */
sram_swap_buffers a31_sram_swap_buffers[] = {
	{ .buf1 = 0x01800, .buf2 = 0x44000, .size = 0x800 },
	{ .buf1 = 0x05C00, .buf2 = 0x44800, .size = 0x8000 - 0x5C00 },
	{ 0 }  /* End of the table */
};

soc_sram_info soc_sram_info_table[] = {
	{
		.soc_id       = 0x1623, /* Allwinner A10 */
		.thunk_addr   = 0xAE00, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
	},
	{
		.soc_id       = 0x1625, /* Allwinner A13 */
		.thunk_addr   = 0xAE00, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
	},
	{
		.soc_id       = 0x1651, /* Allwinner A20 */
		.thunk_addr   = 0xAE00, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
	},
	{
		.soc_id       = 0x1633, /* Allwinner A31 */
		.thunk_addr   = 0x46E00, .thunk_size = 0x200,
		.swap_buffers = a31_sram_swap_buffers,
	},
	{ 0 } /* End of the table */
};

/*
 * This generic record assumes BROM with similar properties to A10/A13/A20/A31,
 * but no extra SRAM sections beyond 0x8000. It also assumes that the IRQ
 * handler stack usage never exceeds 0x400 bytes.
 *
 * The users may or may not hope that the 0x7000-0x8000 area is also unused
 * by the BROM and re-purpose it for the SPL stack.
 *
 * The size limit for the ".text + .data" sections is ~21 KiB.
 */
sram_swap_buffers generic_sram_swap_buffers[] = {
	{ .buf1 = 0x01C00, .buf2 = 0x5800, .size = 0x400 },
	{ 0 }  /* End of the table */
};

soc_sram_info generic_sram_info = {
	.thunk_addr   = 0x5680, .thunk_size = 0x180,
	.swap_buffers = generic_sram_swap_buffers,
};

soc_sram_info *aw_fel_get_sram_info(libusb_device_handle *usb)
{
	int i;
	struct aw_fel_version buf;

	aw_fel_get_version(usb, &buf);

	for (i = 0; soc_sram_info_table[i].swap_buffers; i++)
		if (soc_sram_info_table[i].soc_id == buf.soc_id)
			return &soc_sram_info_table[i];

	printf("Warning: no 'soc_sram_info' data for your SoC (id=%04X)\n",
	       buf.soc_id);
	return &generic_sram_info;
}

static uint32_t fel_to_spl_thunk[] = {
	#include "fel-to-spl-thunk.h"
};

void aw_fel_write_and_execute_spl(libusb_device_handle *usb,
				  uint8_t *buf, size_t len)
{
	soc_sram_info *sram_info = aw_fel_get_sram_info(usb);
	sram_swap_buffers *swap_buffers;
	char header_signature[9] = { 0 };
	size_t i, thunk_size;
	uint32_t *thunk_buf;
	uint32_t spl_checksum, spl_len, spl_len_limit = 0x8000;
	uint32_t *buf32 = (uint32_t *)buf;
	uint32_t written = 0;

	if (!sram_info || !sram_info->swap_buffers) {
		fprintf(stderr, "SPL: Unsupported SoC type\n");
		exit(1);
	}

	if (len < 32 || memcmp(buf + 4, "eGON.BT0", 8) != 0) {
		fprintf(stderr, "SPL: eGON header is not found\n");
		exit(1);
	}

	spl_checksum = 2 * le32toh(buf32[3]) - 0x5F0A6C39;
	spl_len = le32toh(buf32[4]);

	if (spl_len > len || (spl_len % 4) != 0) {
		fprintf(stderr, "SPL: bad length in the eGON header\n");
		exit(1);
	}

	len = spl_len;
	for (i = 0; i < len / 4; i++)
		spl_checksum -= le32toh(buf32[i]);

	if (spl_checksum != 0) {
		fprintf(stderr, "SPL: checksum check failed\n");
		exit(1);
	}

	swap_buffers = sram_info->swap_buffers;
	for (i = 0; swap_buffers[i].size; i++) {
		if (swap_buffers[i].buf2 < spl_len_limit)
			spl_len_limit = swap_buffers[i].buf2;
		if (len > 0 && written < swap_buffers[i].buf1) {
			uint32_t tmp = swap_buffers[i].buf1 - written;
			if (tmp > len)
				tmp = len;
			aw_fel_write(usb, buf, written, tmp);
			written += tmp;
			buf += tmp;
			len -= tmp;
		}
		if (len > 0 && written == swap_buffers[i].buf1) {
			uint32_t tmp = swap_buffers[i].size;
			if (tmp > len)
				tmp = len;
			aw_fel_write(usb, buf, swap_buffers[i].buf2, tmp);
			written += tmp;
			buf += tmp;
			len -= tmp;
		}
	}

	/* Clarify the SPL size limitations, and bail out if they are not met */
	if (sram_info->thunk_addr < spl_len_limit)
		spl_len_limit = sram_info->thunk_addr;

	if (spl_len > spl_len_limit) {
		fprintf(stderr, "SPL: too large (need %d, have %d)\n",
			(int)spl_len, (int)spl_len_limit);
		exit(1);
	}

	/* Write the remaining part of the SPL */
	if (len > 0)
		aw_fel_write(usb, buf, written, len);

	thunk_size = sizeof(fel_to_spl_thunk) + (i + 1) * sizeof(*swap_buffers);

	if (thunk_size > sram_info->thunk_size) {
		fprintf(stderr, "SPL: bad thunk size (need %d, have %d)\n",
			(int)sizeof(fel_to_spl_thunk), sram_info->thunk_size);
		exit(1);
	}

	thunk_buf = malloc(thunk_size);
	memcpy(thunk_buf, fel_to_spl_thunk, sizeof(fel_to_spl_thunk));
	memcpy(thunk_buf + sizeof(fel_to_spl_thunk) / sizeof(uint32_t),
	       swap_buffers, (i + 1) * sizeof(*swap_buffers));

	for (i = 0; i < thunk_size / sizeof(uint32_t); i++)
		thunk_buf[i] = htole32(thunk_buf[i]);

	aw_fel_write(usb, thunk_buf, sram_info->thunk_addr, thunk_size);
	aw_fel_execute(usb, sram_info->thunk_addr);

	free(thunk_buf);

	/* TODO: Try to find and fix the bug, which needs this workaround */
	usleep(250000);

	/* Read back the result and check if everything was fine */
	aw_fel_read(usb, 4, header_signature, 8);
	if (strcmp(header_signature, "eGON.FEL") != 0) {
		fprintf(stderr, "SPL: failure code '%s'\n",
			header_signature);
		exit(1);
	}
}

static int aw_fel_get_endpoint(libusb_device_handle *usb)
{
	struct libusb_device *dev = libusb_get_device(usb);
	struct libusb_config_descriptor *config;
	int if_idx, set_idx, ep_idx, ret;

	ret = libusb_get_active_config_descriptor(dev, &config);
	if (ret)
		return ret;

	for (if_idx = 0; if_idx < config->bNumInterfaces; if_idx++) {
		const struct libusb_interface *iface = config->interface + if_idx;

		for (set_idx = 0; set_idx < iface->num_altsetting; set_idx++) {
			const struct libusb_interface_descriptor *setting =
				iface->altsetting + set_idx;

			for (ep_idx = 0; ep_idx < setting->bNumEndpoints; ep_idx++) {
				const struct libusb_endpoint_descriptor *ep =
					setting->endpoint + ep_idx;

				// Test for bulk transfer endpoint
				if ((ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) !=
						LIBUSB_TRANSFER_TYPE_BULK)
					continue;

				if ((ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) ==
						LIBUSB_ENDPOINT_IN)
					AW_USB_FEL_BULK_EP_IN = ep->bEndpointAddress;
				else
					AW_USB_FEL_BULK_EP_OUT = ep->bEndpointAddress;
			}
		}
	}

	libusb_free_config_descriptor(config);

	return 0;
}

/* Less reliable than clock_gettime, but does not require linking with -lrt */
static double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + (double)tv.tv_usec / 1000000.;
}

int main(int argc, char **argv)
{
	int rc;
	libusb_device_handle *handle = NULL;
	int iface_detached = -1;
	rc = libusb_init(NULL);
	assert(rc == 0);

	if (argc <= 1) {
		printf("Usage: %s [options] command arguments... [command...]\n"
			"	-v, --verbose			Verbose logging\n"
			"	hex[dump] address length	Dumps memory region in hex\n"
			"	dump address length		Binary memory dump\n"
			"	exe[cute] address		Call function address\n"
			"	read address length file	Write memory contents into file\n"
			"	write address file		Store file contents into memory\n"
			"	ver[sion]			Show BROM version\n"
			"	clear address length		Clear memory\n"
			"	fill address length value	Fill memory\n"
			"	spl file			Load and execute U-Boot SPL\n"
			, argv[0]
		);
	}

	handle = libusb_open_device_with_vid_pid(NULL, 0x1f3a, 0xefe8);
	if (!handle) {
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
	rc = libusb_claim_interface(handle, 0);
#if defined(__linux__)
	if (rc != LIBUSB_SUCCESS) {
		libusb_detach_kernel_driver(handle, 0);
		iface_detached = 0;
		rc = libusb_claim_interface(handle, 0);
	}
#endif
	assert(rc == 0);

	if (aw_fel_get_endpoint(handle)) {
		fprintf(stderr, "ERROR: Failed to get FEL mode endpoint addresses!\n");
		exit(1);
	}

	if (argc > 1 && (strcmp(argv[1], "--verbose") == 0 ||
			 strcmp(argv[1], "-v") == 0)) {
		verbose = 1;
		argc -= 1;
		argv += 1;
	}

	while (argc > 1 ) {
		int skip = 1;
		if (strncmp(argv[1], "hex", 3) == 0 && argc > 3) {
			aw_fel_hexdump(handle, strtoul(argv[2], NULL, 0), strtoul(argv[3], NULL, 0));
			skip = 3;
		} else if (strncmp(argv[1], "dump", 4) == 0 && argc > 3) {
			aw_fel_dump(handle, strtoul(argv[2], NULL, 0), strtoul(argv[3], NULL, 0));
			skip = 3;
		} else if ((strncmp(argv[1], "exe", 3) == 0 && argc > 2)
			) {
			aw_fel_execute(handle, strtoul(argv[2], NULL, 0));
			skip=3;
		} else if (strncmp(argv[1], "ver", 3) == 0 && argc > 1) {
			aw_fel_print_version(handle);
			skip=1;
		} else if (strcmp(argv[1], "write") == 0 && argc > 3) {
			double t1, t2;
			size_t size;
			void *buf = load_file(argv[3], &size);
			t1 = gettime();
			aw_fel_write(handle, buf, strtoul(argv[2], NULL, 0), size);
			t2 = gettime();
			if (t2 > t1)
				pr_info("Written %.1f KB in %.1f sec (speed: %.1f KB/s)\n",
					(double)size / 1000., t2 - t1,
					(double)size / (t2 - t1) / 1000.);
			free(buf);
			skip=3;
		} else if (strcmp(argv[1], "read") == 0 && argc > 4) {
			size_t size = strtoul(argv[3], NULL, 0);
			void *buf = malloc(size);
			aw_fel_read(handle, strtoul(argv[2], NULL, 0), buf, size);
			save_file(argv[4], buf, size);
			free(buf);
			skip=4;
		} else if (strcmp(argv[1], "clear") == 0 && argc > 2) {
			aw_fel_fill(handle, strtoul(argv[2], NULL, 0), strtoul(argv[3], NULL, 0), 0);
			skip=3;
		} else if (strcmp(argv[1], "fill") == 0 && argc > 3) {
			aw_fel_fill(handle, strtoul(argv[2], NULL, 0), strtoul(argv[3], NULL, 0), (unsigned char)strtoul(argv[4], NULL, 0));
			skip=4;
		} else if (strcmp(argv[1], "spl") == 0 && argc > 2) {
			size_t size;
			uint8_t *buf = load_file(argv[2], &size);
			aw_fel_write_and_execute_spl(handle, buf, size);
			skip=2;
		} else {
			fprintf(stderr,"Invalid command %s\n", argv[1]);
			exit(1);
		}
		argc-=skip;
		argv+=skip;
	}

#if defined(__linux__)
	if (iface_detached >= 0)
		libusb_attach_kernel_driver(handle, iface_detached);
#endif

	return 0;
}
