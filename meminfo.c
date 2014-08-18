/*
 * A10-meminfo
 * Dumps DRAM controller settings
 *
 * Author: Floris Bos
 * License: GPL
 *
 * Compile with: gcc -static -o a10-meminfo-static a10-meminfo.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/io.h>

typedef uint32_t u32;

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

#define SUNXI_IO_DRAM_DCR	0x004 /* dram configuration */
#define SUNXI_IO_DRAM_IOCR	0x008 /* i/o configuration */

#define SUNXI_IO_DRAM_TPR0	0x014 /* dram timing parameters register 0 */
#define SUNXI_IO_DRAM_TPR1	0x018 /* dram timing parameters register 1 */
#define SUNXI_IO_DRAM_TPR2	0x01C /* dram timing parameters register 2 */

#define SUNXI_IO_DRAM_ZQCR0	0x0A8 /* zq control register 0 */

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

int main(int argc, char **argv)
{
	struct dram_para p = {0};
	int ret;

	devmem_fd = open(DEVMEM_FILE, O_RDWR);
	if (devmem_fd == -1) {
		fprintf(stderr, "Error: failed to open %s: %s\n", DEVMEM_FILE,
			strerror(errno));
		return errno;
	}

	ret = dram_parameters_read(&p);
	if (ret)
		return ret;

	ret = dram_clock_read(&p);
	if (ret)
		return ret;

	dram_para_print_uboot(&p);

    return 0;
}

