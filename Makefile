# Copyright (C) 2012       Alejandro Mery <amery@geeks.cl>
# Copyright (C) 2012,2013  Henrik Nordstrom <henrik@henriknordstrom.net>
# Copyright (C) 2013       Patrick Wood <patrickhwood@gmail.com>
# Copyright (C) 2013       Pat Wood <Pat.Wood@efi.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC = gcc
CFLAGS = -g -O0 -Wall -Wextra
CFLAGS += -std=c99 -D_POSIX_C_SOURCE=200112L
CFLAGS += -Iinclude/

# Tools useful on host and target
TOOLS = sunxi-fexc sunxi-bootinfo sunxi-fel sunxi-nand-part

# Symlinks to sunxi-fexc
FEXC_LINKS = bin2fex fex2bin

# Tools which are only useful on the target
TARGET_TOOLS = sunxi-pio

MISC_TOOLS = phoenix_info

CROSS_COMPILE ?= arm-none-eabi-

DESTDIR ?=
PREFIX  ?= /usr/local
BINDIR  ?= $(PREFIX)/bin

.PHONY: all clean tools target-tools install install-tools install-target-tools

all: tools target-tools

tools: $(TOOLS) $(FEXC_LINKS)
target-tools: $(TARGET_TOOLS)

misc: $(MISC_TOOLS)

install: install-tools install-target-tools

install-tools: $(TOOLS)
	install -d $(DESTDIR)$(BINDIR)
	@set -ex ; for t in $^ ; do \
		install -m0755 $$t $(DESTDIR)$(BINDIR)/$$t ; \
	done
	@set -ex ; for l in $(FEXC_LINKS) ; do \
		ln -nfs sunxi-fexc $(DESTDIR)$(BINDIR)/$$l ; \
	done

install-target-tools: $(TARGET_TOOLS)
	install -d $(DESTDIR)$(BINDIR)
	@set -ex ; for t in $^ ; do \
		install -m0755 $$t $(DESTDIR)$(BINDIR)/$$t ; \
	done


clean:
	@rm -vf $(TOOLS) $(FEXC_LINKS) $(TARGET_TOOLS) $(MISC_TOOLS)
	@rm -vf *.o *.elf *.sunxi *.bin *.nm *.orig

$(TOOLS) $(TARGET_TOOLS): Makefile common.h

fex2bin bin2fex: sunxi-fexc
	ln -nsf $< $@

sunxi-fexc: fexc.h script.h script.c \
	script_uboot.h script_uboot.c \
	script_bin.h script_bin.c \
	script_fex.h script_fex.c

LIBUSB = libusb-1.0
LIBUSB_CFLAGS = `pkg-config --cflags $(LIBUSB)`
LIBUSB_LIBS = `pkg-config --libs $(LIBUSB)`

sunxi-fel: fel.c fel-to-spl-thunk.h progress.c progress.h
	$(CC) $(CFLAGS) $(LIBUSB_CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS) $(LIBUSB_LIBS)

sunxi-nand-part: nand-part-main.c nand-part.c nand-part-a10.h nand-part-a20.h
	$(CC) $(CFLAGS) -c -o nand-part-main.o nand-part-main.c
	$(CC) $(CFLAGS) -c -o nand-part-a10.o nand-part.c -D A10
	$(CC) $(CFLAGS) -c -o nand-part-a20.o nand-part.c -D A20
	$(CC) $(LDFLAGS) -o $@ nand-part-main.o nand-part-a10.o nand-part-a20.o $(LIBS)

sunxi-%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS)

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

sunxi-bootinfo: bootinfo.c

sunxi-meminfo: meminfo.c
	$(CROSS_COMPILE)gcc -g -O0 -Wall -static -o $@ $^

sunxi-script_extractor: script_extractor.c
	$(CROSS_COMPILE)gcc -g -O0 -Wall -static -o $@ $^

.gitignore: Makefile
	@for x in $(TOOLS) $(FEXC_LINKS) $(TARGET_TOOLS) '*.o' '*.swp'; do \
		echo "$$x"; \
	done > $@
