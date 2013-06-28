CC = gcc
CFLAGS = -g -O0 -Wall -Wextra
CFLAGS += -std=c99 -D_POSIX_C_SOURCE=200112L
CFLAGS += -Iinclude/

TOOLS = fexc bin2fex fex2bin bootinfo fel pio
TOOLS += nand-part

MISC_TOOLS = phoenix_info

CROSS_COMPILE ?= arm-none-eabi-

.PHONY: all clean

all: $(TOOLS)

misc: $(MISC_TOOLS)

clean:
	@rm -vf $(TOOLS) $(MISC_TOOLS) *.o *.elf *.sunxi *.bin *.nm *.orig


$(TOOLS): Makefile common.h

fex2bin bin2fex: fexc
	ln -s $< $@

fexc: fexc.h script.h script.c \
	script_uboot.h script_uboot.c \
	script_bin.h script_bin.c \
	script_fex.h script_fex.c

LIBUSB = libusb-1.0
LIBUSB_CFLAGS = `pkg-config --cflags $(LIBUSB)`
LIBUSB_LIBS = `pkg-config --libs $(LIBUSB)`

fel: fel.c
	$(CC) $(CFLAGS) $(LIBUSB_CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS) $(LIBUSB_LIBS)

%: %.c
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

bootinfo: bootinfo.c

.gitignore: Makefile
	@for x in $(TOOLS) '*.o' '*.swp'; do \
		echo "$$x"; \
	done > $@
