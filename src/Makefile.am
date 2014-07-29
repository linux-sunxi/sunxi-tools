
AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = adb-devprobe.sh \
             boot_head.S     \
             boot_head.lds   \
             fel-gpio        \
             fel-pio.c       \
             fel-pio.lds     \
             fel-sdboot.c    \
             fel-sdboot.lds  \
             jtag-loop.c     \
             jtag-loop.lds   \
             \
             jtag-loop.S     \
             fel-copy.c

AM_CFLAGS  = -g -Os -fpic -fno-common -fno-builtin -ffreestanding -nostdinc        \
             -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral \
             -Wno-format-security -fno-toplevel-reorder -nostdlib


BINARIES   = fel-pio.bin         \
             jtag-loop.bin       \
             fel-sdboot.bin      \
             boot_head_sun4i.bin \
             boot_head_sun5i.bin

scriptsdir      = $(datadir)/doc/@PACKAGE@-@VERSION@/scripts
scripts_SCRIPTS = adb-devprobe.sh fel-gpio


clean-local:
	rm -f boot_head_sun4i.elf  boot_head_sun5i.elf  fel-sdboot.elf  fel-pio.elf fel-pio.nm jtag-loop.elf boot_head_sun4i.bin  boot_head_sun5i.bin  fel-sdboot.bin  fel-pio.bin  jtag-loop.bin *~

all: $(BINARIES)


fel-pio.bin: fel-pio.elf fel-pio.nm
	$(OBJCOPY) -O binary fel-pio.elf fel-pio.bin

fel-pio.elf: fel-pio.c fel-pio.lds
	$(CC) $(CFLAGS) $(AM_CFLAGS) $(THUMB_CFLAGS) $(top_srcdir)/src/fel-pio.c \
	      -nostdlib -o fel-pio.elf -T $(top_srcdir)/src/fel-pio.lds

fel-pio.nm: fel-pio.elf
	$(NM) $(CURDIR)/fel-pio.elf | grep -v " _" >fel-pio.nm

jtag-loop.elf: jtag-loop.c jtag-loop.lds
	$(CC) $(CFLAGS) $(AM_CFLAGS) $(THUMB_CFLAGS) $(top_srcdir)/src/jtag-loop.c \
	      -o jtag-loop.elf -T $(top_srcdir)/src/jtag-loop.lds -Wl,-N

jtag-loop.bin: jtag-loop.elf
	$(OBJCOPY) -O binary jtag-loop.elf jtag-loop.bin

#jtag-loop.sunxi: jtag-loop.bin
#	mksunxiboot jtag-loop.bin jtag-loop.sunxi

fel-sdboot.elf: fel-sdboot.c fel-sdboot.lds
	$(CC) $(CFLAGS) $(AM_CFLAGS) $(THUMB_CFLAGS) $(top_srcdir)/src/fel-sdboot.c \
	      -o fel-sdboot.elf -T $(top_srcdir)/src/fel-sdboot.lds -Wl,-N

fel-sdboot.bin: fel-sdboot.elf
	$(OBJCOPY) -O binary fel-sdboot.elf fel-sdboot.bin

#fel-sdboot.sunxi: fel-sdboot.bin
#	mksunxiboot fel-sdboot.bin fel-sdboot.sunxi

boot_head_sun3i.elf: boot_head_sun3i.S boot_head_sun3i.lds
	$(CC) $(CFLAGS) $(AM_CFLAGS) $(THUMB_CFLAGS) $(top_srcdir)/src/boot_head.S \
	      -o boot_head_sun3i.elf -T $(top_srcdir)/src/boot_head.lds -Wl,-N -DMACHID=0x1094

boot_head_sun3i.bin: boot_head_sun3i.elf
	$(OBJCOPY) -O binary boot_head_sun3i.elf boot_head_sun3i.bin

boot_head_sun4i.elf: boot_head.S boot_head.lds
	$(CC) $(CFLAGS) $(AM_CFLAGS) $(THUMB_CFLAGS) $(top_srcdir)/src/boot_head.S \
	      -o boot_head_sun4i.elf -T $(top_srcdir)/src/boot_head.lds -Wl,-N -DMACHID=0x1008

boot_head_sun4i.bin: boot_head_sun4i.elf
	$(OBJCOPY) -O binary boot_head_sun4i.elf boot_head_sun4i.bin

boot_head_sun5i.elf: boot_head.S boot_head.lds
	$(CC) $(CFLAGS) $(AM_CFLAGS) $(THUMB_CFLAGS) $(top_srcdir)/src/boot_head.S \
	      -o boot_head_sun5i.elf -T $(top_srcdir)/src/boot_head.lds -Wl,-N -DMACHID=0x102A

boot_head_sun5i.bin: boot_head_sun5i.elf
	$(OBJCOPY) -O binary boot_head_sun5i.elf boot_head_sun5i.bin
