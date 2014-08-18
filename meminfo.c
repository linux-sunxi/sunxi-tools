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

#define SUNXI_DRAMC_BASE    0x01c01000

typedef uint32_t u32;
typedef uint8_t u8;

/*
 * Memory header definition copied from u-boot arch/arm/include/asm/arch-sunxi/dram.h
 * (C) Copyright 2007-2012 Allwinner Technology Co., Ltd. <www.allwinnertech.com>)
 */
struct sunxi_dram_reg {
	u32 ccr;		/* 0x00 controller configuration register */
	u32 dcr;		/* 0x04 dram configuration register */
	u32 iocr;		/* 0x08 i/o configuration register */
	u32 csr;		/* 0x0c controller status register */
	u32 drr;		/* 0x10 dram refresh register */
	u32 tpr0;		/* 0x14 dram timing parameters register 0 */
	u32 tpr1;		/* 0x18 dram timing parameters register 1 */
	u32 tpr2;		/* 0x1c dram timing parameters register 2 */
	u32 gdllcr;		/* 0x20 global dll control register */
	u8 res0[0x28];
	u32 rslr0;		/* 0x4c rank system latency register */
	u32 rslr1;		/* 0x50 rank system latency register */
	u8 res1[0x8];
	u32 rdgr0;		/* 0x5c rank dqs gating register */
	u32 rdgr1;		/* 0x60 rank dqs gating register */
	u8 res2[0x34];
	u32 odtcr;		/* 0x98 odt configuration register */
	u32 dtr0;		/* 0x9c data training register 0 */
	u32 dtr1;		/* 0xa0 data training register 1 */
	u32 dtar;		/* 0xa4 data training address register */
	u32 zqcr0;		/* 0xa8 zq control register 0 */
	u32 zqcr1;		/* 0xac zq control register 1 */
	u32 zqsr;		/* 0xb0 zq status register */
	u32 idcr;		/* 0xb4 initializaton delay configure reg */
	u8 res3[0x138];
	u32 mr;			/* 0x1f0 mode register */
	u32 emr;		/* 0x1f4 extended mode register */
	u32 emr2;		/* 0x1f8 extended mode register */
	u32 emr3;		/* 0x1fc extended mode register */
	u32 dllctr;		/* 0x200 dll control register */
	u32 dllcr[5];	/* 0x204 dll control register 0(byte 0) */
	/* 0x208 dll control register 1(byte 1) */
	/* 0x20c dll control register 2(byte 2) */
	/* 0x210 dll control register 3(byte 3) */
	/* 0x214 dll control register 4(byte 4) */
	u32 dqtr0;		/* 0x218 dq timing register */
	u32 dqtr1;		/* 0x21c dq timing register */
	u32 dqtr2;		/* 0x220 dq timing register */
	u32 dqtr3;		/* 0x224 dq timing register */
	u32 dqstr;		/* 0x228 dqs timing register */
	u32 dqsbtr;		/* 0x22c dqsb timing register */
	u32 mcr;		/* 0x230 mode configure register */
	u8 res[0x8];
	u32 reg_23c;	/* 0x23c register description unknown!!! */
	u32 apr;		/* 0x240 arbiter period register */
	u32 pldtr;		/* 0x244 priority level data threshold reg */
	u8 res5[0x8];
	u32 hpcr[32];	/* 0x250 host port configure register */
	u8 res6[0x10];
	u32 csel;		/* 0x2e0 controller select register */
};

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

volatile void *map_physical_memory(uint32_t addr, size_t len)
{
    volatile void *mem;

    mem = (volatile void *) mmap(NULL, len, PROT_READ, MAP_SHARED, devmem_fd, (off_t) addr);
    
    if (mem == MAP_FAILED)
    {
        perror("mmap");
        exit (1);
    }

    return mem;
}

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
	volatile struct sunxi_dram_reg *r;
	struct dram_para p = {0};
	int ret;

	devmem_fd = open(DEVMEM_FILE, O_RDWR);
	if (devmem_fd == -1) {
		fprintf(stderr, "Error: failed to open %s: %s\n", DEVMEM_FILE,
			strerror(errno));
		return errno;
	}

	r  = map_physical_memory(SUNXI_DRAMC_BASE, 4096);

    /* Convert information found inside registers back to dram_para struct */
    p.tpr0   = r->tpr0;
    p.tpr1   = r->tpr1;
    p.tpr2   = r->tpr2;
    p.tpr3   = ((((r->dllcr[0]) >> 6) & 0x3f) << 16) |
               ((((r->dllcr[1]) >> 14) & 0xf) << 0) |
               ((((r->dllcr[2]) >> 14) & 0xf) << 4) |
               ((((r->dllcr[3]) >> 14) & 0xf) << 8) |
               ((((r->dllcr[4]) >> 14) & 0xf) << 12);
    p.emr1   = r->emr;
    p.emr2   = r->emr2;
    p.emr3   = r->emr3;
    p.type   = (r->dcr & 0x1 ? 3 : 2);
    p.odt_en = (r->iocr & 0x3);
    p.zq     = (r->zqcr0 & 0xf0000000)+(r->zqcr0 >> 20 & 0xff)+((r->zqcr0 & 0xfffff) << 8);
    p.cas    = (r->mr >> 4 & 15);
    if (p.type == 3)
        p.cas += 4;
    p.density  = 1 << (8 + ((r->dcr >> 3) & 7));
    p.rank_num = (r->dcr >> 10 & 3)+1;
    p.io_width = (r->dcr >> 1 & 3) << 3;
    p.bus_width = ((r->dcr >> 6 & 3)+1) << 3;

	ret = dram_clock_read(&p);
	if (ret)
		return ret;

	dram_para_print_uboot(&p);

    /* Clean up */
    munmap((void *)r, 4096);

    return 0;
}

