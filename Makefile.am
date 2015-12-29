bin_PROGRAMS = sunxi-nand sunxi-fexc sunxi-phoenix_info sunxi-bootinfo
noinst_PROGRAMS = sunxi-mkboot

if ARM_BUILD
bin_PROGRAMS += sunxi-meminfo sunxi-pio sunxi-script-extractor
endif

sunxi_nand_SOURCES = sunxi-nand.c sunxi-nand.h sunxi-nand-common.c sunxi-nand-common.h sunxi-nand-read.c sunxi-nand-read.h sunxi-nand-sun4i.c sunxi-nand-sun4i.h sunxi-nand-sun7i.c sunxi-nand-sun7i.h sunxi-nand-write.c sunxi-nand-write.h

sunxi_bootinfo_SOURCES = sunxi-bootinfo.c
sunxi_bootinfo_CPPFLAGS = -Iinclude/

sunxi_phoenix_info_SOURCES = sunxi-phoenix_info.c
sunxi_phoenix_info_CPPFLAGS = -Iinclude/

if BUILD_SUNXIFEL
bin_PROGRAMS +=sunxi-fel
sunxi_fel_SOURCES = sunxi-fel.c sunxi-fel-to-spl-thunk.h sunxi-progress.c sunxi-progress.h
sunxi_fel_CPPFLAGS = -Iinclude/ $(USB_CFLAGS)
sunxi_fel_LDADD = $(USB_LDFLAGS)
endif

sunxi_meminfo_SOURCES = sunxi-meminfo.c
sunxi_meminfo_CFLAGS = -static

sunxi_pio_SOURCES = sunxi-pio.c
sunxi_pio_CPPFLAGS = -Iinclude/

sunxi_mkboot_SOURCES = sunxi-mkboot.c

sunxi_script_extractor_SOURCES = sunxi-script-extractor.c

sunxi_fexc_SOURCES = sunxi-fexc.c sunxi-fexc.h sunxi-script.h sunxi-script.c sunxi-script_uboot.h sunxi-script_uboot.c sunxi-script_bin.h sunxi-script_bin.c sunxi-script_fex.h sunxi-script_fex.c

.PHONY: sunxi-bin help


if BUILD_BAREMETAL
sunxi-bin: sunxi-fel-copy.bin sunxi-fel-pio.bin sunxi-jtag-loop.sunxi sunxi-jtag-loop-alt.sunxi sunxi-fel-sdboot.sunxi sunxi-boot_head_sun4i.bin sunxi-boot_head_sun5i.bin


sunxi-fel-copy.elf: sunxi-fel-copy.c
	$(ARM_GCC) -g -Os -fno-common -ffixed-r8 -msoft-float -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder sunxi-fel-copy.c -c -o sunxi-fel-copy.elf

sunxi-fel-copy.bin: sunxi-fel-copy.elf
	$(ARM_CPY) -O binary sunxi-fel-copy.elf sunxi-fel-copy.bin


sunxi-fel-pio.elf: sunxi-fel-pio.c sunxi-fel-pio.lds
	$(ARM_GCC) -g  -Os -fpic  -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder sunxi-fel-pio.c -nostdlib -o sunxi-fel-pio.elf -T sunxi-fel-pio.lds

sunxi-fel-pio.nm: sunxi-fel-pio.elf
	$(ARM_NM) sunxi-fel-pio.elf | grep -v " _" > sunxi-fel-pio.nm

sunxi-fel-pio.bin: sunxi-fel-pio.elf sunxi-fel-pio.nm
	$(ARM_CPY) -O binary sunxi-fel-pio.elf sunxi-fel-pio.bin


sunxi-jtag-loop.elf: sunxi-jtag-loop.c sunxi-jtag-loop.lds
	$(ARM_GCC) -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder sunxi-jtag-loop.c -nostdlib -o sunxi-jtag-loop.elf -T sunxi-jtag-loop.lds -Wl,-N

sunxi-jtag-loop.bin: sunxi-jtag-loop.elf
	$(ARM_CPY) -O binary sunxi-jtag-loop.elf sunxi-jtag-loop.bin

sunxi-jtag-loop.sunxi: sunxi-jtag-loop.bin sunxi-mkboot
	$(srcdir)/sunxi-mkboot sunxi-jtag-loop.bin sunxi-jtag-loop.sunxi
	

sunxi-jtag-loop-alt.elf: sunxi-jtag-loop.S
	 $(ARM_GCC) -g -g -fno-common -ffixed-r8 -msoft-float -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder sunxi-jtag-loop.S -o sunxi-jtag-loop-alt.elf -c

sunxi-jtag-loop-alt.bin: sunxi-jtag-loop-alt.elf
	$(ARM_CPY) -O binary sunxi-jtag-loop-alt.elf sunxi-jtag-loop-alt.bin

sunxi-jtag-loop-alt.sunxi: sunxi-jtag-loop-alt.bin sunxi-mkboot
	$(srcdir)/sunxi-mkboot sunxi-jtag-loop-alt.bin sunxi-jtag-loop-alt.sunxi


sunxi-fel-sdboot.elf: sunxi-fel-sdboot.c sunxi-fel-sdboot.lds
	$(ARM_GCC) -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder sunxi-fel-sdboot.c -nostdlib -o sunxi-fel-sdboot.elf -T sunxi-fel-sdboot.lds -Wl,-N

sunxi-fel-sdboot.bin: sunxi-fel-sdboot.elf
	$(ARM_CPY) -O binary sunxi-fel-sdboot.elf sunxi-fel-sdboot.bin

sunxi-fel-sdboot.sunxi: sunxi-fel-sdboot.bin sunxi-mkboot
	$(srcdir)/sunxi-mkboot sunxi-fel-sdboot.bin sunxi-fel-sdboot.sunxi


sunxi-boot_head_sun4i.elf: sunxi-boot_head.S sunxi-boot_head.lds
	$(ARM_GCC) -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder sunxi-boot_head.S -nostdlib -o sunxi-boot_head_sun4i.elf -T sunxi-boot_head.lds -Wl,-N -DMACHID=0x1008

sunxi-boot_head_sun4i.bin: sunxi-boot_head_sun4i.elf
	$(ARM_CPY) -O binary sunxi-boot_head_sun4i.elf sunxi-boot_head_sun4i.bin

sunxi-boot_head_sun5i.elf: sunxi-boot_head.S sunxi-boot_head.lds
	$(ARM_GCC) -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder sunxi-boot_head.S -nostdlib -o sunxi-boot_head_sun5i.elf -T sunxi-boot_head.lds -Wl,-N -DMACHID=0x102A

sunxi-boot_head_sun5i.bin: sunxi-boot_head_sun5i.elf
	$(ARM_CPY) -O binary sunxi-boot_head_sun5i.elf sunxi-boot_head_sun5i.bin
endif

install-exec-hook:
	-rm -f $(DESTDIR)$(bindir)/sunxi-fex2bin
	-$(LN_S) $(DESTDIR)$(bindir)/sunxi-fexc $(DESTDIR)$(bindir)/sunxi-fex2bin
	-rm -f $(DESTDIR)$(bindir)/sunxi-bin2fex
	-$(LN_S) $(DESTDIR)$(bindir)/sunxi-fexc $(DESTDIR)$(bindir)/sunxi-bin2fex
	
uninstall-local:
	-rm -f $(DESTDIR)$(bindir)/sunxi-bin2fex
	-rm -f $(DESTDIR)$(bindir)/sunxi-fex2bin

clean-local:
	-rm -f *.bin
	-rm -f *.elf
	-rm -f *.sunxi
	-rm -f *.nm

help:
	@echo "sunxi-tools."
	@echo -e "\nBuild targets:"
	@echo "  all                        - Build everything (default)."
	@echo -e "\nPlatform independent targets:"
	@echo "  sunxi-nand                 - Build sunxi-nand."
	@echo "  sunxi-fexc                 - sunxi-fexc."
	@echo "  sunxi-phoenix_info         - Build sunxi-phoenix_info."
	@echo "  sunxi-bootinfo             - Build sunxi-bootinfo."
	@echo -e "\nArm only targets (Cross compilable):"
	@echo "  sunxi-meminfo              - Build sunxi-meminfo."
	@echo "  sunxi-pio                  - Build sunxi-pio."
	@echo "  sunxi-script-extractor     - Build sunxi-script-extractor."
if BUILD_SUNXIFEL
	@echo -e "\nArm only targets: (Native compilable only):"
	@echo "  sunxi-fel                  - Build sunxi-fel."
endif
if BUILD_BAREMETAL
	@echo -e "\nBare metal targets:\n"
	@echo "  Since theses are specialized tools not needed unless you are"
	@echo "  working on bare-metal devices (IE. no operating system),"
	@echo "  the default 'all' target does not build any of theses tools."
	@echo "  You need to specify the 'sunxi-bin' target or an individual"
	@echo -e "  tool.\n"
	@echo "  sunxi-bin                  - Build all bare-metal targets."
	@echo "  sunxi-fel-copy.bin         - sunxi-fel-copy.bin."
	@echo "  sunxi-fel-pio.bin          - sunxi-fel-pio.bin."
	@echo "  sunxi-jtag-loop.sunxi      - sunxi-jtag-loop.sunxi."
	@echo "  sunxi-jtag-loop-alt.sunxi  - sunxi-jtag-loop-alt.sunxi."
	@echo "  sunxi-fel-sdboot.sunxi     - sunxi-fel-sdboot.sunxi."
	@echo "  sunxi-boot_head_sun4i.bin  - sunxi-boot_head_sun4i.bin."
	@echo "  sunxi-boot_head_sun5i.bin  - sunxi-boot_head_sun5i.bin."
endif
	@echo -e "\nstandard targets:"
	@echo "  install                    - Install everything."
	@echo "  uninstal                   - Uninstall everything."
	@echo "  clean                      - Remove most generated files but keep configuration."
	@echo -e "  distclean                  - Remove all generaated files and configuration.\n"
