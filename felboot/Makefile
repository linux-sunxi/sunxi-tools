
CC=gcc

# Target board name. This should match the dram_<boardname>.c in
# u-boot/board/sunxi/
BOARD=cubieboard

# Path/prefix of your tool chain
CROSS_COMPILE=arm-linux-gnueabihf-

# U-boot main source path
UBOOT=$(HOME)/SRC/u-boot/

# U-boot object path (O=... when building u-boot).
UBOOTOBJ=$(UBOOT)build/$(BOARD)/

CFLAGS=-g -O2 -marm -mno-thumb-interwork -mabi=aapcs-linux -march=armv7-a -fno-common -fno-builtin -ffreestanding -ffixed-r8 -msoft-float -D__KERNEL__ -I$(UBOOTOBJ)include2 -I$(UBOOTOBJ)include -I$(UBOOT)include 

all: fel-boot-$(BOARD).bin

UBOOT_OBJS= \
	$(addprefix spl/arch/arm/cpu/armv7/sunxi/,clock.o pinmux.o dram.o board.o timer.o) \
	spl/arch/arm/cpu/armv7/syslib.o \
	spl/arch/arm/lib/eabi_compat.o \
	spl/drivers/power/axp209.o \
	spl/drivers/i2c/libi2c.o \
	spl/common/memsize.o \
	spl/board/sunxi/dram_$(BOARD).o

.c.o:
	$(CROSS_COMPILE)$(CC) -c $(CFLAGS) $< -o $@

OBJS += main.o

ifeq ($(NO_PRINTF),1)
CFLAGS += -DNO_PRINTF
else
OBJS += printf-stdarg.o early_print.o
endif

fel-boot-$(BOARD).elf: $(OBJS) $(addprefix $(UBOOTOBJ),$(UBOOT_OBJS))
	$(CROSS_COMPILE)$(CC) -Tfel-boot.ld -static -nostartfiles $(CFLAGS) -Wl,-Map=$@.map $^ -o $@

fel-boot-$(BOARD).bin: fel-boot-$(BOARD).elf
	$(CROSS_COMPILE)objcopy -O binary $< $@

clean:
	rm -f *.o *.map *.elf
