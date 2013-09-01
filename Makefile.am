AUTOMAKE_OPTIONS = foreign
CFLAGS = -g -O0 -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200112L -Iinclude/

TOOLS = fexc bin2fex fex2bin bootinfo fel pio nand-part nand-part-a20 phoenix_info
BINARIES = fel-pio.bin jtag-loop.bin fel-sdboot.bin boot_head_sun4i.bin boot_head_sun5i.bin adb-devprobe.sh fel-gpio usb-boot

bin_PROGRAMS = $(TOOLS)
exampledir = $(datarootdir)/doc/@PACKAGE@
example_DATA = $(BINARIES) bin/fel-sdboot.sunxi bin/jtag-loop.sunxi bin/ramboot.scr bin/ramboot.uboot-sh README

fexc_SOURCES = bootinfo.c fel-copy.c fel-sdboot.c jtag-loop.c phoenix_info.c script_bin.c script_fex.c fel.c fel-pio.c fexc.c nand-part.c pio.c script.c script_uboot.c common.h fexc.h nand-part-a20.h nand-part.h script_bin.h script_fex.h script.h script_uboot.h include/endian_compat.h include/types.h
EXTRA_DIST = boot_head.S jtag-loop.S boot_head.lds fel-pio.lds fel-sdboot.lds jtag-loop.lds sunxi-tools.7 adb-devprobe.sh fel-gpio usb-boot bin/fel-pio.bin bin/fel-pio.nm bin/fel-sdboot.sunxi bin/jtag-loop.sunxi bin/ramboot.scr bin/ramboot.uboot-sh makedeb

nodist_bin2fex_SOURCES = bin2fex.c
nodist_fex2bin_SOURCES = fex2bin.c
nodist_nand_part_a20_SOURCES = nand-part-a20.c

man_MANS = sunxi-tools.7

CROSS_COMPILE ?= arm-none-eabi-

clean-local:
	rm -f boot_head_sun4i.elf  boot_head_sun5i.elf  fel-sdboot.elf  fel-pio.elf fel-pio.nm jtag-loop.elf boot_head_sun4i.bin  boot_head_sun5i.bin  fel-sdboot.bin  fel-pio.bin  jtag-loop.bin *~ *.deb

fexc: fexc.h script.h script.c script_uboot.h script_uboot.c script_bin.h script_bin.c script_fex.h script_fex.c

fex2bin: fexc
	ln -sf fexc fex2bin

bin2fex: fexc
	ln -sf fexc bin2fex

fel: fel.c
	$(CC) $(CFLAGS) $(LIBUSB_CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS) $(LIBUSB_LIBS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS)

nand-part-a20: nand-part.c nand-part-a20.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ nand-part.c -D A20 $(LIBS)

fel-pio.bin: fel-pio.elf fel-pio.nm
	$(CROSS_COMPILE)objcopy -O binary fel-pio.elf fel-pio.bin

fel-pio.elf: fel-pio.c fel-pio.lds
	$(CROSS_COMPILE)gcc  -g  -Os -fpic  -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  fel-pio.c -nostdlib -o fel-pio.elf -T fel-pio.lds

fel-pio.nm: fel-pio.elf
	$(CROSS_COMPILE)nm fel-pio.elf | grep -v " _" >fel-pio.nm

jtag-loop.elf: jtag-loop.c jtag-loop.lds
	$(CROSS_COMPILE)gcc  -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  jtag-loop.c -nostdlib -o jtag-loop.elf -T jtag-loop.lds -Wl,-N

jtag-loop.bin: jtag-loop.elf
	$(CROSS_COMPILE)objcopy -O binary jtag-loop.elf jtag-loop.bin

jtag-loop.sunxi: jtag-loop.bin
	mksunxiboot jtag-loop.bin jtag-loop.sunxi

fel-sdboot.elf: fel-sdboot.c fel-sdboot.lds
	$(CROSS_COMPILE)gcc  -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  fel-sdboot.c -nostdlib -o fel-sdboot.elf -T fel-sdboot.lds -Wl,-N

fel-sdboot.bin: fel-sdboot.elf
	$(CROSS_COMPILE)objcopy -O binary fel-sdboot.elf fel-sdboot.bin

fel-sdboot.sunxi: fel-sdboot.bin
	mksunxiboot fel-sdboot.bin fel-sdboot.sunxi

boot_head_sun3i.elf: boot_head_sun3i.S boot_head_sun3i.lds
	$(CROSS_COMPILE)gcc  -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  boot_head.S -nostdlib -o boot_head_sun3i.elf -T boot_head.lds -Wl,-N -DMACHID=0x1094

boot_head_sun3i.bin: boot_head_sun3i.elf
	$(CROSS_COMPILE)objcopy -O binary boot_head_sun3i.elf boot_head_sun3i.bin

boot_head_sun4i.elf: boot_head.S boot_head.lds
	$(CROSS_COMPILE)gcc  -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  boot_head.S -nostdlib -o boot_head_sun4i.elf -T boot_head.lds -Wl,-N -DMACHID=0x1008

boot_head_sun4i.bin: boot_head_sun4i.elf
	$(CROSS_COMPILE)objcopy -O binary boot_head_sun4i.elf boot_head_sun4i.bin

boot_head_sun5i.elf: boot_head.S boot_head.lds
	$(CROSS_COMPILE)gcc  -g  -Os  -fpic -fno-common -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  boot_head.S -nostdlib -o boot_head_sun5i.elf -T boot_head.lds -Wl,-N -DMACHID=0x102A

boot_head_sun5i.bin: boot_head_sun5i.elf
	$(CROSS_COMPILE)objcopy -O binary boot_head_sun5i.elf boot_head_sun5i.bin

bootinfo: bootinfo.c

dpkg-deb:
	./makedeb

.gitignore: Makefile
	@for x in $(TOOLS) $(BINARIES) '*.o' '*.swp' '*~' '.deps/*' Makefile.in /autom4te.cache /aclocal.m4 /compile /configure /depcomp /install-sh /missing; do \
		echo "$$x"; \
	done > $@
