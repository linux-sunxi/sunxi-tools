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
struct sun4i_dram_para {
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
	SUNXI_SOC_SUN9I = 0x1667, /* A33 */
	SUNXI_SOC_SUN10I = 0x1635, /* A80 */
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

/*
 * Read DRAM clock.
 */
#define SUNXI_IO_CCM_BASE	0x01C20000
#define SUNXI_IO_CCM_SIZE	0x00001000

#define SUNXI_IO_CCM_PLL5_CFG	0x20

static int
sunxi_dram_clock_read(unsigned int *clock)
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

	switch (soc_version) {
	case SUNXI_SOC_SUN6I:
	case SUNXI_SOC_SUN8I:
		n++;
		break;
	default:
		break;
	}

	*clock = (24 * n * k) / m;

	return 0;
}

struct regs {
	int offset;
	char *name;
};

static int
dram_registers_print(unsigned int address, int size, const struct regs *regs,
		     const char *description, const char *prefix)
{
	void *base;
	int i, j;

	base = mmap(NULL, size, PROT_READ, MAP_SHARED, devmem_fd, address);
	if (base == MAP_FAILED) {
		fprintf(stderr, "Failed to map %s registers: %s\n",
			description, strerror(errno));
		return errno;
	}

	printf("/*\n");
	printf(" * %s Registers\n", description);
	printf(" */\n");

	for (i = 0; i < size; i += 4) {
		unsigned int reg = sunxi_io_read(base, i);

		for (j = 0; regs[j].name; j++)
			if (i == regs[j].offset) {
				printf("%s = 0x%08x;\n", regs[j].name, reg);
			}

		if (reg && !regs[j].name)
			printf("%s_%03X = 0x%08x;\n", prefix, i, reg);
	}

	printf("\n");

	munmap(base, size);

	return 0;
}

static int
dram_register_range_print(unsigned int address, int size,
			  const char *description, const char *prefix)
{
	void *base;
	int i;

	base = mmap(NULL, size, PROT_READ, MAP_SHARED, devmem_fd, address);
	if (base == MAP_FAILED) {
		fprintf(stderr, "Failed to map %s registers: %s\n",
			description, strerror(errno));
		return errno;
	}

	printf("/*\n");
	printf(" * %s Registers\n", description);
	printf(" */\n");

	for (i = 0; i < size; i += 4) {
		unsigned int reg = sunxi_io_read(base, i);

		if (reg)
			printf("%s_%03X = 0x%08x;\n", prefix, i, reg);
	}

	printf("\n");

	munmap(base, size);

	return 0;
}

/*
 * Read DRAM parameters.
 */
#define SUN4I_IO_DRAM_BASE	0x01C01000
#define SUN4I_IO_DRAM_SIZE	0x00001000

#define SUN4I_IO_DRAM_CCR	0x000 /* controller configuration register */
#define SUN4I_IO_DRAM_DCR	0x004 /* dram configuration */
#define SUN4I_IO_DRAM_IOCR	0x008 /* i/o configuration */

#define SUN4I_IO_DRAM_TPR0	0x014 /* dram timing parameters register 0 */
#define SUN4I_IO_DRAM_TPR1	0x018 /* dram timing parameters register 1 */
#define SUN4I_IO_DRAM_TPR2	0x01C /* dram timing parameters register 2 */

#define SUN4I_IO_DRAM_ZQCR0	0x0A8 /* zq control register 0 */
#define SUN4I_IO_DRAM_ZQCR1	0x0AC /* zq control register 1 */

#define SUN4I_IO_DRAM_MR	0x1F0 /* mode register */
#define SUN4I_IO_DRAM_EMR	0x1F4 /* extended mode register */
#define SUN4I_IO_DRAM_EMR2	0x1F8 /* extended mode register */
#define SUN4I_IO_DRAM_EMR3	0x1FC /* extended mode register */

#define SUN4I_IO_DRAM_DLLCR0	0x204 /* dll control register 0(byte 0) */
#define SUN4I_IO_DRAM_DLLCR1	0x208 /* dll control register 1(byte 1) */
#define SUN4I_IO_DRAM_DLLCR2	0x20C /* dll control register 2(byte 2) */
#define SUN4I_IO_DRAM_DLLCR3	0x210 /* dll control register 3(byte 3) */
#define SUN4I_IO_DRAM_DLLCR4	0x214 /* dll control register 4(byte 4) */

static int
sun4i_dram_parameters_read(struct sun4i_dram_para *dram_para)
{
	void *base;
	unsigned int zqcr0, dcr;
	unsigned int dllcr0, dllcr1, dllcr2, dllcr3, dllcr4;

	base = mmap(NULL, SUN4I_IO_DRAM_SIZE, PROT_READ,
		    MAP_SHARED, devmem_fd, SUN4I_IO_DRAM_BASE);
	if (base == MAP_FAILED) {
		fprintf(stderr, "Failed to map dram registers: %s\n",
			strerror(errno));
		return errno;
	}

	dram_para->tpr0 = sunxi_io_read(base, SUN4I_IO_DRAM_TPR0);
	dram_para->tpr1 = sunxi_io_read(base, SUN4I_IO_DRAM_TPR1);
	dram_para->tpr2 = sunxi_io_read(base, SUN4I_IO_DRAM_TPR2);

	dllcr0 = (sunxi_io_read(base, SUN4I_IO_DRAM_DLLCR0) >> 6) & 0x3F;
	dllcr1 = (sunxi_io_read(base, SUN4I_IO_DRAM_DLLCR1) >> 14) & 0x0F;
	dllcr2 = (sunxi_io_read(base, SUN4I_IO_DRAM_DLLCR2) >> 14) & 0x0F;
	dllcr3 = (sunxi_io_read(base, SUN4I_IO_DRAM_DLLCR3) >> 14) & 0x0F;
	dllcr4 = (sunxi_io_read(base, SUN4I_IO_DRAM_DLLCR4) >> 14) & 0x0F;

	dram_para->tpr3 = (dllcr0 << 16) |
		(dllcr4 << 12) | (dllcr3 << 8) | (dllcr2 << 4) | dllcr1;

	if (soc_version == SUNXI_SOC_SUN7I) {
		if (sunxi_io_read(base, SUN4I_IO_DRAM_CCR) & 0x20)
			dram_para->tpr4 |= 0x01;
		if (!(sunxi_io_read(base, SUN4I_IO_DRAM_ZQCR1) & 0x01000000))
			dram_para->tpr4 |= 0x02;
	}

	dram_para->cas = (sunxi_io_read(base, SUN4I_IO_DRAM_MR) >> 4) & 0x0F;
	dram_para->emr1 = sunxi_io_read(base, SUN4I_IO_DRAM_EMR);
	dram_para->emr2 = sunxi_io_read(base, SUN4I_IO_DRAM_EMR2);
	dram_para->emr3 = sunxi_io_read(base, SUN4I_IO_DRAM_EMR3);

	dram_para->odt_en = sunxi_io_read(base, SUN4I_IO_DRAM_IOCR) & 0x03;
	zqcr0 = sunxi_io_read(base, SUN4I_IO_DRAM_ZQCR0);
	dram_para->zq = (zqcr0 & 0xf0000000) |
		((zqcr0 >> 20) & 0xff) |
		((zqcr0 & 0xfffff) << 8);

	dcr = sunxi_io_read(base, SUN4I_IO_DRAM_DCR);
	if (dcr & 0x01) {
		dram_para->cas += 4;
		dram_para->type = 3;
	} else
		dram_para->type = 2;

	dram_para->density = (1 << ((dcr >> 3) & 0x07)) * 256;
	dram_para->rank_num = ((dcr >> 10) & 0x03) + 1;
	dram_para->io_width = ((dcr >> 1) & 0x03) * 8;
	dram_para->bus_width = (((dcr >> 6) & 3) + 1) * 8;

	munmap(base, SUN4I_IO_DRAM_SIZE);

	return 0;
}

/*
 * Print a dram.c that can be stuck immediately into u-boot.
 */
void
sun4i_dram_para_print_uboot(struct sun4i_dram_para *dram_para)
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
sun4i_dram_para_print_fex(struct sun4i_dram_para *dram_para)
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

static int
sun4i_dram_para_print(bool uboot)
{
	struct sun4i_dram_para dram_para = {0};
	int ret;

	ret = sunxi_dram_clock_read(&dram_para.clock);
	if (ret)
		return ret;

	ret = sun4i_dram_parameters_read(&dram_para);
	if (ret)
		return ret;

	if (uboot)
		sun4i_dram_para_print_uboot(&dram_para);
	else
		sun4i_dram_para_print_fex(&dram_para);

	return 0;
}

/*
 *
 */
#define SUN6I_IO_DRAMCOM_BASE 0x01C62000
#define SUN6I_IO_DRAMCOM_SIZE 0x0300
#define SUN6I_IO_DRAMCTL_BASE 0x01C63000
#define SUN6I_IO_DRAMCTL_SIZE 0x0400
#define SUN6I_IO_DRAMPHY_BASE 0x01C65000
#define SUN6I_IO_DRAMPHY_SIZE 0x0400

static struct regs
sun6i_dramcom_regs[] = {
	{0x00, "SDR_COM_CR"},
	{0x04, "SDR_COM_CCR"},
	{0x10, "SDR_COM_MFACR"},
	{0x30, "SDR_COM_MSACR"},
	{0x50, "SDR_COM_MBACR"},
	{0, NULL}
};

static struct regs
sun6i_dramctl_regs[] = {
	{0x004, "SDR_SCTL"},
	{0x008, "SDR_SSTAT"},
	{0x040, "SDR_MCMD"},
	{0x04c, "SDR_CMDSTAT"},
	{0x050, "SDR_CMDSTATEN"},
	{0x060, "SDR_MRRCFG0"},
	{0x064, "SDR_MRRSTAT0"},
	{0x068, "SDR_MRRSTAT1"},
	{0x07c, "SDR_MCFG1"},
	{0x080, "SDR_MCFG"},
	{0x084, "SDR_PPCFG"},
	{0x088, "SDR_MSTAT"},
	{0x08c, "SDR_LP2ZQCFG"},
	{0x094, "SDR_DTUSTAT"},
	{0x098, "SDR_DTUNA"},
	{0x09c, "SDR_DTUNE"},
	{0x0a0, "SDR_DTUPRD0"},
	{0x0a4, "SDR_DTUPRD1"},
	{0x0a8, "SDR_DTUPRD2"},
	{0x0ac, "SDR_DTUPRD3"},
	{0x0b0, "SDR_DTUAWDT"},
	{0x0c0, "SDR_TOGCNT1U"},
	{0x0cc, "SDR_TOGCNT100N"},
	{0x0d0, "SDR_TREFI"},
	{0x0d4, "SDR_TMRD"},
	{0x0d8, "SDR_TRFC"},
	{0x0dc, "SDR_TRP"},
	{0x0e0, "SDR_TRTW"},
	{0x0e4, "SDR_TAL"},
	{0x0e8, "SDR_TCL"},
	{0x0ec, "SDR_TCWL"},
	{0x0f0, "SDR_TRAS"},
	{0x0f4, "SDR_TRC"},
	{0x0f8, "SDR_TRCD"},
	{0x0fc, "SDR_TRRD"},
	{0x100, "SDR_TRTP"},
	{0x104, "SDR_TWR"},
	{0x108, "SDR_TWTR"},
	{0x10c, "SDR_TEXSR"},
	{0x110, "SDR_TXP"},
	{0x114, "SDR_TXPDLL"},
	{0x118, "SDR_TZQCS"},
	{0x11c, "SDR_TZQCSI"},
	{0x120, "SDR_TDQS"},
	{0x124, "SDR_TCKSRE"},
	{0x128, "SDR_TCKSRX"},
	{0x12c, "SDR_TCKE"},
	{0x130, "SDR_TMOD"},
	{0x134, "SDR_TRSTL"},
	{0x138, "SDR_TZQCL"},
	{0x13c, "SDR_TMRR"},
	{0x140, "SDR_TCKESR"},
	{0x144, "SDR_TDPD"},
	{0x200, "SDR_DTUWACTL"},
	{0x204, "SDR_DTURACTL"},
	{0x208, "SDR_DTUCFG"},
	{0x20c, "SDR_DTUECTL"},
	{0x210, "SDR_DTUWD0"},
	{0x214, "SDR_DTUWD1"},
	{0x218, "SDR_DTUWD2"},
	{0x21c, "SDR_DTUWD3"},
	{0x220, "SDR_DTUWDM"},
	{0x224, "SDR_DTURD0"},
	{0x224, "SDR_DTURD1"},
	{0x22c, "SDR_DTURD2"},
	{0x230, "SDR_DTURD3"},
	{0x234, "SDR_DTULFSRWD"},
	{0x238, "SDR_DTULFSRRD"},
	{0x23c, "SDR_DTUEAF"},
	{0x240, "SDR_DFITCTLDLY"},
	{0x244, "SDR_DFIODTCFG"},
	{0x248, "SDR_DFIODTCFG1"},
	{0x24c, "SDR_DFIODTRMAP"},
	{0x250, "SDR_DFITPHYWRD"},
	{0x254, "SDR_DFITPHYWRL"},
	{0x260, "SDR_DFITRDDEN"},
	{0x264, "SDR_DFITPHYRDL"},
	{0x270, "SDR_DFITPHYUPDTYPE0"},
	{0x274, "SDR_DFITPHYUPDTYPE1"},
	{0x278, "SDR_DFITPHYUPDTYPE2"},
	{0x27c, "SDR_DFITPHYUPDTYPE3"},
	{0x280, "SDR_DFITCTRLUPDMIN"},
	{0x284, "SDR_DFITCTRLUPDMAX"},
	{0x288, "SDR_DFITCTRLUPDDLY"},
	{0x290, "SDR_DFIUPDCFG"},
	{0x294, "SDR_DFITREFMSKI"},
	{0x298, "SDR_DFITCRLUPDI"},
	{0x2ac, "SDR_DFITRCFG0"},
	{0x2b0, "SDR_DFITRSTAT0"},
	{0x2b4, "SDR_DFITRWRLVLEN"},
	{0x2b8, "SDR_DFITRRDLVLEN"},
	{0x2bc, "SDR_DFITRRDLVLGATEEN"},
	{0x2c4, "SDR_DFISTCFG0"},
	{0x2c8, "SDR_DFISTCFG1"},
	{0x2d0, "SDR_DFITDRAMCLKEN"},
	{0x2d4, "SDR_DFITDRAMCLKDIS"},
	{0x2f0, "SDR_DFILPCFG0"},
	{0, NULL}
};

static struct regs
sun6i_dramphy_regs[] = {
	{0x004, "SDR_PIR"},
	{0x008, "SDR_PGCR"},
	{0x00c, "SDR_PGSR"},
	{0x010, "SDR_DLLGCR"},
	{0x014, "SDR_ACDLLCR"},
	{0x018, "SDR_PTR0"},
	{0x01c, "SDR_PTR1"},
	{0x020, "SDR_PTR2"},
	{0x024, "SDR_ACIOCR"},
	{0x028, "SDR_DXCCR"},
	{0x02c, "SDR_DSGCR"},
	{0x030, "SDR_DCR"},
	{0x034, "SDR_DTPR0"},
	{0x038, "SDR_DTPR1"},
	{0x03c, "SDR_DTPR2"},
	{0x040, "SDR_MR0"},
	{0x044, "SDR_MR1"},
	{0x048, "SDR_MR2"},
	{0x04c, "SDR_MR3"},
	{0x050, "SDR_ODTCR"},
	{0x054, "SDR_DTAR"},
	{0x058, "SDR_DTDT0"},
	{0x05c, "SDR_DTDT1"},
	{0x0c0, "SDR_DCUAR"},
	{0x0c4, "SDR_DCUDR"},
	{0x0c8, "SDR_DCURR"},
	{0x0cc, "SDR_DCULR"},
	{0x0d0, "SDR_DCUGCR"},
	{0x0d4, "SDR_DCUTPR"},
	{0x0d8, "SDR_DCUSR0"},
	{0x0dc, "SDR_DCUSR1"},
	{0x100, "SDR_BISTRR"},
	{0x104, "SDR_BISTMSKR0"},
	{0x108, "SDR_BISTMSKR1"},
	{0x10c, "SDR_BISTWCR"},
	{0x110, "SDR_BISTLSR"},
	{0x114, "SDR_BISTAR0"},
	{0x118, "SDR_BISTAR1"},
	{0x11c, "SDR_BISTAR2"},
	{0x120, "SDR_BISTUDPR"},
	{0x124, "SDR_BISTGSR"},
	{0x128, "SDR_BISTWER"},
	{0x12c, "SDR_BISTBER0"},
	{0x130, "SDR_BISTBER1"},
	{0x134, "SDR_BISTBER2"},
	{0x138, "SDR_BISTWCSR"},
	{0x13c, "SDR_BISTFWR0"},
	{0x140, "SDR_BISTFWR1"},
	{0x180, "SDR_ZQ0CR0"},
	{0x184, "SDR_ZQ0CR1"},
	{0x188, "SDR_ZQ0SR0"},
	{0x18c, "SDR_ZQ0SR1"},
	{0x1c0, "SDR_DX0GCR"},
	{0x1c4, "SDR_DX0GSR0"},
	{0x1c8, "SDR_DX0GSR1"},
	{0x1cc, "SDR_DX0DLLCR"},
	{0x1d0, "SDR_DX0DQTR"},
	{0x1d4, "SDR_DX0DQSTR"},
	{0x200, "SDR_DX1GCR"},
	{0x204, "SDR_DX1GSR0"},
	{0x208, "SDR_DX1GSR1"},
	{0x20c, "SDR_DX1DLLCR"},
	{0x210, "SDR_DX1DQTR"},
	{0x214, "SDR_DX1DQSTR"},
	{0x240, "SDR_DX2GCR"},
	{0x244, "SDR_DX2GSR0"},
	{0x248, "SDR_DX2GSR1"},
	{0x24c, "SDR_DX2DLLCR"},
	{0x250, "SDR_DX2DQTR"},
	{0x254, "SDR_DX2DQSTR"},
	{0x280, "SDR_DX3GCR"},
	{0x284, "SDR_DX3GSR0"},
	{0x288, "SDR_DX3GSR1"},
	{0x28c, "SDR_DX3DLLCR"},
	{0x290, "SDR_DX3DQTR"},
	{0x294, "SDR_DX3DQSTR"},
	{0, NULL}
};

static int
sun6i_dram_regs_print(void)
{
	unsigned int clock;
	int ret;

	ret = sunxi_dram_clock_read(&clock);
	if (ret)
		return ret;

	printf("DRAM Clock: %dMHz\n", clock);

	ret = dram_registers_print(SUN6I_IO_DRAMCOM_BASE,
				   SUN6I_IO_DRAMCOM_SIZE,
				   &sun6i_dramcom_regs[0],
				   "DRAM COM", "SDR_COM");
	if (ret)
		return ret;

	ret = dram_registers_print(SUN6I_IO_DRAMCTL_BASE,
				   SUN6I_IO_DRAMCTL_SIZE,
				   &sun6i_dramctl_regs[0],
				   "DRAM CTL", "SDR_CTL");
	if (ret)
		return ret;

	ret = dram_registers_print(SUN6I_IO_DRAMPHY_BASE,
				   SUN6I_IO_DRAMPHY_SIZE,
				   &sun6i_dramphy_regs[0],
				   "DRAM PHY", "SDR_PHY");
	if (ret)
		return ret;

	return 0;
}

/*
 *
 */
static int
sun8i_dram_regs_print(void)
{
	unsigned int clock;
	int ret;

	ret = sunxi_dram_clock_read(&clock);
	if (ret)
		return ret;

	printf("DRAM Clock: %dMHz\n", clock);

	ret = dram_register_range_print(SUN6I_IO_DRAMCOM_BASE,
					SUN6I_IO_DRAMCOM_SIZE,
					"DRAM COM", "SDR_COM");
	if (ret)
		return ret;


	ret = dram_register_range_print(SUN6I_IO_DRAMCTL_BASE,
					SUN6I_IO_DRAMCTL_SIZE,
					"DRAM CTL", "SDR_CTL");
	if (ret)
		return ret;

	ret = dram_register_range_print(SUN6I_IO_DRAMPHY_BASE,
					SUN6I_IO_DRAMPHY_SIZE,
					"DRAM PHY", "SDR_PHY");
	if (ret)
		return ret;

	return 0;
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
	switch (soc_version) {
	case SUNXI_SOC_SUN4I:
	case SUNXI_SOC_SUN5I:
	case SUNXI_SOC_SUN7I:
		return sun4i_dram_para_print(uboot);
	case SUNXI_SOC_SUN6I:
		return sun6i_dram_regs_print();
	case SUNXI_SOC_SUN8I:
		return sun8i_dram_regs_print();
	default:
		fprintf(stderr, "Error: unknown or unhandled Soc: 0x%04X\n",
			soc_version);
		return -1;
	}

 usage:
	fprintf(stderr, "Error: wrong argument(s).\n");
	print_usage(argv[0]);
	return EINVAL;
 help:
	print_usage(argv[0]);
	return 0;
}
