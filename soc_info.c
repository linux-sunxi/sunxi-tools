/*
 * Copyright (C) 2012  Henrik Nordstrom <henrik@henriknordstrom.net>
 * Copyright (C) 2015  Siarhei Siamashka <siarhei.siamashka@gmail.com>
 * Copyright (C) 2016  Bernhard Nortmann <bernhard.nortmann@web.de>
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

/**********************************************************************
 * SoC information and retrieval of soc_sram_info
 **********************************************************************/
#include "soc_info.h"

#include <stdio.h>
#include <string.h>

/*
 * The FEL code from BROM in A10/A13/A20 sets up two stacks for itself. One
 * at 0x2000 (and growing down) for the IRQ handler. And another one at 0x7000
 * (and also growing down) for the regular code. In order to use the whole
 * 32 KiB in the A1/A2 sections of SRAM, we need to temporarily move these
 * stacks elsewhere. And the addresses 0x7D00-0x7FFF contain something
 * important too (overwriting them kills FEL). On A10/A13/A20 we can use
 * the SRAM sections A3/A4 (0x8000-0xBFFF) for this purpose.
 */
sram_swap_buffers a10_a13_a20_sram_swap_buffers[] = {
	/* 0x1C00-0x1FFF (IRQ stack) */
	{ .buf1 = 0x1C00, .buf2 = 0xA400, .size = 0x0400 },
	/* 0x5C00-0x6FFF (Stack) */
	{ .buf1 = 0x5C00, .buf2 = 0xA800, .size = 0x1400 },
	/* 0x7C00-0x7FFF (Something important) */
	{ .buf1 = 0x7C00, .buf2 = 0xBC00, .size = 0x0400 },
	{ .size = 0 }  /* End of the table */
};

/*
 * A31 is very similar to A10/A13/A20, except that it has no SRAM at 0x8000.
 * So we use the SRAM section B at 0x20000-0x2FFFF instead. In the FEL mode,
 * the MMU translation table is allocated by the BROM at 0x20000. But we can
 * also safely use it as the backup storage because the MMU is temporarily
 * disabled during the time of the SPL execution.
 */
sram_swap_buffers a31_sram_swap_buffers[] = {
	{ .buf1 = 0x1800, .buf2 = 0x20000, .size = 0x800 },
	{ .buf1 = 0x5C00, .buf2 = 0x20800, .size = 0x8000 - 0x5C00 },
	{ .size = 0 }  /* End of the table */
};

/*
 * A64 has 32KiB of SRAM A at 0x10000 and a large SRAM C at 0x18000. SRAM A
 * and SRAM C reside in the address space back-to-back without any gaps, thus
 * representing a singe large contiguous area. The BROM FEL code memory areas
 * are the same as on A10/A13/A20, but just shifted by 0x10000.
 * We put the backup buffers towards the end of SRAM C, in a location that
 * is also available on the H5.
 */
sram_swap_buffers a64_sram_swap_buffers[] = {
	/* 0x11C00-0x11FFF (IRQ stack) */
	{ .buf1 = 0x11C00, .buf2 = 0x31400, .size = 0x0400 },
	/* 0x15C00-0x16FFF (Stack) */
	{ .buf1 = 0x15C00, .buf2 = 0x31800, .size = 0x1400 },
	/* 0x17C00-0x17FFF (Something important) */
	{ .buf1 = 0x17C00, .buf2 = 0x32c00, .size = 0x0400 },
	{ .size = 0 }  /* End of the table */
};

/*
 * Use the SRAM section at 0x44000 as the backup storage. This is the memory,
 * which is normally shared with the OpenRISC core (should we do an extra check
 * to ensure that this core is powered off and can't interfere?).
 */
sram_swap_buffers ar100_abusing_sram_swap_buffers[] = {
	{ .buf1 = 0x1800, .buf2 = 0x44000, .size = 0x800 },
	{ .buf1 = 0x5C00, .buf2 = 0x44800, .size = 0x8000 - 0x5C00 },
	{ .size = 0 }  /* End of the table */
};

/*
 * A80 has 40KiB SRAM A1 at 0x10000 where the SPL has to be loaded to. The
 * secure SRAM B at 0x20000 is used as backup area for FEL stacks and data.
 */
sram_swap_buffers a80_sram_swap_buffers[] = {
	{ .buf1 = 0x11800, .buf2 = 0x20000, .size = 0x800 },
	{ .buf1 = 0x15400, .buf2 = 0x20800, .size = 0x18000 - 0x15400 },
	{ .size = 0 }  /* End of the table */
};

/*
 * H6 has 32KiB of SRAM A at 0x20000 and a large SRAM C at 0x28000. SRAM A
 * and SRAM C reside in the address space back-to-back without any gaps, thus
 * representing a singe large contiguous area. Everything is the same as on
 * A10/A13/A20, but just shifted by 0x20000.
 */
sram_swap_buffers h6_sram_swap_buffers[] = {
	/* 0x21C00-0x21FFF (IRQ stack) */
	{ .buf1 = 0x21C00, .buf2 = 0x42400, .size = 0x0400 },
	/* 0x25C00-0x26FFF (Stack) */
	{ .buf1 = 0x25C00, .buf2 = 0x42800, .size = 0x1400 },
	/* 0x27C00-0x27FFF (Something important) */
	{ .buf1 = 0x27C00, .buf2 = 0x43c00, .size = 0x0400 },
	{ .size = 0 }  /* End of the table */
};

/*
 * V831 has 96KiB SRAM A1 at 0x20000 where the SPL has to be loaded to.
 * SRAM C is continuous with SRAM A1, and both SRAMs are tried to be used
 * by BROM. Memory space is allocated both from the start of SRAM A1 and
 * the end of SRAM C.
 * The start of SRAM C is in between these areas, and can serve as backup
 * of IRQ stack, which is inside the first 32KiB of SRAM A1. Other areas
 * that are critical on older SoCs seem to be already in SRAM C, which
 * we do not need to preserve.
 */
sram_swap_buffers v831_sram_swap_buffers[] = {
	{ .buf1 = 0x21000, .buf2 = 0x38000, .size = 0x1000 },
	{ .size = 0 }  /* End of the table */
};

/* H616 situation is the same as V831 one, except it has 32 KiB of SRAM A1. */
sram_swap_buffers h616_sram_swap_buffers[] = {
	{ .buf1 = 0x21000, .buf2 = 0x52a00, .size = 0x1000 },
	{ .size = 0 }  /* End of the table */
};

/*
 * R329 has no SRAM A1, but a huge SRAM A2 at 0x100000. SPL and BROM uses
 * this SRAM A2's first part like how other SoCs use SRAM A1. The sp and
 * sp_irq values checked with thunk are 0x13c2c8 and 0x101400, which looks
 * similar to the situation of V831, in which the stack is quite high.
 */
sram_swap_buffers r329_sram_swap_buffers[] = {
	/* 0x101000-0x101400 (IRQ stack) */
	{ .buf1 = 0x101000, .buf2 = 0x13bc00, .size = 0x0400 },
	{ .size = 0 }  /* End of the table */
};

/*
 * The FEL code from BROM in F1C100s also uses SRAM A in a similar way
 * with A10/A13/A20.
 * Unfortunately the SRAM layout of F1C100s is not documented at all, so
 * we can only try by r/w under FEL mode.
 * The result is that there's a contingous SRAM zone from 0x8800 to 0xb5ff.
 */
sram_swap_buffers f1c100s_sram_swap_buffers[] = {
	/* 0x1C00-0x1FFF (IRQ stack) */
	{ .buf1 = 0x1C00, .buf2 = 0x9000, .size = 0x0400 },
	/* 0x5C00-0x6FFF (Stack) */
	{ .buf1 = 0x5C00, .buf2 = 0x9400, .size = 0x1400 },
	/* 0x7C00-0x7FFF (Something important) */
	{ .buf1 = 0x7C00, .buf2 = 0xa800, .size = 0x0400 },
	{ .size = 0 }  /* End of the table */
};

const watchdog_info wd_a10_compat = {
	.reg_mode = 0x01C20C94,
	.reg_mode_value = 3,
};

const watchdog_info wd_h3_compat = {
	.reg_mode = 0x01C20Cb8,
	.reg_mode_value = 1,
};

const watchdog_info wd_a80 = {
	.reg_mode = 0x06000CB8,
	.reg_mode_value = 1,
};

const watchdog_info wd_h6_compat = {
	.reg_mode = 0x030090b8,
	.reg_mode_value = 1,
};

const watchdog_info wd_v853_compat = {
	.reg_mode = 0x020500b8,
	.reg_mode_value = 0x16aa0001,
};

soc_info_t soc_info_table[] = {
	{
		.soc_id       = 0x1623, /* Allwinner A10 */
		.name         = "A10",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sram_size    = 48 * 1024,
		.needs_l2en   = true,
		.sid_base     = 0x01C23800,
		.watchdog     = &wd_a10_compat,
	},{
		.soc_id       = 0x1625, /* Allwinner A10s, A13, R8 */
		.name         = "A13",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sram_size    = 48 * 1024,
		.needs_l2en   = true,
		.sid_base     = 0x01C23800,
		.watchdog     = &wd_a10_compat,
	},{
		.soc_id       = 0x1651, /* Allwinner A20 */
		.name         = "A20",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sram_size    = 48 * 1024,
		.sid_base     = 0x01C23800,
		.watchdog     = &wd_a10_compat,
	},{
		.soc_id       = 0x1650, /* Allwinner A23 */
		.name         = "A23",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0x46E00, .thunk_size = 0x200,
		.swap_buffers = ar100_abusing_sram_swap_buffers,
		.sram_size    = 64 * 1024,
		.sid_base     = 0x01C23800,
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1633, /* Allwinner A31 */
		.name         = "A31",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0x22E00, .thunk_size = 0x200,
		.swap_buffers = a31_sram_swap_buffers,
		.sram_size    = 32 * 1024,
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1667, /* Allwinner A33, R16 */
		.name         = "A33",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0x46E00, .thunk_size = 0x200,
		.swap_buffers = ar100_abusing_sram_swap_buffers,
		.sram_size    = 32 * 1024,
		.sid_base     = 0x01C23800,
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1689, /* Allwinner A64 */
		.name         = "A64",
		.spl_addr     = 0x10000,
		.scratch_addr = 0x11000,
		.thunk_addr   = 0x31200, .thunk_size = 0x200,
		.swap_buffers = a64_sram_swap_buffers,
		.sram_size    = 140 * 1024,
		.sid_base     = 0x01C14000,
		.sid_offset   = 0x200,
		.rvbar_reg    = 0x017000A0,
		/* Check L.NOP in the OpenRISC reset vector */
		.needs_smc_workaround_if_zero_word_at_addr = 0x40004,
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1639, /* Allwinner A80 */
		.name         = "A80",
		.spl_addr     = 0x10000,
		.scratch_addr = 0x11000,
		.thunk_addr   = 0x23400, .thunk_size = 0x200,
		.swap_buffers = a80_sram_swap_buffers,
		.sram_size    = 40 * 1024,
		.sid_base     = 0X01C0E000,
		.sid_offset   = 0x200,
		.watchdog     = &wd_a80,
	},{
		.soc_id       = 0x1663, /* Allwinner F1C100s (all new sun3i?) */
		.name         = "F1C100s",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xb400, .thunk_size = 0x200,
		.swap_buffers = f1c100s_sram_swap_buffers,
		.sram_size    = 32 * 1024,
		/* No SID */
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1673, /* Allwinner A83T */
		.name         = "A83T",
		.scratch_addr = 0x1000,
		.mmu_tt_addr  = 0x44000,
		.thunk_addr   = 0x46E00, .thunk_size = 0x200,
		.swap_buffers = ar100_abusing_sram_swap_buffers,
		.sram_size    = 32 * 1024,
		.sid_base     = 0x01C14000,
		.sid_offset   = 0x200,
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1680, /* Allwinner H3, H2+ */
		.name         = "H3",
		.scratch_addr = 0x1000,
		.mmu_tt_addr  = 0x8000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sram_size    = 108 * 1024,
		.sid_base     = 0x01C14000,
		.sid_offset   = 0x200,
		.sid_fix      = true,
		/* Check L.NOP in the OpenRISC reset vector */
		.needs_smc_workaround_if_zero_word_at_addr = 0x40004,
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1681, /* Allwinner V3s */
		.name         = "V3s",
		.scratch_addr = 0x1000,
		.mmu_tt_addr  = 0x8000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sram_size    = 60 * 1024,
		.sid_base     = 0x01C23800,
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1718, /* Allwinner H5 */
		.name         = "H5",
		.spl_addr     = 0x10000,
		.scratch_addr = 0x11000,
		.thunk_addr   = 0x31200, .thunk_size = 0x200,
		.swap_buffers = a64_sram_swap_buffers,
		.sram_size    = 140 * 1024,
		.sid_base     = 0x01C14000,
		.sid_offset   = 0x200,
		.rvbar_reg    = 0x017000A0,
		/* Check L.NOP in the OpenRISC reset vector */
		.needs_smc_workaround_if_zero_word_at_addr = 0x40004,
		.watchdog     = &wd_h3_compat,
	},{
		.soc_id       = 0x1701, /* Allwinner R40 */
		.name         = "R40",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sram_size    = 48 * 1024,
		.sid_base     = 0x01C1B000,
		.sid_offset   = 0x200,
		.watchdog     = &wd_a10_compat,
	},{
		.soc_id       = 0x1719, /* Allwinner A63 */
		.name         = "A63",
		.spl_addr     = 0x20000,
		.scratch_addr = 0x21000,
		.thunk_addr   = 0x42200, .thunk_size = 0x200,
		.swap_buffers = h6_sram_swap_buffers,
		.sram_size    = 144 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.rvbar_reg    = 0x09010040,
		.watchdog     = &wd_h6_compat,
	},{
		.soc_id       = 0x1728, /* Allwinner H6 */
		.name         = "H6",
		.spl_addr     = 0x20000,
		.scratch_addr = 0x21000,
		.thunk_addr   = 0x42200, .thunk_size = 0x200,
		.swap_buffers = h6_sram_swap_buffers,
		.sram_size    = 144 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.rvbar_reg    = 0x09010040,
		/* Check L.NOP in the OpenRISC reset vector */
		.needs_smc_workaround_if_zero_word_at_addr = 0x100004,
		.watchdog     = &wd_h6_compat,
	},{
		.soc_id       = 0x1816, /* Allwinner V536 */
		.name         = "V536",
		.spl_addr     = 0x20000,
		.scratch_addr = 0x21000,
		.thunk_addr   = 0x2A200, .thunk_size = 0x200,
		.swap_buffers = v831_sram_swap_buffers,
		.sram_size    = 228 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.watchdog     = &wd_h6_compat,
	},{
		.soc_id       = 0x1817, /* Allwinner V831 */
		.name         = "V831",
		.spl_addr     = 0x20000,
		.scratch_addr = 0x21000,
		.thunk_addr   = 0x2A200, .thunk_size = 0x200,
		.swap_buffers = v831_sram_swap_buffers,
		.sram_size    = 228 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.watchdog     = &wd_h6_compat,
	},{
		.soc_id       = 0x1823, /* Allwinner H616 */
		.name         = "H616",
		.spl_addr     = 0x20000,
		.scratch_addr = 0x21000,
		.thunk_addr   = 0x53a00, .thunk_size = 0x200,
		.swap_buffers = h616_sram_swap_buffers,
		.sram_size    = 207 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.rvbar_reg    = 0x09010040,
		.watchdog     = &wd_h6_compat,
	},{
		.soc_id       = 0x1851, /* Allwinner R329 */
		.name         = "R329",
		.spl_addr     = 0x100000,
		.scratch_addr = 0x101000,
		.mmu_tt_addr  = 0x130000,
		.thunk_addr   = 0x13ba00, .thunk_size = 0x200,
		.swap_buffers = r329_sram_swap_buffers,
		.sram_size    = 1856 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.rvbar_reg    = 0x08100040,
		.watchdog     = &wd_h6_compat,
	},{
		.soc_id       = 0x1886, /* Allwinner V853 */
		.name         = "V853",
		.spl_addr     = 0x20000,
		.scratch_addr = 0x21000,
		.thunk_addr   = 0x3A200, .thunk_size = 0x200,
		.swap_buffers = v831_sram_swap_buffers,
		.sram_size    = 132 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.icache_fix   = true,
		.watchdog     = &wd_v853_compat,
	},{
		.soc_id       = 0x1859, /* Allwinner D1/D1s/R528/T113-S3 */
		.name         = "R528",
		.spl_addr     = 0x20000,
		.scratch_addr = 0x21000,
		.thunk_addr   = 0x3a200, .thunk_size = 0x200,
		.swap_buffers = v831_sram_swap_buffers,
		.sram_size    = 160 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.icache_fix   = true,
		.watchdog     = &wd_v853_compat,
	},{
		.soc_id       = 0x1721, /* Allwinner V5 */
		.name         = "V5",
		.spl_addr     = 0x20000,
		.scratch_addr = 0x21000,
		.thunk_addr   = 0x42200, .thunk_size = 0x200,
		.swap_buffers = h6_sram_swap_buffers,
		.sram_size    = 136 * 1024,
		.sid_base     = 0x03006000,
		.sid_offset   = 0x200,
		.watchdog     = &wd_h6_compat,
	},{
		.swap_buffers = NULL /* End of the table */
	}
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
	{ .buf1 = 0x1C00, .buf2 = 0x5800, .size = 0x400 },
	{ .size = 0 }  /* End of the table */
};

soc_info_t generic_soc_info = {
	.scratch_addr = 0x1000,
	.thunk_addr   = 0x5680, .thunk_size = 0x180,
	.swap_buffers = generic_sram_swap_buffers,
};

/* functions to retrieve SoC information */

soc_info_t *get_soc_info_from_id(uint32_t soc_id)
{
	soc_info_t *soc, *result = NULL;

	for (soc = soc_info_table; soc->swap_buffers; soc++)
		if (soc->soc_id == soc_id) {
			result = soc;
			break;
		}

	if (!result) {
		printf("Warning: no 'soc_sram_info' data for your SoC (id=%04X)\n",
		       soc_id);
		result = &generic_soc_info;
	}
	return result;
}

soc_info_t *get_soc_info_from_version(struct aw_fel_version *buf)
{
	return get_soc_info_from_id(buf->soc_id);
}

void get_soc_name_from_id(soc_name_t buffer, uint32_t soc_id)
{
	soc_info_t *soc;
	for (soc = soc_info_table; soc->swap_buffers; soc++)
		if (soc->soc_id == soc_id && soc->name != NULL) {
			strncpy(buffer, soc->name, sizeof(soc_name_t) - 1);
			return;
		}

	/* unknown SoC (or name string missing), use the hexadecimal ID */
	snprintf(buffer, sizeof(soc_name_t) - 1, "0x%04X", soc_id);
}
