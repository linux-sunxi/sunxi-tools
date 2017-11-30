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

#include "common.h"
#include "portable_endian.h"
#include "fel_lib.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zlib.h>
#include <sys/stat.h>

static bool verbose = false; /* If set, makes the 'fel' tool more talkative */
static uint32_t uboot_entry = 0; /* entry point (address) of U-Boot */
static uint32_t uboot_size  = 0; /* size of U-Boot binary */

/* printf-style output, but only if "verbose" flag is active */
#define pr_info(...) \
	do { if (verbose) printf(__VA_ARGS__); } while (0);

/* Constants taken from ${U-BOOT}/include/image.h */
#define IH_MAGIC	0x27051956	/* Image Magic Number	*/
#define IH_ARCH_ARM		2	/* ARM			*/
#define IH_TYPE_INVALID		0	/* Invalid Image	*/
#define IH_TYPE_FIRMWARE	5	/* Firmware Image	*/
#define IH_TYPE_SCRIPT		6	/* Script file		*/
#define IH_NMLEN		32	/* Image Name Length	*/

/* Additional error codes, newly introduced for get_image_type() */
#define IH_TYPE_ARCH_MISMATCH	-1

/*
 * Legacy format image U-Boot header,
 * all data in network byte order (aka natural aka bigendian).
 * Taken from ${U-BOOT}/include/image.h
 */
typedef struct image_header {
	uint32_t	ih_magic;	/* Image Header Magic Number	*/
	uint32_t	ih_hcrc;	/* Image Header CRC Checksum	*/
	uint32_t	ih_time;	/* Image Creation Timestamp	*/
	uint32_t	ih_size;	/* Image Data Size		*/
	uint32_t	ih_load;	/* Data	 Load  Address		*/
	uint32_t	ih_ep;		/* Entry Point Address		*/
	uint32_t	ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
} image_header_t;

#define HEADER_NAME_OFFSET	offsetof(image_header_t, ih_name)
#define HEADER_SIZE		sizeof(image_header_t)

/*
 * Utility function to determine the image type from a mkimage-compatible
 * header at given buffer (address).
 *
 * For invalid headers (insufficient size or 'magic' mismatch) the function
 * will return IH_TYPE_INVALID. Negative return values might indicate
 * special error conditions, e.g. IH_TYPE_ARCH_MISMATCH signals that the
 * image doesn't match the expected (ARM) architecture.
 * Otherwise the function will return the "ih_type" field for valid headers.
 */
int get_image_type(const uint8_t *buf, size_t len)
{
	image_header_t *hdr = (image_header_t *)buf;

	if (len <= HEADER_SIZE) /* insufficient length/size */
		return IH_TYPE_INVALID;

	if (be32toh(hdr->ih_magic) != IH_MAGIC) /* signature mismatch */
		return IH_TYPE_INVALID;
	/* For sunxi, we always expect ARM architecture here */
	if (hdr->ih_arch != IH_ARCH_ARM)
		return IH_TYPE_ARCH_MISMATCH;

	/* assume a valid header, and return ih_type */
	return hdr->ih_type;
}

void aw_fel_print_version(feldev_handle *dev)
{
	struct aw_fel_version buf = dev->soc_version;
	const char *soc_name = dev->soc_name;

	if (soc_name[0] == '0') /* hexadecimal ID -> unknown SoC */
		soc_name = "unknown";

	printf("%.8s soc=%08x(%s) %08x ver=%04x %02x %02x scratchpad=%08x %08x %08x\n",
		buf.signature, buf.soc_id, soc_name, buf.unknown_0a,
		buf.protocol, buf.unknown_12, buf.unknown_13,
		buf.scratchpad, buf.pad[0], buf.pad[1]);
}

/*
 * This wrapper for the FEL write functionality safeguards against overwriting
 * an already loaded U-Boot binary.
 * The return value represents elapsed time in seconds (needed for execution).
 */
double aw_write_buffer(feldev_handle *dev, void *buf, uint32_t offset,
		       size_t len, bool progress)
{
	/* safeguard against overwriting an already loaded U-Boot binary */
	if (uboot_size > 0 && offset <= uboot_entry + uboot_size
			   && offset + len >= uboot_entry)
		pr_fatal("ERROR: Attempt to overwrite U-Boot! "
			 "Request 0x%08X-0x%08X overlaps 0x%08X-0x%08X.\n",
			 offset, (uint32_t)(offset + len),
			 uboot_entry, uboot_entry + uboot_size);

	double start = gettime();
	aw_fel_write_buffer(dev, buf, offset, len, progress);
	return gettime() - start;
}

void hexdump(void *data, uint32_t offset, size_t size)
{
	size_t j;
	unsigned char *buf = data;
	for (j = 0; j < size; j+=16) {
		size_t i;
		printf("%08zx: ", offset + j);
		for (i = 0; i < 16; i++) {
			if (j + i < size)
				printf("%02x ", buf[j+i]);
			else
				printf("__ ");
		}
		putchar(' ');
		for (i = 0; i < 16; i++) {
			if (j + i >= size)
				putchar('.');
			else
				putchar(isprint(buf[j+i]) ? buf[j+i] : '.');
		}
		putchar('\n');
	}
}

unsigned int file_size(const char *filename)
{
	struct stat st;
	if (stat(filename, &st) != 0)
		pr_fatal("stat() error on file \"%s\": %s\n", filename,
			 strerror(errno));
	if (!S_ISREG(st.st_mode))
		pr_fatal("error: \"%s\" is not a regular file\n", filename);

	return st.st_size;
}

int save_file(const char *name, void *data, size_t size)
{
	FILE *out = fopen(name, "wb");
	int rc;
	if (!out) {
		perror("Failed to open output file");
		exit(1);
	}
	rc = fwrite(data, size, 1, out);
	fclose(out);
	return rc;
}

void *load_file(const char *name, size_t *size)
{
	size_t offset = 0, bufsize = 8192;
	char *buf = malloc(bufsize);
	FILE *in;
	if (strcmp(name, "-") == 0)
		in = stdin;
	else
		in = fopen(name, "rb");
	if (!in) {
		perror("Failed to open input file");
		exit(1);
	}
	
	while (true) {
		size_t len = bufsize - offset;
		size_t n = fread(buf+offset, 1, len, in);
		offset += n;
		if (n < len)
			break;
		bufsize *= 2;
		buf = realloc(buf, bufsize);
		if (!buf) {
			perror("Failed to resize load_file() buffer");
			exit(1);
		}
	}
	if (size) 
		*size = offset;
	if (in != stdin)
		fclose(in);
	return buf;
}

void aw_fel_hexdump(feldev_handle *dev, uint32_t offset, size_t size)
{
	if (size > 0) {
		unsigned char buf[size];
		aw_fel_read(dev, offset, buf, size);
		hexdump(buf, offset, size);
	}
}

void aw_fel_dump(feldev_handle *dev, uint32_t offset, size_t size)
{
	if (size > 0) {
		unsigned char buf[size];
		aw_fel_read(dev, offset, buf, size);
		fwrite(buf, size, 1, stdout);
	}
}
void aw_fel_fill(feldev_handle *dev, uint32_t offset, size_t size, unsigned char value)
{
	if (size > 0) {
		unsigned char buf[size];
		memset(buf, value, size);
		aw_write_buffer(dev, buf, offset, size, false);
	}
}

static uint32_t fel_to_spl_thunk[] = {
	#include "thunks/fel-to-spl-thunk.h"
};

#define	DRAM_BASE		0x40000000
#define	DRAM_SIZE		0x80000000

uint32_t aw_read_arm_cp_reg(feldev_handle *dev, soc_info_t *soc_info,
			    uint32_t coproc, uint32_t opc1, uint32_t crn,
			    uint32_t crm, uint32_t opc2)
{
	uint32_t val = 0;
	uint32_t opcode = 0xEE000000 | (1 << 20) | (1 << 4)
			  | ((opc1 & 0x7) << 21) | ((crn & 0xF) << 16)
			  | ((coproc & 0xF) << 8) | ((opc2 & 0x7) << 5)
			  | (crm & 0xF);
	uint32_t arm_code[] = {
		htole32(opcode),     /* mrc  coproc, opc1, r0, crn, crm, opc2 */
		htole32(0xe58f0000), /* str  r0, [pc]                         */
		htole32(0xe12fff1e), /* bx   lr                               */
	};
	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	aw_fel_execute(dev, soc_info->scratch_addr);
	aw_fel_read(dev, soc_info->scratch_addr + 12, &val, sizeof(val));
	return le32toh(val);
}

void aw_write_arm_cp_reg(feldev_handle *dev, soc_info_t *soc_info,
			 uint32_t coproc, uint32_t opc1, uint32_t crn,
			 uint32_t crm, uint32_t opc2, uint32_t val)
{
	uint32_t opcode = 0xEE000000 | (0 << 20) | (1 << 4)
			  | ((opc1 & 0x7) << 21) | ((crn & 0xF) << 16)
			  | ((coproc & 0xF) << 8) | ((opc2 & 7) << 5)
			  | (crm & 0xF);
	uint32_t arm_code[] = {
		htole32(0xe59f000c), /* ldr  r0, [pc, #12]                    */
		htole32(opcode),     /* mcr  coproc, opc1, r0, crn, crm, opc2 */
		htole32(0xf57ff04f), /* dsb  sy                               */
		htole32(0xf57ff06f), /* isb  sy                               */
		htole32(0xe12fff1e), /* bx   lr                               */
		htole32(val)
	};
	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	aw_fel_execute(dev, soc_info->scratch_addr);
}

/* "readl" of a single value */
uint32_t fel_readl(feldev_handle *dev, uint32_t addr)
{
	uint32_t val;
	fel_readl_n(dev, addr, &val, 1);
	return val;
}

/* "writel" of a single value */
void fel_writel(feldev_handle *dev, uint32_t addr, uint32_t val)
{
	fel_writel_n(dev, addr, &val, 1);
}

void aw_fel_print_sid(feldev_handle *dev, bool force_workaround)
{
	uint32_t key[4];
	soc_info_t *soc_info = dev->soc_info;

	if (!soc_info->sid_base) {
		printf("SID registers for your SoC (%s) are unknown or inaccessible.\n",
			dev->soc_name);
		return;
	}

	if (soc_info->sid_fix || force_workaround) {
		pr_info("Read SID key via registers, base = 0x%08X\n",
			soc_info->sid_base);
	} else {
		pr_info("SID key (e-fuses) at 0x%08X\n",
			soc_info->sid_base + soc_info->sid_offset);
	}
	fel_get_sid_root_key(dev, key, force_workaround);

	/* output SID in "xxxxxxxx:xxxxxxxx:xxxxxxxx:xxxxxxxx" format */
	for (unsigned i = 0; i <= 3; i++)
		printf("%08x%c", key[i], i < 3 ? ':' : '\n');
}

void aw_enable_l2_cache(feldev_handle *dev, soc_info_t *soc_info)
{
	uint32_t arm_code[] = {
		htole32(0xee112f30), /* mrc        15, 0, r2, cr1, cr0, {1}  */
		htole32(0xe3822002), /* orr        r2, r2, #2                */
		htole32(0xee012f30), /* mcr        15, 0, r2, cr1, cr0, {1}  */
		htole32(0xe12fff1e), /* bx         lr                        */
	};

	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	aw_fel_execute(dev, soc_info->scratch_addr);
}

void aw_get_stackinfo(feldev_handle *dev, soc_info_t *soc_info,
                      uint32_t *sp_irq, uint32_t *sp)
{
	uint32_t results[2] = { 0 };
#if 0
	/* Does not work on Cortex-A8 (needs Virtualization Extensions) */
	uint32_t arm_code[] = {
		htole32(0xe1010300), /* mrs        r0, SP_irq                */
		htole32(0xe58f0004), /* str        r0, [pc, #4]              */
		htole32(0xe58fd004), /* str        sp, [pc, #4]              */
		htole32(0xe12fff1e), /* bx         lr                        */
	};

	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	aw_fel_execute(dev, soc_info->scratch_addr);
	aw_fel_read(dev, soc_info->scratch_addr + 0x10, results, 8);
#else
	/* Works everywhere */
	uint32_t arm_code[] = {
		htole32(0xe10f0000), /* mrs        r0, CPSR                  */
		htole32(0xe3c0101f), /* bic        r1, r0, #31               */
		htole32(0xe3811012), /* orr        r1, r1, #18               */
		htole32(0xe121f001), /* msr        CPSR_c, r1                */
		htole32(0xe1a0100d), /* mov        r1, sp                    */
		htole32(0xe121f000), /* msr        CPSR_c, r0                */
		htole32(0xe58f1004), /* str        r1, [pc, #4]              */
		htole32(0xe58fd004), /* str        sp, [pc, #4]              */
		htole32(0xe12fff1e), /* bx         lr                        */
	};

	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	aw_fel_execute(dev, soc_info->scratch_addr);
	aw_fel_read(dev, soc_info->scratch_addr + 0x24, results, 8);
#endif
	*sp_irq = le32toh(results[0]);
	*sp     = le32toh(results[1]);
}

uint32_t aw_get_ttbr0(feldev_handle *dev, soc_info_t *soc_info)
{
	return aw_read_arm_cp_reg(dev, soc_info, 15, 0, 2, 0, 0);
}

uint32_t aw_get_ttbcr(feldev_handle *dev, soc_info_t *soc_info)
{
	return aw_read_arm_cp_reg(dev, soc_info, 15, 0, 2, 0, 2);
}

uint32_t aw_get_dacr(feldev_handle *dev, soc_info_t *soc_info)
{
	return aw_read_arm_cp_reg(dev, soc_info, 15, 0, 3, 0, 0);
}

uint32_t aw_get_sctlr(feldev_handle *dev, soc_info_t *soc_info)
{
	return aw_read_arm_cp_reg(dev, soc_info, 15, 0, 1, 0, 0);
}

void aw_set_ttbr0(feldev_handle *dev, soc_info_t *soc_info,
		  uint32_t ttbr0)
{
	return aw_write_arm_cp_reg(dev, soc_info, 15, 0, 2, 0, 0, ttbr0);
}

void aw_set_ttbcr(feldev_handle *dev, soc_info_t *soc_info,
		  uint32_t ttbcr)
{
	return aw_write_arm_cp_reg(dev, soc_info, 15, 0, 2, 0, 2, ttbcr);
}

void aw_set_dacr(feldev_handle *dev, soc_info_t *soc_info,
		 uint32_t dacr)
{
	aw_write_arm_cp_reg(dev, soc_info, 15, 0, 3, 0, 0, dacr);
}

void aw_set_sctlr(feldev_handle *dev, soc_info_t *soc_info,
		  uint32_t sctlr)
{
	aw_write_arm_cp_reg(dev, soc_info, 15, 0, 1, 0, 0, sctlr);
}

/*
 * Issue a "smc #0" instruction. This brings a SoC booted in "secure boot"
 * state from the default non-secure FEL into secure FEL.
 * This crashes on devices using "non-secure boot", as the BROM does not
 * provide a handler address in MVBAR. So we have a runtime check.
 */
void aw_apply_smc_workaround(feldev_handle *dev)
{
	soc_info_t *soc_info = dev->soc_info;
	uint32_t val;
	uint32_t arm_code[] = {
		htole32(0xe1600070), /* smc	#0	*/
		htole32(0xe12fff1e), /* bx	lr	*/
	};

	/* Return if the SoC does not need this workaround */
	if (!soc_info->needs_smc_workaround_if_zero_word_at_addr)
		return;

	/* This has less overhead than fel_readl_n() and may be good enough */
	aw_fel_read(dev, soc_info->needs_smc_workaround_if_zero_word_at_addr,
	            &val, sizeof(val));

	/* Return if the workaround is not needed or has been already applied */
	if (val != 0)
		return;

	pr_info("Applying SMC workaround... ");
	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	aw_fel_execute(dev, soc_info->scratch_addr);
	pr_info(" done.\n");
}

/*
 * Reconstruct the same MMU translation table as used by the A20 BROM.
 * We are basically reverting the changes, introduced in newer SoC
 * variants. This works fine for the SoC variants with the memory
 * layout similar to A20 (the SRAM is in the first megabyte of the
 * address space and the BROM is in the last megabyte of the address
 * space).
 */
uint32_t *aw_generate_mmu_translation_table(void)
{
	uint32_t *tt = malloc(4096 * sizeof(uint32_t));
	uint32_t i;

	/*
	 * Direct mapping using 1MB sections with TEXCB=00000 (Strongly
	 * ordered) for all memory except the first and the last sections,
	 * which have TEXCB=00100 (Normal). Domain bits are set to 1111
	 * and AP bits are set to 11, but this is mostly irrelevant.
	 */
	for (i = 0; i < 4096; i++)
		tt[i] = 0x00000DE2 | (i << 20);
	tt[0x000] |= 0x1000;
	tt[0xFFF] |= 0x1000;

	return tt;
}

uint32_t *aw_backup_and_disable_mmu(feldev_handle *dev,
                                    soc_info_t *soc_info)
{
	uint32_t *tt = NULL;
	uint32_t sctlr, ttbr0, ttbcr, dacr;
	uint32_t i;

	uint32_t arm_code[] = {
		/* Disable I-cache, MMU and branch prediction */
		htole32(0xee110f10), /* mrc        15, 0, r0, cr1, cr0, {0}  */
		htole32(0xe3c00001), /* bic        r0, r0, #1                */
		htole32(0xe3c00b06), /* bic        r0, r0, #0x1800           */
		htole32(0xee010f10), /* mcr        15, 0, r0, cr1, cr0, {0}  */
		/* Return back to FEL */
		htole32(0xe12fff1e), /* bx         lr                        */
	};

	/*
	 * Below are some checks for the register values, which are known
	 * to be initialized in this particular way by the existing BROM
	 * implementations. We don't strictly need them to exactly match,
	 * but still have these safety guards in place in order to detect
	 * and review any potential configuration changes in future SoC
	 * variants (if one of these checks fails, then it is not a serious
	 * problem but more likely just an indication that one of these
	 * checks needs to be relaxed).
	 */

	/* Basically, ignore M/Z/I/V/UNK bits and expect no TEX remap */
	sctlr = aw_get_sctlr(dev, soc_info);
	if ((sctlr & ~((0x7 << 11) | (1 << 6) | 1)) != 0x00C50038)
		pr_fatal("Unexpected SCTLR (%08X)\n", sctlr);

	if (!(sctlr & 1)) {
		pr_info("MMU is not enabled by BROM\n");
		return NULL;
	}

	dacr = aw_get_dacr(dev, soc_info);
	if (dacr != 0x55555555)
		pr_fatal("Unexpected DACR (%08X)\n", dacr);

	ttbcr = aw_get_ttbcr(dev, soc_info);
	if (ttbcr != 0x00000000)
		pr_fatal("Unexpected TTBCR (%08X)\n", ttbcr);

	ttbr0 = aw_get_ttbr0(dev, soc_info);
	if (ttbr0 & 0x3FFF)
		pr_fatal("Unexpected TTBR0 (%08X)\n", ttbr0);

	tt = malloc(16 * 1024);
	pr_info("Reading the MMU translation table from 0x%08X\n", ttbr0);
	aw_fel_read(dev, ttbr0, tt, 16 * 1024);
	for (i = 0; i < 4096; i++)
		tt[i] = le32toh(tt[i]);

	/* Basic sanity checks to be sure that this is a valid table */
	for (i = 0; i < 4096; i++) {
		if (((tt[i] >> 1) & 1) != 1 || ((tt[i] >> 18) & 1) != 0)
			pr_fatal("MMU: not a section descriptor\n");
		if ((tt[i] >> 20) != i)
			pr_fatal("MMU: not a direct mapping\n");
	}

	pr_info("Disabling I-cache, MMU and branch prediction...");
	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	aw_fel_execute(dev, soc_info->scratch_addr);
	pr_info(" done.\n");

	return tt;
}

void aw_restore_and_enable_mmu(feldev_handle *dev,
                               soc_info_t *soc_info,
                               uint32_t *tt)
{
	uint32_t i;
	uint32_t ttbr0 = aw_get_ttbr0(dev, soc_info);

	uint32_t arm_code[] = {
		/* Invalidate I-cache, TLB and BTB */
		htole32(0xe3a00000), /* mov        r0, #0                    */
		htole32(0xee080f17), /* mcr        15, 0, r0, cr8, cr7, {0}  */
		htole32(0xee070f15), /* mcr        15, 0, r0, cr7, cr5, {0}  */
		htole32(0xee070fd5), /* mcr        15, 0, r0, cr7, cr5, {6}  */
		htole32(0xf57ff04f), /* dsb        sy                        */
		htole32(0xf57ff06f), /* isb        sy                        */
		/* Enable I-cache, MMU and branch prediction */
		htole32(0xee110f10), /* mrc        15, 0, r0, cr1, cr0, {0}  */
		htole32(0xe3800001), /* orr        r0, r0, #1                */
		htole32(0xe3800b06), /* orr        r0, r0, #0x1800           */
		htole32(0xee010f10), /* mcr        15, 0, r0, cr1, cr0, {0}  */
		/* Return back to FEL */
		htole32(0xe12fff1e), /* bx         lr                        */
	};

	pr_info("Setting write-combine mapping for DRAM.\n");
	for (i = (DRAM_BASE >> 20); i < ((DRAM_BASE + DRAM_SIZE) >> 20); i++) {
		/* Clear TEXCB bits */
		tt[i] &= ~((7 << 12) | (1 << 3) | (1 << 2));
		/* Set TEXCB to 00100 (Normal uncached mapping) */
		tt[i] |= (1 << 12);
	}

	pr_info("Setting cached mapping for BROM.\n");
	/* Clear TEXCB bits first */
	tt[0xFFF] &= ~((7 << 12) | (1 << 3) | (1 << 2));
	/* Set TEXCB to 00111 (Normal write-back cached mapping) */
	tt[0xFFF] |= (1 << 12) | /* TEX */
		     (1 << 3)  | /* C */
		     (1 << 2);   /* B */

	pr_info("Writing back the MMU translation table.\n");
	for (i = 0; i < 4096; i++)
		tt[i] = htole32(tt[i]);
	aw_fel_write(dev, tt, ttbr0, 16 * 1024);

	pr_info("Enabling I-cache, MMU and branch prediction...");
	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	aw_fel_execute(dev, soc_info->scratch_addr);
	pr_info(" done.\n");

	free(tt);
}

/*
 * Maximum size of SPL, at the same time this is the start offset
 * of the main U-Boot image within u-boot-sunxi-with-spl.bin
 */
#define SPL_LEN_LIMIT 0x8000

void aw_fel_write_and_execute_spl(feldev_handle *dev, uint8_t *buf, size_t len)
{
	soc_info_t *soc_info = dev->soc_info;
	sram_swap_buffers *swap_buffers;
	char header_signature[9] = { 0 };
	size_t i, thunk_size;
	uint32_t *thunk_buf;
	uint32_t sp, sp_irq;
	uint32_t spl_checksum, spl_len, spl_len_limit = SPL_LEN_LIMIT;
	uint32_t *buf32 = (uint32_t *)buf;
	uint32_t cur_addr = soc_info->spl_addr;
	uint32_t *tt = NULL;

	if (!soc_info || !soc_info->swap_buffers)
		pr_fatal("SPL: Unsupported SoC type\n");
	if (len < 32 || memcmp(buf + 4, "eGON.BT0", 8) != 0)
		pr_fatal("SPL: eGON header is not found\n");

	spl_checksum = 2 * le32toh(buf32[3]) - 0x5F0A6C39;
	spl_len = le32toh(buf32[4]);

	if (spl_len > len || (spl_len % 4) != 0)
		pr_fatal("SPL: bad length in the eGON header\n");

	len = spl_len;
	for (i = 0; i < len / 4; i++)
		spl_checksum -= le32toh(buf32[i]);

	if (spl_checksum != 0)
		pr_fatal("SPL: checksum check failed\n");

	if (soc_info->needs_l2en) {
		pr_info("Enabling the L2 cache\n");
		aw_enable_l2_cache(dev, soc_info);
	}

	aw_get_stackinfo(dev, soc_info, &sp_irq, &sp);
	pr_info("Stack pointers: sp_irq=0x%08X, sp=0x%08X\n", sp_irq, sp);

	tt = aw_backup_and_disable_mmu(dev, soc_info);
	if (!tt && soc_info->mmu_tt_addr) {
		if (soc_info->mmu_tt_addr & 0x3FFF)
			pr_fatal("SPL: 'mmu_tt_addr' must be 16K aligned\n");
		pr_info("Generating the new MMU translation table at 0x%08X\n",
			soc_info->mmu_tt_addr);
		/*
		 * These settings are used by the BROM in A10/A13/A20 and
		 * we replicate them here when enabling the MMU. The DACR
		 * value 0x55555555 means that accesses are checked against
		 * the permission bits in the translation tables for all
		 * domains. The TTBCR value 0x00000000 means that the short
		 * descriptor translation table format is used, TTBR0 is used
		 * for all the possible virtual addresses (N=0) and that the
		 * translation table must be aligned at a 16K boundary.
		 */
		aw_set_dacr(dev, soc_info, 0x55555555);
		aw_set_ttbcr(dev, soc_info, 0x00000000);
		aw_set_ttbr0(dev, soc_info, soc_info->mmu_tt_addr);
		tt = aw_generate_mmu_translation_table();
	}

	swap_buffers = soc_info->swap_buffers;
	for (i = 0; swap_buffers[i].size; i++) {
		if ((swap_buffers[i].buf2 >= soc_info->spl_addr) &&
		    (swap_buffers[i].buf2 < soc_info->spl_addr + spl_len_limit))
			spl_len_limit = swap_buffers[i].buf2 - soc_info->spl_addr;
		if (len > 0 && cur_addr < swap_buffers[i].buf1) {
			uint32_t tmp = swap_buffers[i].buf1 - cur_addr;
			if (tmp > len)
				tmp = len;
			aw_fel_write(dev, buf, cur_addr, tmp);
			cur_addr += tmp;
			buf += tmp;
			len -= tmp;
		}
		if (len > 0 && cur_addr == swap_buffers[i].buf1) {
			uint32_t tmp = swap_buffers[i].size;
			if (tmp > len)
				tmp = len;
			aw_fel_write(dev, buf, swap_buffers[i].buf2, tmp);
			cur_addr += tmp;
			buf += tmp;
			len -= tmp;
		}
	}

	/* Clarify the SPL size limitations, and bail out if they are not met */
	if (soc_info->thunk_addr < spl_len_limit)
		spl_len_limit = soc_info->thunk_addr;

	if (spl_len > spl_len_limit)
		pr_fatal("SPL: too large (need %u, have %u)\n",
			 spl_len, spl_len_limit);

	/* Write the remaining part of the SPL */
	if (len > 0)
		aw_fel_write(dev, buf, cur_addr, len);

	thunk_size = sizeof(fel_to_spl_thunk) + sizeof(soc_info->spl_addr) +
		     (i + 1) * sizeof(*swap_buffers);

	if (thunk_size > soc_info->thunk_size)
		pr_fatal("SPL: bad thunk size (need %d, have %d)\n",
			 (int)sizeof(fel_to_spl_thunk), soc_info->thunk_size);

	thunk_buf = malloc(thunk_size);
	memcpy(thunk_buf, fel_to_spl_thunk, sizeof(fel_to_spl_thunk));
	memcpy(thunk_buf + sizeof(fel_to_spl_thunk) / sizeof(uint32_t),
	       &soc_info->spl_addr, sizeof(soc_info->spl_addr));
	memcpy(thunk_buf + sizeof(fel_to_spl_thunk) / sizeof(uint32_t) + 1,
	       swap_buffers, (i + 1) * sizeof(*swap_buffers));

	for (i = 0; i < thunk_size / sizeof(uint32_t); i++)
		thunk_buf[i] = htole32(thunk_buf[i]);

	pr_info("=> Executing the SPL...");
	aw_fel_write(dev, thunk_buf, soc_info->thunk_addr, thunk_size);
	aw_fel_execute(dev, soc_info->thunk_addr);
	pr_info(" done.\n");

	free(thunk_buf);

	/* TODO: Try to find and fix the bug, which needs this workaround */
	struct timespec req = { .tv_nsec = 250000000 }; /* 250ms */
	nanosleep(&req, NULL);

	/* Read back the result and check if everything was fine */
	aw_fel_read(dev, soc_info->spl_addr + 4, header_signature, 8);
	if (strcmp(header_signature, "eGON.FEL") != 0)
		pr_fatal("SPL: failure code '%s'\n", header_signature);

	/* re-enable the MMU if it was enabled by BROM */
	if (tt != NULL)
		aw_restore_and_enable_mmu(dev, soc_info, tt);
}

/*
 * This function tests a given buffer address and length for a valid U-Boot
 * image. Upon success, the image data gets transferred to the default memory
 * address stored within the image header; and the function preserves the
 * U-Boot entry point (offset) and size values.
 */
void aw_fel_write_uboot_image(feldev_handle *dev, uint8_t *buf, size_t len)
{
	if (len <= HEADER_SIZE)
		return; /* Insufficient size (no actual data), just bail out */

	image_header_t hdr = *(image_header_t *)buf;

	uint32_t hcrc = be32toh(hdr.ih_hcrc);

	/* The CRC is calculated on the whole header but the CRC itself */
	hdr.ih_hcrc = 0;
	uint32_t computed_hcrc = crc32(0, (const uint8_t *) &hdr, HEADER_SIZE);
	if (hcrc != computed_hcrc)
		pr_fatal("U-Boot header CRC mismatch: expected %x, got %x\n",
			 hcrc, computed_hcrc);

	/* Check for a valid mkimage header */
	int image_type = get_image_type(buf, len);
	if (image_type <= IH_TYPE_INVALID) {
		switch (image_type) {
		case IH_TYPE_INVALID:
			pr_error("Invalid U-Boot image: bad size or signature\n");
			break;
		case IH_TYPE_ARCH_MISMATCH:
			pr_error("Invalid U-Boot image: wrong architecture\n");
			break;
		default:
			pr_error("Invalid U-Boot image: error code %d\n",
				 image_type);
		}
		exit(1);
	}
	if (image_type != IH_TYPE_FIRMWARE)
		pr_fatal("U-Boot image type mismatch: "
			 "expected IH_TYPE_FIRMWARE, got %02X\n", image_type);

	uint32_t data_size = be32toh(hdr.ih_size); /* Image Data Size */
	uint32_t load_addr = be32toh(hdr.ih_load); /* Data Load Address */
	if (data_size > len - HEADER_SIZE)
		pr_fatal("U-Boot image data trucated: "
			 "expected %zu bytes, got %u\n",
			 len - HEADER_SIZE, data_size);

	uint32_t dcrc = be32toh(hdr.ih_dcrc);
	uint32_t computed_dcrc = crc32(0, buf + HEADER_SIZE, data_size);
	if (dcrc != computed_dcrc)
		pr_fatal("U-Boot data CRC mismatch: expected %x, got %x\n",
			 dcrc, computed_dcrc);

	/* If we get here, we're "good to go" (i.e. actually write the data) */
	pr_info("Writing image \"%.*s\", %u bytes @ 0x%08X.\n",
		IH_NMLEN, buf + HEADER_NAME_OFFSET, data_size, load_addr);

	aw_write_buffer(dev, buf + HEADER_SIZE, load_addr, data_size, false);

	/* keep track of U-Boot memory region in global vars */
	uboot_entry = load_addr;
	uboot_size = data_size;
}

/*
 * This function handles the common part of both "spl" and "uboot" commands.
 */
void aw_fel_process_spl_and_uboot(feldev_handle *dev, const char *filename)
{
	/* load file into memory buffer */
	size_t size;
	uint8_t *buf = load_file(filename, &size);
	/* write and execute the SPL from the buffer */
	aw_fel_write_and_execute_spl(dev, buf, size);
	/* check for optional main U-Boot binary (and transfer it, if applicable) */
	if (size > SPL_LEN_LIMIT)
		aw_fel_write_uboot_image(dev, buf + SPL_LEN_LIMIT, size - SPL_LEN_LIMIT);
	free(buf);
}

/*
 * Test the SPL header for our "sunxi" variant. We want to make sure that
 * we can safely use specific header fields to pass information to U-Boot.
 * In case of a missing signature (e.g. Allwinner boot0) or header version
 * mismatch, this function will return "false". If all seems fine,
 * the result is "true".
 */
#define SPL_SIGNATURE			"SPL" /* marks "sunxi" header */
#define SPL_MIN_VERSION			1 /* minimum required version */
#define SPL_MAX_VERSION			2 /* maximum supported version */
bool have_sunxi_spl(feldev_handle *dev, uint32_t spl_addr)
{
	uint8_t spl_signature[4];

	aw_fel_read(dev, spl_addr + 0x14,
		&spl_signature, sizeof(spl_signature));

	if (memcmp(spl_signature, SPL_SIGNATURE, 3) != 0)
		return false; /* signature mismatch, no "sunxi" SPL */

	if (spl_signature[3] < SPL_MIN_VERSION) {
		pr_error("sunxi SPL version mismatch: "
			 "found 0x%02X < required minimum 0x%02X\n",
			 spl_signature[3], SPL_MIN_VERSION);
		pr_error("You need to update your U-Boot (mksunxiboot) to a more recent version.\n");
		return false;
	}
	if (spl_signature[3] > SPL_MAX_VERSION) {
		pr_error("sunxi SPL version mismatch: "
			 "found 0x%02X > maximum supported 0x%02X\n",
			 spl_signature[3], SPL_MAX_VERSION);
		pr_error("You need a more recent version of this (sunxi-tools) fel utility.\n");
		return false;
	}
	return true; /* sunxi SPL and suitable version */
}

/*
 * Pass information to U-Boot via specialized fields in the SPL header
 * (see "boot_file_head" in ${U-BOOT}/arch/arm/include/asm/arch-sunxi/spl.h),
 * providing the boot script address (DRAM location of boot.scr).
 */
void pass_fel_information(feldev_handle *dev,
			  uint32_t script_address, uint32_t uEnv_length)
{
	soc_info_t *soc_info = dev->soc_info;

	/* write something _only_ if we have a suitable SPL header */
	if (have_sunxi_spl(dev, soc_info->spl_addr)) {
		pr_info("Passing boot info via sunxi SPL: "
			"script address = 0x%08X, uEnv length = %u\n",
			script_address, uEnv_length);
		uint32_t transfer[] = {
			htole32(script_address),
			htole32(uEnv_length)
		};
		aw_fel_write(dev, transfer,
			soc_info->spl_addr + 0x18, sizeof(transfer));
	}
}

/*
 * This function stores a given entry point to the RVBAR address for CPU0,
 * and then writes the Reset Management Register to request a warm boot.
 * It is useful with some AArch64 transitions, e.g. when passing control to
 * ARM Trusted Firmware (ATF) during the boot process of Pine64.
 *
 * The code was inspired by
 * https://github.com/apritzel/u-boot/commit/fda6bd1bf285c44f30ea15c7e6231bf53c31d4a8
 */
void aw_rmr_request(feldev_handle *dev, uint32_t entry_point, bool aarch64)
{
	soc_info_t *soc_info = dev->soc_info;
	if (!soc_info->rvbar_reg) {
		pr_error("ERROR: Can't issue RMR request!\n"
			 "RVBAR is not supported or unknown for your SoC (%s).\n",
			 dev->soc_name);
		return;
	}

	uint32_t rmr_mode = (1 << 1) | (aarch64 ? 1 : 0); /* RR, AA64 flag */
	uint32_t arm_code[] = {
		htole32(0xe59f0028), /* ldr        r0, [rvbar_reg]          */
		htole32(0xe59f1028), /* ldr        r1, [entry_point]        */
		htole32(0xe5801000), /* str        r1, [r0]                 */
		htole32(0xf57ff04f), /* dsb        sy                       */
		htole32(0xf57ff06f), /* isb        sy                       */

		htole32(0xe59f101c), /* ldr        r1, [rmr_mode]           */
		htole32(0xee1c0f50), /* mrc        15, 0, r0, cr12, cr0, {2}*/
		htole32(0xe1800001), /* orr        r0, r0, r1               */
		htole32(0xee0c0f50), /* mcr        15, 0, r0, cr12, cr0, {2}*/
		htole32(0xf57ff06f), /* isb        sy                       */

		htole32(0xe320f003), /* loop:      wfi                      */
		htole32(0xeafffffd), /* b          <loop>                   */

		htole32(soc_info->rvbar_reg),
		htole32(entry_point),
		htole32(rmr_mode)
	};
	/* scratch buffer setup: transfers ARM code and parameter values */
	aw_fel_write(dev, arm_code, soc_info->scratch_addr, sizeof(arm_code));
	/* execute the thunk code (triggering a warm reset on the SoC) */
	pr_info("Store entry point 0x%08X to RVBAR 0x%08X, "
		"and request warm reset with RMR mode %u...",
		entry_point, soc_info->rvbar_reg, rmr_mode);
	aw_fel_execute(dev, soc_info->scratch_addr);
	pr_info(" done.\n");
}

/* check buffer for magic "#=uEnv", indicating uEnv.txt compatible format */
static bool is_uEnv(void *buffer, size_t size)
{
	if (size <= 6)
		return false; /* insufficient size */
	return memcmp(buffer, "#=uEnv", 6) == 0;
}

/* private helper function, gets used for "write*" and "multi*" transfers */
static unsigned int file_upload(feldev_handle *dev, size_t count,
				size_t argc, char **argv, progress_cb_t callback)
{
	if (argc < count * 2)
		pr_fatal("error: too few arguments for uploading %zu files\n",
			 count);

	/* get all file sizes, keeping track of total bytes */
	size_t size = 0;
	unsigned int i;
	for (i = 0; i < count; i++)
		size += file_size(argv[i * 2 + 1]);

	progress_start(callback, size); /* set total size and progress callback */

	/* now transfer each file in turn */
	for (i = 0; i < count; i++) {
		void *buf = load_file(argv[i * 2 + 1], &size);
		if (size > 0) {
			uint32_t offset = strtoul(argv[i * 2], NULL, 0);
			aw_write_buffer(dev, buf, offset, size, callback != NULL);

			/* If we transferred a script, try to inform U-Boot about its address. */
			if (get_image_type(buf, size) == IH_TYPE_SCRIPT)
				pass_fel_information(dev, offset, 0);
			if (is_uEnv(buf, size)) /* uEnv-style data */
				pass_fel_information(dev, offset, size);
		}
		free(buf);
	}

	return i; /* return number of files that were processed */
}

static void felusb_list_devices(void)
{
	size_t devices; /* FEL device count */
	feldev_list_entry *list, *entry;

	list = list_fel_devices(&devices);
	for (entry = list; entry->soc_version.soc_id; entry++) {
		printf("USB device %03d:%03d   Allwinner %-8s",
			entry->busnum, entry->devnum, entry->soc_name);
		/* output SID only if non-zero */
		if (entry->SID[0] | entry->SID[1] | entry->SID[2] | entry->SID[3])
			printf("%08x:%08x:%08x:%08x",
			       entry->SID[0], entry->SID[1], entry->SID[2], entry->SID[3]);
		putchar('\n');
	}
	free(list);

	if (verbose && devices == 0)
		pr_error("No Allwinner devices in FEL mode detected.\n");

	feldev_done(NULL);
	exit(devices > 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void select_by_sid(const char *sid_arg, int *busnum, int *devnum)
{
	char sid[36];
	feldev_list_entry *list, *entry;

	list = list_fel_devices(NULL);
	for (entry = list; entry->soc_version.soc_id; entry++) {
		snprintf(sid, sizeof(sid), "%08x:%08x:%08x:%08x",
			entry->SID[0], entry->SID[1], entry->SID[2], entry->SID[3]);
		if (strcmp(sid, sid_arg) == 0) {
			*busnum = entry->busnum;
			*devnum = entry->devnum;
			break;
		}
	}
	free(list);
}

void usage(const char *cmd) {
	puts("sunxi-fel " VERSION "\n");
	printf("Usage: %s [options] command arguments... [command...]\n"
		"	-h, --help			Print this usage summary and exit\n"
		"	-v, --verbose			Verbose logging\n"
		"	-p, --progress			\"write\" transfers show a progress bar\n"
		"	-l, --list			Enumerate all (USB) FEL devices and exit\n"
		"	-d, --dev bus:devnum		Use specific USB bus and device number\n"
		"	    --sid SID			Select device by SID key (exact match)\n"
		"\n"
		"	spl file			Load and execute U-Boot SPL\n"
		"		If file additionally contains a main U-Boot binary\n"
		"		(u-boot-sunxi-with-spl.bin), this command also transfers that\n"
		"		to memory (default address from image), but won't execute it.\n"
		"\n"
		"	uboot file-with-spl		like \"spl\", but actually starts U-Boot\n"
		"		U-Boot execution will take place when the fel utility exits.\n"
		"		This allows combining \"uboot\" with further \"write\" commands\n"
		"		(to transfer other files needed for the boot).\n"
		"\n"
		"	hex[dump] address length	Dumps memory region in hex\n"
		"	dump address length		Binary memory dump\n"
		"	exe[cute] address		Call function address\n"
		"	reset64 address			RMR request for AArch64 warm boot\n"
		"	memmove dest source size	Copy <size> bytes within device memory\n"
		"	readl address			Read 32-bit value from device memory\n"
		"	writel address value		Write 32-bit value to device memory\n"
		"	read address length file	Write memory contents into file\n"
		"	write address file		Store file contents into memory\n"
		"	write-with-progress addr file	\"write\" with progress bar\n"
		"	write-with-gauge addr file	Output progress for \"dialog --gauge\"\n"
		"	write-with-xgauge addr file	Extended gauge output (updates prompt)\n"
		"	multi[write] # addr file ...	\"write-with-progress\" multiple files,\n"
		"					sharing a common progress status\n"
		"	multi[write]-with-gauge ...	like their \"write-with-*\" counterpart,\n"
		"	multi[write]-with-xgauge ...	  but following the 'multi' syntax:\n"
		"					  <#> addr file [addr file [...]]\n"
		"	echo-gauge \"some text\"		Update prompt/caption for gauge output\n"
		"	ver[sion]			Show BROM version\n"
		"	sid				Retrieve and output 128-bit SID key\n"
		"	clear address length		Clear memory\n"
		"	fill address length value	Fill memory\n"
		, cmd);
	exit(0);
}

int main(int argc, char **argv)
{
	bool uboot_autostart = false; /* flag for "uboot" command = U-Boot autostart */
	bool pflag_active = false; /* -p switch, causing "write" to output progress */
	bool device_list = false; /* -l switch, prints device list and exits */
	feldev_handle *handle;
	int busnum = -1, devnum = -1;
	char *sid_arg = NULL;

	if (argc <= 1)
		usage(argv[0]);

	/* process all "prefix"-type arguments first */
	while (argc > 1) {
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
			usage(argv[0]);
		else if (strcmp(argv[1], "--verbose") == 0 || strcmp(argv[1], "-v") == 0)
			verbose = true;
		else if (strcmp(argv[1], "--progress") == 0 || strcmp(argv[1], "-p") == 0)
			pflag_active = true;
		else if (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-l") == 0
			 || strcmp(argv[1], "list") == 0)
			device_list = true;
		else if (strncmp(argv[1], "--dev", 5) == 0 || strncmp(argv[1], "-d", 2) == 0) {
			char *dev_arg = argv[1];
			dev_arg += strspn(dev_arg, "-dev="); /* skip option chars, ignore '=' */
			if (*dev_arg == 0 && argc > 2) { /* at end of argument, use the next one instead */
				dev_arg = argv[2];
				argc -= 1;
				argv += 1;
			}
			if (sscanf(dev_arg, "%d:%d", &busnum, &devnum) != 2
			    || busnum <= 0 || devnum <= 0)
				pr_fatal("ERROR: Expected 'bus:devnum', got '%s'.\n", dev_arg);
			pr_info("Selecting USB Bus %03d Device %03d\n", busnum, devnum);
		}
		else if (strcmp(argv[1], "--sid") == 0 && argc > 2) {
			sid_arg = argv[2];
			argc -= 1;
			argv += 1;
		} else
			break; /* no valid (prefix) option detected, exit loop */
		argc -= 1;
		argv += 1;
	}

	/*
	 * If any option-style arguments remain (starting with '-') we know that
	 * we won't recognize them later (at best yielding "Invalid command").
	 * However this would only happen _AFTER_ trying to open a FEL device,
	 * which might fail with "Allwinner USB FEL device not found". To avoid
	 * confusing the user, bail out here - with a more descriptive message.
	 */
	int i;
	for (i = 1; i < argc; i++)
		if (*argv[i] == '-')
			pr_fatal("Invalid option %s\n", argv[i]);

	/* Process options that don't require a FEL device handle */
	if (device_list)
		felusb_list_devices(); /* and exit program afterwards */
	if (sid_arg) {
		/* try to set busnum and devnum according to "--sid" option */
		select_by_sid(sid_arg, &busnum, &devnum);
		if (busnum <= 0 || devnum <= 0)
			pr_fatal("No matching FEL device found for SID '%s'\n",
				 sid_arg);
		pr_info("Selecting FEL device %03d:%03d by SID\n", busnum, devnum);
	}

	/*
	 * Open FEL device - either specified by busnum:devnum, or
	 * the first one matching the given USB vendor/procduct ID.
	 */
	handle = feldev_open(busnum, devnum, AW_USB_VENDOR_ID, AW_USB_PRODUCT_ID);

	/* Some SoCs need the SMC workaround to enter the secure boot mode */
	aw_apply_smc_workaround(handle);

	/* Handle command-style arguments, in order of appearance */
	while (argc > 1 ) {
		int skip = 1;

		if (strncmp(argv[1], "hex", 3) == 0 && argc > 3) {
			aw_fel_hexdump(handle, strtoul(argv[2], NULL, 0), strtoul(argv[3], NULL, 0));
			skip = 3;
		} else if (strncmp(argv[1], "dump", 4) == 0 && argc > 3) {
			aw_fel_dump(handle, strtoul(argv[2], NULL, 0), strtoul(argv[3], NULL, 0));
			skip = 3;
		} else if (strcmp(argv[1], "memmove") == 0 && argc > 4) {
			/* three parameters: destination addr, source addr, byte count */
			fel_memmove(handle, strtoul(argv[2], NULL, 0),
				    strtoul(argv[3], NULL, 0), strtoul(argv[4], NULL, 0));
			skip = 4;
		} else if (strcmp(argv[1], "readl") == 0 && argc > 2) {
			printf("0x%08x\n", fel_readl(handle, strtoul(argv[2], NULL, 0)));
			skip = 2;
		} else if (strcmp(argv[1], "writel") == 0 && argc > 3) {
			fel_writel(handle, strtoul(argv[2], NULL, 0), strtoul(argv[3], NULL, 0));
			skip = 3;
		} else if (strncmp(argv[1], "exe", 3) == 0 && argc > 2) {
			aw_fel_execute(handle, strtoul(argv[2], NULL, 0));
			skip=3;
		} else if (strcmp(argv[1], "reset64") == 0 && argc > 2) {
			aw_rmr_request(handle, strtoul(argv[2], NULL, 0), true);
			/* Cancel U-Boot autostart, and stop processing args */
			uboot_autostart = false;
			break;
		} else if (strncmp(argv[1], "ver", 3) == 0) {
			aw_fel_print_version(handle);
		} else if (strcmp(argv[1], "sid") == 0) {
			aw_fel_print_sid(handle, false);
		} else if (strcmp(argv[1], "sid-registers") == 0) {
			aw_fel_print_sid(handle, true); /* enforce register access */
		} else if (strcmp(argv[1], "write") == 0 && argc > 3) {
			skip += 2 * file_upload(handle, 1, argc - 2, argv + 2,
					pflag_active ? progress_bar : NULL);
		} else if (strcmp(argv[1], "write-with-progress") == 0 && argc > 3) {
			skip += 2 * file_upload(handle, 1, argc - 2, argv + 2,
						progress_bar);
		} else if (strcmp(argv[1], "write-with-gauge") == 0 && argc > 3) {
			skip += 2 * file_upload(handle, 1, argc - 2, argv + 2,
						progress_gauge);
		} else if (strcmp(argv[1], "write-with-xgauge") == 0 && argc > 3) {
			skip += 2 * file_upload(handle, 1, argc - 2, argv + 2,
						progress_gauge_xxx);
		} else if ((strcmp(argv[1], "multiwrite") == 0 ||
			    strcmp(argv[1], "multi") == 0) && argc > 4) {
			size_t count = strtoul(argv[2], NULL, 0); /* file count */
			skip = 2 + 2 * file_upload(handle, count, argc - 3,
						   argv + 3, progress_bar);
		} else if ((strcmp(argv[1], "multiwrite-with-gauge") == 0 ||
			    strcmp(argv[1], "multi-with-gauge") == 0) && argc > 4) {
			size_t count = strtoul(argv[2], NULL, 0); /* file count */
			skip = 2 + 2 * file_upload(handle, count, argc - 3,
						   argv + 3, progress_gauge);
		} else if ((strcmp(argv[1], "multiwrite-with-xgauge") == 0 ||
			    strcmp(argv[1], "multi-with-xgauge") == 0) && argc > 4) {
			size_t count = strtoul(argv[2], NULL, 0); /* file count */
			skip = 2 + 2 * file_upload(handle, count, argc - 3,
						   argv + 3, progress_gauge_xxx);
		} else if ((strcmp(argv[1], "echo-gauge") == 0) && argc > 2) {
			skip = 2;
			printf("XXX\n0\n%s\nXXX\n", argv[2]);
			fflush(stdout);
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
			aw_fel_process_spl_and_uboot(handle, argv[2]);
			skip=2;
		} else if (strcmp(argv[1], "uboot") == 0 && argc > 2) {
			aw_fel_process_spl_and_uboot(handle, argv[2]);
			uboot_autostart = (uboot_entry > 0 && uboot_size > 0);
			if (!uboot_autostart)
				printf("Warning: \"uboot\" command failed to detect image! Can't execute U-Boot.\n");
			skip=2;
		} else {
			pr_fatal("Invalid command %s\n", argv[1]);
		}
		argc-=skip;
		argv+=skip;
	}

	/* auto-start U-Boot if requested (by the "uboot" command) */
	if (uboot_autostart) {
		pr_info("Starting U-Boot (0x%08X).\n", uboot_entry);
		aw_fel_execute(handle, uboot_entry);
	}

	feldev_done(handle);

	return 0;
}
