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
 * representing a singe large contiguous area. Everything is the same as on
 * A10/A13/A20, but just shifted by 0x10000.
 */
sram_swap_buffers a64_sram_swap_buffers[] = {
	/* 0x11C00-0x11FFF (IRQ stack) */
	{ .buf1 = 0x11C00, .buf2 = 0x1A400, .size = 0x0400 },
	/* 0x15C00-0x16FFF (Stack) */
	{ .buf1 = 0x15C00, .buf2 = 0x1A800, .size = 0x1400 },
	/* 0x17C00-0x17FFF (Something important) */
	{ .buf1 = 0x17C00, .buf2 = 0x1BC00, .size = 0x0400 },
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

soc_info_t soc_info_table[] = {
	{
		.soc_id       = 0x1623, /* Allwinner A10 */
		.name         = "A10",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.needs_l2en   = true,
		.sid_base     = 0x01C23800,
	},{
		.soc_id       = 0x1625, /* Allwinner A10s, A13, R8 */
		.name         = "A13",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.needs_l2en   = true,
		.sid_base     = 0x01C23800,
	},{
		.soc_id       = 0x1651, /* Allwinner A20 */
		.name         = "A20",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sid_base     = 0x01C23800,
	},{
		.soc_id       = 0x1650, /* Allwinner A23 */
		.name         = "A23",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0x46E00, .thunk_size = 0x200,
		.swap_buffers = ar100_abusing_sram_swap_buffers,
		.sid_base     = 0x01C23800,
	},{
		.soc_id       = 0x1633, /* Allwinner A31 */
		.name         = "A31",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0x22E00, .thunk_size = 0x200,
		.swap_buffers = a31_sram_swap_buffers,
	},{
		.soc_id       = 0x1667, /* Allwinner A33, R16 */
		.name         = "A33",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0x46E00, .thunk_size = 0x200,
		.swap_buffers = ar100_abusing_sram_swap_buffers,
		.sid_base     = 0x01C23800,
	},{
		.soc_id       = 0x1689, /* Allwinner A64 */
		.name         = "A64",
		.spl_addr     = 0x10000,
		.scratch_addr = 0x11000,
		.thunk_addr   = 0x1A200, .thunk_size = 0x200,
		.swap_buffers = a64_sram_swap_buffers,
		.sid_base     = 0x01C14000,
		.sid_offset   = 0x200,
		.rvbar_reg    = 0x017000A0,
		/* Check L.NOP in the OpenRISC reset vector */
		.needs_smc_workaround_if_zero_word_at_addr = 0x40004,
	},{
		.soc_id       = 0x1639, /* Allwinner A80 */
		.name         = "A80",
		.spl_addr     = 0x10000,
		.scratch_addr = 0x11000,
		.thunk_addr   = 0x23400, .thunk_size = 0x200,
		.swap_buffers = a80_sram_swap_buffers,
		.sid_base     = 0X01C0E000,
		.sid_offset   = 0x200,
	},{
		.soc_id       = 0x1673, /* Allwinner A83T */
		.name         = "A83T",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0x46E00, .thunk_size = 0x200,
		.swap_buffers = ar100_abusing_sram_swap_buffers,
		.sid_base     = 0x01C14000,
		.sid_offset   = 0x200,
	},{
		.soc_id       = 0x1680, /* Allwinner H3, H2+ */
		.name         = "H3",
		.scratch_addr = 0x1000,
		.mmu_tt_addr  = 0x8000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sid_base     = 0x01C14000,
		.sid_offset   = 0x200,
		.sid_fix      = true,
		/* Check L.NOP in the OpenRISC reset vector */
		.needs_smc_workaround_if_zero_word_at_addr = 0x40004,
	},{
		.soc_id       = 0x1681, /* Allwinner V3s */
		.name         = "V3s",
		.scratch_addr = 0x1000,
		.mmu_tt_addr  = 0x8000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sid_base     = 0x01C23800,
	},{
		.soc_id       = 0x1718, /* Allwinner H5 */
		.name         = "H5",
		.spl_addr     = 0x10000,
		.scratch_addr = 0x11000,
		.thunk_addr   = 0x1A200, .thunk_size = 0x200,
		.swap_buffers = a64_sram_swap_buffers,
		.sid_base     = 0x01C14000,
		.sid_offset   = 0x200,
		.rvbar_reg    = 0x017000A0,
		/* Check L.NOP in the OpenRISC reset vector */
		.needs_smc_workaround_if_zero_word_at_addr = 0x40004,
	},{
		.soc_id       = 0x1701, /* Allwinner R40 */
		.name         = "R40",
		.scratch_addr = 0x1000,
		.thunk_addr   = 0xA200, .thunk_size = 0x200,
		.swap_buffers = a10_a13_a20_sram_swap_buffers,
		.sid_base     = 0x01C1B000,
		.sid_offset   = 0x200,
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
