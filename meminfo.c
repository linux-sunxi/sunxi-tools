/*
 * Copyright (C) 2012  Floris Bos <bos@je-eigen-domein.nl>
 * Copyright (c) 2014  Luc Verhaegen <libv@skynet.be>
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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include <sys/io.h>
#include <stdbool.h>

typedef uint32_t u32;

/* from u-boot code: */
struct dram_para {
	u32 baseaddr;
	u32 clock;
	u32 type;
	u32 rank_num;
	u32 density;
	u32 io_width;
	u32 bus_width;
	u32 cas;
	u32 zq;
	u32 odt_en;
	u32 size;
	u32 tpr0;
	u32 tpr1;
	u32 tpr2;
	u32 tpr3;
	u32 tpr4;
	u32 tpr5;
	u32 emr1;
	u32 emr2;
	u32 emr3;
};

#define DEVMEM_FILE "/dev/mem"
static int devmem_fd;

enum sunxi_soc_version {
	SUNXI_SOC_SUN4I = 0x1623, /* A10 */
	SUNXI_SOC_SUN5I = 0x1625, /* A13, A10s */
	SUNXI_SOC_SUN6I = 0x1633, /* A31 */
	SUNXI_SOC_SUN7I = 0x1651, /* A20 */
	SUNXI_SOC_SUN8I = 0x1650, /* A23 */
};

static enum sunxi_soc_version soc_version;

/*
 * Libv's favourite register handling calls.
 */
unsigned int
sunxi_io_read(void *base, int offset)
{
	return inl((unsigned long) (base + offset));
}

void
sunxi_io_write(void *base, int offset, unsigned int value)
{
	outl(value, (unsigned long) (base + offset));
}

void
sunxi_io_mask(void *base, int offset, unsigned int value, unsigned int mask)
{
	unsigned int tmp = inl((unsigned long) (base + offset));

	tmp &= ~mask;
	tmp |= value & mask;

	outl(tmp, (unsigned long) (base + offset));
}


/*
 * Find out exactly which SoC we are dealing with.
 */
#define SUNXI_IO_SRAM_BASE	0x01C00000
#define SUNXI_IO_SRAM_SIZE	0x00001000

#define SUNXI_IO_SRAM_VERSION	0x24

static int
soc_version_read(void)
{
	void *base;
	unsigned int restore;

	base = mmap(NULL, SUNXI_IO_SRAM_SIZE, PROT_READ|PROT_WRITE,
		    MAP_SHARED, devmem_fd, SUNXI_IO_SRAM_BASE);
	if (base == MAP_FAILED) {
		fprintf(stderr, "Failed to map sram registers: %s\n",
			strerror(errno));
		return errno;
	}

	restore = sunxi_io_read(base, SUNXI_IO_SRAM_VERSION);

	sunxi_io_mask(base, SUNXI_IO_SRAM_VERSION, 0x8000, 0x8000);

	soc_version = sunxi_io_read(base, SUNXI_IO_SRAM_VERSION) >> 16;

	sunxi_io_mask(base, SUNXI_IO_SRAM_VERSION, restore, 0x8000);

	munmap(base, SUNXI_IO_SRAM_SIZE);

	return 0;
}

static int
soc_version_check(void)
{
	switch (soc_version) {
	case SUNXI_SOC_SUN4I:
	case SUNXI_SOC_SUN5I:
	case SUNXI_SOC_SUN7I:
		return 0;
	default:
		fprintf(stderr, "Error: unknown or unhandled Soc: 0x%04X\n",
			soc_version);
		return -1;
	}
}

/*
 * Read DRAM clock.
 */
#define SUNXI_IO_CCM_BASE	0x01C20000
#define SUNXI_IO_CCM_SIZE	0x00001000

#define SUNXI_IO_CCM_PLL5_CFG	0x20

static int
dram_clock_read(struct dram_para *dram_para)
{
	void *base;
	unsigned int tmp;
	int n, k, m;

	base = mmap(NULL, SUNXI_IO_CCM_SIZE, PROT_READ,
		    MAP_SHARED, devmem_fd, SUNXI_IO_CCM_BASE);
	if (base == MAP_FAILED) {
		fprintf(stderr, "Failed to map ccm registers: %s\n",
			strerror(errno));
		return errno;
	}

	tmp = sunxi_io_read(base, SUNXI_IO_CCM_PLL5_CFG);

	munmap(base, SUNXI_IO_CCM_SIZE);

	n = (tmp >> 8) & 0x1F;
	k = ((tmp >> 4) & 0x03) + 1;
	m = (tmp & 0x03) + 1;

	dram_para->clock = (24 * n * k) / m;

	return 0;
}

/*
 * Read DRAM parameters.
 */
#define SUNXI_IO_DRAM_BASE	0x01C01000
#define SUNXI_IO_DRAM_SIZE	0x00001000

#define SUNXI_IO_DRAM_CCR	0x000 /* controller configuration register */
#define SUNXI_IO_DRAM_DCR	0x004 /* dram configuration */
#define SUNXI_IO_DRAM_IOCR	0x008 /* i/o configuration */

#define SUNXI_IO_DRAM_TPR0	0x014 /* dram timing parameters register 0 */
#define SUNXI_IO_DRAM_TPR1	0x018 /* dram timing parameters register 1 */
#define SUNXI_IO_DRAM_TPR2	0x01C /* dram timing parameters register 2 */

#define SUNXI_IO_DRAM_ZQCR0	0x0A8 /* zq control register 0 */
#define SUNXI_IO_DRAM_ZQCR1	0x0AC /* zq control register 1 */

#define SUNXI_IO_DRAM_MR	0x1F0 /* mode register */
#define SUNXI_IO_DRAM_EMR	0x1F4 /* extended mode register */
#define SUNXI_IO_DRAM_EMR2	0x1F8 /* extended mode register */
#define SUNXI_IO_DRAM_EMR3	0x1FC /* extended mode register */

#define SUNXI_IO_DRAM_DLLCR0	0x204 /* dll control register 0(byte 0) */
#define SUNXI_IO_DRAM_DLLCR1	0x208 /* dll control register 1(byte 1) */
#define SUNXI_IO_DRAM_DLLCR2	0x20C /* dll control register 2(byte 2) */
#define SUNXI_IO_DRAM_DLLCR3	0x210 /* dll control register 3(byte 3) */
#define SUNXI_IO_DRAM_DLLCR4	0x214 /* dll control register 4(byte 4) */

static int
dram_parameters_read(struct dram_para *dram_para)
{
	void *base;
	unsigned int zqcr0, dcr;
	unsigned int dllcr0, dllcr1, dllcr2, dllcr3, dllcr4;

	base = mmap(NULL, SUNXI_IO_DRAM_SIZE, PROT_READ,
		    MAP_SHARED, devmem_fd, SUNXI_IO_DRAM_BASE);
	if (base == MAP_FAILED) {
		fprintf(stderr, "Failed to map dram registers: %s\n",
			strerror(errno));
		return errno;
	}

	dram_para->tpr0 = sunxi_io_read(base, SUNXI_IO_DRAM_TPR0);
	dram_para->tpr1 = sunxi_io_read(base, SUNXI_IO_DRAM_TPR1);
	dram_para->tpr2 = sunxi_io_read(base, SUNXI_IO_DRAM_TPR2);

	dllcr0 = (sunxi_io_read(base, SUNXI_IO_DRAM_DLLCR0) >> 6) & 0x3F;
	dllcr1 = (sunxi_io_read(base, SUNXI_IO_DRAM_DLLCR1) >> 14) & 0x0F;
	dllcr2 = (sunxi_io_read(base, SUNXI_IO_DRAM_DLLCR2) >> 14) & 0x0F;
	dllcr3 = (sunxi_io_read(base, SUNXI_IO_DRAM_DLLCR3) >> 14) & 0x0F;
	dllcr4 = (sunxi_io_read(base, SUNXI_IO_DRAM_DLLCR4) >> 14) & 0x0F;

	dram_para->tpr3 = (dllcr0 << 16) |
		(dllcr4 << 12) | (dllcr3 << 8) | (dllcr2 << 4) | dllcr1;

	if (soc_version == SUNXI_SOC_SUN7I) {
		if (sunxi_io_read(base, SUNXI_IO_DRAM_CCR) & 0x20)
			dram_para->tpr4 |= 0x01;
		if (!(sunxi_io_read(base, SUNXI_IO_DRAM_ZQCR1) & 0x01000000))
			dram_para->tpr4 |= 0x02;
	}

	dram_para->cas = (sunxi_io_read(base, SUNXI_IO_DRAM_MR) >> 4) & 0x0F;
	dram_para->emr1 = sunxi_io_read(base, SUNXI_IO_DRAM_EMR);
	dram_para->emr2 = sunxi_io_read(base, SUNXI_IO_DRAM_EMR2);
	dram_para->emr3 = sunxi_io_read(base, SUNXI_IO_DRAM_EMR3);

	dram_para->odt_en = sunxi_io_read(base, SUNXI_IO_DRAM_IOCR) & 0x03;
	zqcr0 = sunxi_io_read(base, SUNXI_IO_DRAM_ZQCR0);
	dram_para->zq = (zqcr0 & 0xf0000000) |
		((zqcr0 >> 20) & 0xff) |
		((zqcr0 & 0xfffff) << 8);

	dcr = sunxi_io_read(base, SUNXI_IO_DRAM_DCR);
	if (dcr & 0x01) {
		dram_para->cas += 4;
		dram_para->type = 3;
	} else
		dram_para->type = 2;

	dram_para->density = (1 << ((dcr >> 3) & 0x07)) * 256;
	dram_para->rank_num = ((dcr >> 10) & 0x03) + 1;
	dram_para->io_width = ((dcr >> 1) & 0x03) * 8;
	dram_para->bus_width = (((dcr >> 6) & 3) + 1) * 8;

	munmap(base, SUNXI_IO_DRAM_SIZE);

	return 0;
}

/*
 * Print a dram.c that can be stuck immediately into u-boot.
 */
void
dram_para_print_uboot(struct dram_para *dram_para)
{
	printf("// place this file in board/sunxi/ in u-boot\n");
	printf("/* this file is generated, don't edit it yourself */\n");
	printf("\n");
	printf("#include \"common.h\"\n");
	printf("#include <asm/arch/dram.h>\n");
	printf("\n");
	printf("static struct dram_para dram_para = {\n");
	printf("\t.clock = %d,\n", dram_para->clock);
	printf("\t.type = %d,\n", dram_para->type);
	printf("\t.rank_num = %d,\n", dram_para->rank_num);
	printf("\t.density = %d,\n", dram_para->density);
	printf("\t.io_width = %d,\n", dram_para->io_width);
	printf("\t.bus_width = %d,\n", dram_para->bus_width);
	printf("\t.cas = %d,\n", dram_para->cas);
	printf("\t.zq = 0x%02x,\n", dram_para->zq);
	printf("\t.odt_en = %d,\n", dram_para->odt_en);
	printf("\t.size = !!! FIXME !!!, /* in MiB */\n");
	printf("\t.tpr0 = 0x%08x,\n", dram_para->tpr0);
	printf("\t.tpr1 = 0x%04x,\n", dram_para->tpr1);
	printf("\t.tpr2 = 0x%05x,\n", dram_para->tpr2);
	printf("\t.tpr3 = 0x%02x,\n", dram_para->tpr3);
	printf("\t.tpr4 = 0x%02x,\n", dram_para->tpr4);
	printf("\t.tpr5 = 0x%02x,\n", dram_para->tpr5);
	printf("\t.emr1 = 0x%02x,\n", dram_para->emr1);
	printf("\t.emr2 = 0x%02x,\n", dram_para->emr2);
	printf("\t.emr3 = 0x%02x,\n", dram_para->emr3);
	printf("};\n");
	printf("\n");
	printf("unsigned long sunxi_dram_init(void)\n");
	printf("{\n");
	printf("\treturn dramc_init(&dram_para);\n");
	printf("}\n");
}

/*
 * Print output matching the .fex output, so it can be stuck in a
 * fex file directly.
 */
void
dram_para_print_fex(struct dram_para *dram_para)
{
	printf("; Insert this section into your .fex file\n");
	printf("[dram_para]\n");
	printf("dram_baseaddr = 0x40000000\n");
	printf("dram_clk = %d\n", dram_para->clock);
	printf("dram_type = %d\n", dram_para->type);
	printf("dram_rank_num = %d\n", dram_para->rank_num);
	printf("dram_chip_density = %d\n", dram_para->density);
	printf("dram_io_width = %d\n", dram_para->io_width);
	printf("dram_bus_width = %d\n", dram_para->bus_width);
	printf("dram_cas = %d\n", dram_para->cas);
	printf("dram_zq = 0x%02x\n", dram_para->zq);
	printf("dram_odt_en = %d\n", dram_para->odt_en);
	printf("dram_size = !!! FIXME !!!\n");
	printf("dram_tpr0 = 0x%08x\n", dram_para->tpr0);
	printf("dram_tpr1 = 0x%04x\n", dram_para->tpr1);
	printf("dram_tpr2 = 0x%05x\n", dram_para->tpr2);
	printf("dram_tpr3 = 0x%02x\n", dram_para->tpr3);
	printf("dram_tpr4 = 0x%02x\n", dram_para->tpr4);
	printf("dram_tpr5 = 0x%02x\n", dram_para->tpr5);
	printf("dram_emr1 = 0x%02x\n", dram_para->emr1);
	printf("dram_emr2 = 0x%02x\n", dram_para->emr2);
	printf("dram_emr3 = 0x%02x\n", dram_para->emr3);
}

static void
print_usage(const char *name)
{
	printf("Utility to retrieve DRAM information from registers on "
	       "Allwinner SoCs.\n");
	printf("\n");
	printf("This is part of the sunxi-tools package from the sunxi "
	       "project. ");
	printf("For more \ninformation visit "
	       "http://linux-sunxi.org/Sunxi-tools.\n");
	printf("\n");
	printf("Usage: %s [OPTION]\n", name);
	printf("\n");
	printf("Options:\n");
	printf("  -f: print in FEX format (default).\n");
	printf("  -u: print in sunxi U-Boot dram.c file format.\n");
	printf("  -h: print this usage information.\n");
}

int
main(int argc, char *argv[])
{
	struct dram_para dram_para = {0};
	bool uboot;
	int ret;

	if (argc == 2) {
		if (argv[1][0] == '-') {
			if (argv[1][1] == 'f')
				uboot = false;
			else if (argv[1][1] == 'u')
				uboot = true;
			else if (argv[1][1] == 'h')
				 goto help;
			else if ((argv[1][1] == '-') && (argv[1][2] == 'h'))
				goto help;
			else
				goto usage;

			if (argv[1][2] != 0)
				goto usage;
		} else
			goto usage;
	} else if (argc == 1)
		uboot = false;
	else
		goto usage;

	devmem_fd = open(DEVMEM_FILE, O_RDWR);
	if (devmem_fd == -1) {
		fprintf(stderr, "Error: failed to open %s: %s\n", DEVMEM_FILE,
			strerror(errno));
		return errno;
	}

	ret = soc_version_read();
	if (ret)
		return ret;
	ret = soc_version_check();
	if (ret)
		return ret;

	ret = dram_parameters_read(&dram_para);
	if (ret)
		return ret;

	ret = dram_clock_read(&dram_para);
	if (ret)
		return ret;

	if (uboot)
		dram_para_print_uboot(&dram_para);
	else
		dram_para_print_fex(&dram_para);

	return 0;

 usage:
	fprintf(stderr, "Error: wrong argument(s).\n");
	print_usage(argv[0]);
	return EINVAL;
 help:
	print_usage(argv[0]);
	return 0;
}
