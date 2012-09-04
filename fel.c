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

#include <libusb.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "endian_compat.h"

struct  aw_usb_request {
	char signature[8];
	uint32_t length;
	uint32_t unknown1;	/* 0x0c000000 */
	uint16_t request;
	uint32_t length2;	/* Same as length */
	char	pad[10];
}  __attribute__((packed));

static const int AW_USB_READ = 0x11;
static const int AW_USB_WRITE = 0x12;

static const int AW_USB_FEL_BULK_EP_OUT=0x01;
static const int AW_USB_FEL_BULK_EP_IN=0x82;
static int timeout = 60000;

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

void aw_fel_get_version(libusb_device_handle *usb)
{
	struct aw_fel_version {
		char signature[8];
		uint32_t unknown_08;	/* 0x00162300 */
		uint32_t unknown_0a;	/* 1 */
		uint16_t protocol;	/* 1 */
		uint8_t  unknown_12;	/* 0x44 */
		uint8_t  unknown_13;	/* 0x08 */
		uint32_t scratchpad;	/* 0x7e00 */
		uint32_t pad[2];	/* unused */
	} __attribute__((packed)) buf;

	aw_send_fel_request(usb, AW_FEL_VERSION, 0, 0);
	aw_usb_read(usb, &buf, sizeof(buf));
	aw_read_fel_status(usb);

	buf.unknown_08 = le32toh(buf.unknown_08);
	buf.unknown_0a = le32toh(buf.unknown_0a);
	buf.protocol = le32toh(buf.protocol);
	buf.scratchpad = le16toh(buf.scratchpad);
	buf.pad[0] = le32toh(buf.pad[0]);
	buf.pad[1] = le32toh(buf.pad[1]);

	printf("%.8s %08x %08x ver=%04x %02x %02x scratchpad=%08x %08x %08x\n", buf.signature, buf.unknown_08, buf.unknown_0a, buf.protocol, buf.unknown_12, buf.unknown_13, buf.scratchpad, buf.pad[0], buf.pad[1]);
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
	assert(out);
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
	assert(in);
	
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

int main(int argc, char **argv)
{
	int rc;
	libusb_device_handle *handle = NULL;
	rc = libusb_init(NULL);
	assert(rc == 0);

	if (argc <= 1) {
		printf("Usage: %s command arguments... [command...]\n"
			"	hex[dump] address length	Dumps memory region in hex\n"
			"	dump address length		Binary memory dump\n"
			"	exe[cute] address		Call function address\n"
			"	read address length file	Write memory contents into file\n"
			"	write address file		Store file contents into memory\n"
			"	ver[sion]			Show BROM version\n"
			"	clear address length		Clear memory\n"
			"	fill address length value	Fill memory\n"
			, argv[0]
		);
	}

	handle = libusb_open_device_with_vid_pid(NULL, 0x1f3a, 0xefe8);
	if (!handle) {
		fprintf(stderr, "A10 USB FEL device not found!");
		exit(1);
	}
	rc = libusb_claim_interface(handle, 0);
	assert(rc == 0);

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
			aw_fel_get_version(handle);
			skip=1;
		} else if (strcmp(argv[1], "write") == 0 && argc > 3) {
			size_t size;
			void *buf = load_file(argv[3], &size);
			aw_fel_write(handle, buf, strtoul(argv[2], NULL, 0), size);
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
		} else {
			fprintf(stderr,"Invalid command %s\n", argv[1]);
			exit(1);
		}
		argc-=skip;
		argv+=skip;
	}

	return 0;
}
