# Allwinner FEL operation

The FEL protocol is a proprietary USB protocol developed and solely used by
Allwinner. The device side is implemented in the Boot-ROM of all Allwinner SoCs.
The purpose of this protocol is to give easy debug and bootstrapping access
to the SoC's operation, allowing to upload and execute code without any other
media connected. This eventually allows to boot operating systems solely via
the USB interface, or, with the right payload code, to access peripherals on
the board.

Upon leaving the initial reset state, the primary core starts executing
Boot-ROM code. One of the first checks is for the status of the "FEL button"
pin: if this is connected to GND, the Boot-ROM starts executing FEL code. The
same is true when all of the other media boot methods fail, or when user code
later jumps to a certain Boot-ROM address (typically offset 0x20).

The Boot-ROM FEL code sets up the MUSB USB0 controller in peripheral mode, and
waits for connections from a host. The protocol uses four commands:
- `AW_FEL_VERSION`: returns very basic protocol and SoC information. This mostly
  just contains a signature and a protocol version number, to confirm that this
  is really a device using the FEL protocol, and the 16-bit SoC-ID, to identify
  the SoC model.
- `AW_FEL_1_WRITE`: writes data into the specified memory area of the SoC.
  Theoretically any memory location can be used, including MMIO regions, but the
  accesses are done using byte-wide accessors, so are incompatible with most
  devices' requirements for 32-bit wide accesses. So in practice those writes
  must be to either SRAM or DRAM.
- `AW_FEL_1_READ`: reads data from the specified memory area of the SoC. The
  same byte-wide accesses as for writes are used, so the same limitations
  apply: only read from SRAM or DRAM.
- `AW_FEL_1_EXEC`: branches to the specified memory address to execute code.
  The Boot-ROM runs in 32-bit mode, so at least the initial instructions
  must be in AArch32 ARM mode, even on 64-bit SoCs.
  The user code can return to the Boot-ROM FEL code by simply executing a
  `bx lr` instruction, provided the state of the SoC hasn't been changed in
  a way which makes it incompatible with the Boot-ROM FEL code.
  Alternatively the code can just start running user code without considering
  the Boot-ROM anymore, there is no requirement to return to it. The USB
  device would sooner or later be disconnected then.

The protocol just defines those four primitives, modelled by the `ver`, `write`,
`read`, `exec` commands of the sunxi-fel tool. Those command always work, even
if a specific SoC model is not yet supported by sunxi-fel.
Any other functionality is achieved by uploading custom code into some SRAM
area, executing that code, and read its results back from SRAM, after the
custom code returned to the Boot-ROM.

# sunxi-fel new SoC support guide

The `sunxi-fel` tool is an invaluable tool for development, especially for
early bringup of new SoCs. As such, sunxi-fel should support new SoCs
as soon as possible. This document contains explanations on what needs to
be done to add support for a new SoC.

## soc-info.c

Most of the time all it takes to support a new SoC is to add a new member
to the `soc_info_table[]` array in `soc-info.c`. This means providing values
to as many members of the `soc_info_t` struct as possible and needed. A good
start is to find a SoC that is close (SoC-ID is numerically close), and take
that entry as a template, walking through every member and adjust the values
as needed.

- `soc_id`: The 16-bit number identifying the SoC.
  This number will be reported by sunxi-fel (in hex) even without supporting
  the SoC:

      AWUSBFEX soc=00001855(unknown) 00000001 ver=0001 44 08 scratchpad=00007e00 00000000 00000000
                       ^^^^

  Sometimes this value can also be found in BSP source code, or read from the
  SID, with a special sequence.
- `name`: A string with the short name of the SoC, like `"A133"` or `"T527"`.
- `spl_addr`: This is the address where the BootROM(BROM) will load an eGON
  wrapped payload to. Often this is the start of the first SRAM block (SRAM A1),
  though newer SoCs tend to deviate here. This must be the same value where the
  BROM will load boot0/SPL from SD card, eMMC or SPI flash, to allow the same
  SPL binary to work with any load method.
  The SRAM address(es) should be mentioned in the memory map of the manual, but
  it's best to confirm this by loading a small code snippet from SD card and
  printing the load address.
- `scratch_addr`: Address of some memory region where small code snippets can be
  uploaded and executed. That's mostly used for smaller code, with no more than
  a few dozen bytes, but the SPI flash code uploads almost 500 bytes. All SoCs
  so far choose 4KB into SRAM A1, so `spl_addr` + 0x1000, which typically
  grows towards the BROM stack.
- `thunk_addr`: Address of some memory region where the SPL save-and-restore
  code will be loaded to. This needs to be in an area which will not be needed
  or touched by neither the SPL payload nor the BootROM. At the moment we
  require at most 332 bytes, but choosing 512 free bytes would be recommended.
  A common choice is towards the end of the available SRAM blocks, to leave as
  much space for the SPL as possible, but outside of any areas that might be
  used by the BootROM. This can be tested by writing some data there, then
  doing some FEL operations, and reading back from the area to verify the
  identity with the written data. A safer way it to look at BootROM
  disassembly, to identify the location of the normal and IRQ stack, and the
  addresses of data buffers used by the BootROM code. Typically the BROM uses
  some areas near the beginning and some areas towards the end of the SRAM.
  A 512-byte area below the lowest address of the end region is a good choice.
- `thunk_size`: The available space at `thunk_addr`. 512 bytes are recommended,
  but a smaller region might be sufficient, if space is really tight.
- `needs_l2en`: Boolean flag to determine whether payload code must enable
  the L2 cache. This is only needed for the two oldest SoCs, and can be left
  out (set to `false`) for every other SoC.
- `mmu_tt_addr`: Address of some memory area to hold the page tables during
  the FEL operation. Only needed on older SoCs that enable and require the
  MMU during FEL operation. Can be left out (set to 0) for newer SoCs.
- `sid_base`: MMIO base address of the SID register block. Can be found in
  the memory map in the manual or in BSP source code.
- `sid_offset`: Offset of the register address of the "SID root key" within
  the SID MMIO region. Typically this is the beginning of the memory mapped
  area representing the eFuses, and is almost universally 0x200 for newer SoCs.
  The value can be found in the SID register description in the manual, or
  in BSP source code.
- `sid_sections`: Pointer to an array of `sid_section` structs for describing
  the layout and usage of the various SID eFuse bits. Can be set to
  `generic_2k_sid_maps` for the initial submission, to be refined later.
  Typically this layout is not mentioned in the manuals, but some BSP source
  code drops contain some hints.
- `rvbar_reg`: For 64-bit SoCs, the MMIO address of the RVBARARRD0_L register.
  This register contains the address where code execution will start after a
  reset to AArch64. The address can be found in the manual, or lifted from
  either BSP source code or by trying addresses from other SoCs. Needed for
  all ARMv8 SoCs (for the AARCH64 64-bit execution mode).
- `rvbar_reg_alt`: Alternative MMIO address of the RVBARADDR0_L register. Only
  needed for the H616 SoC, which ships in two different die variants, with
  a different location of this register. Can be left out for every other SoC.
- `ver_reg`: MMIO address of the "Version Register", used to identify die
  variants of the otherwise same SoC. The address can be found in the manual,
  but is only needed for H616 SoCs, to differentiate between the differing
  RVBAR addresses.
- `watchdog`: Pointer to a `watchdog_info` struct containing an address and
  the value to write to that address to trigger a watchdog reset. The values
  can be found in the manual, and are required to implement the `wdreset`
  functionality.
- `sid_fix`: Boolean flag to enable a workaround for a broken SID
  implementation, where the MMIO based register reads do not correctly reflect
  the SID eFuses. Only needed on the H3.
- `icache_fix`: Boolean flag to disable the instruction cache when writing
  and executing uploaded code. This is needed on some SoCs before the first
  write, to prevent stale code to be executed.
- `needs_smc_workaround_if_zero_word_at_addr`: An address to read from which
  allows to determine the status of secure boot. Some MMIO or SRAM areas will
  read-as-zero in non-secure state when secure boot is enabled, but contain
  other data when read from secure state. This is used to trigger the `smc #0`
  trick to get back into secure state, which is essential for proper SPL
  operation, and to switch the CPU into AArch64 state. Can be left out if
  FEL always run in secure state, or when the hack does not work. The code
  will issue the `smc #0` when this memory address reads as 0.
- `sram_size`: The size of contiguous and usable SRAM banks, starting at
  `spl_addr`. This should be the summed-up size of all contiguous SRAM banks.
  Any thunks or swap_buffers within this region will be automaticaly removed
  for that size. So that value should simply reflect the maximum amount of SRAM,
  regardless of the location of buffers or BROM stacks.
- `swap_buffers`: Pointer to an array of `sram_swap_buffers` structs, that
  describe regions that need to be backed up before more complex code like
  the SPL is executed. This probably covers the stack and IRQ stack, also
  any buffers used by the BootROM code, as far as they are located in the
  first part of SRAM (A1). Any data used by the BROM towards the end of the
  SRAM does not need to be backed up.

### example SRAM memory map usage on H616

| address | size | area     | user                                            |
|---------|------|----------|-------------------------------------------------|
| 0x00000 |  64K | Boot-ROM |                                                 |
| 0x10000 |  64K | unused   |                                                 |
| 0x20000 |  32K | SRAM A1  | `spl_addr`, Boot-ROM load address for boot0/SPL |
|    "    |   4K |    "     |     used by SPI flash for buffers               |
| 0x21000 |   1K |    "     |     `scratch_addr`, growing into Boot-ROM stack |
| 0x21400 |      |    "     |     Boot-ROM IRQ stack **top**                  |
| 0x28000 | 192K | SRAM C   | contiguous with SRAM A1, used by boot0/SPL      |
| 0x52a00 |   4K |    "     |     `swap_buffers` backup area for IRQ stack    |
| 0x53a00 |17.5K |    "     |     buffer used by Boot-ROM (USB packets)       |
| 0x58000 |      |    "     |     end of SRAM C                               |
