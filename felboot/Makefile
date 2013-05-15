
CC=gcc
BOARD=eoma68
CROSS_COMPILE=/home/henrik/toolchains/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
UBOOT=/home/henrik/SRC/u-boot/
UBOOTOBJ=$(UBOOT)build/$(BOARD)/
CFLAGS=-g -O2 -marm -mno-thumb-interwork -mabi=aapcs-linux -march=armv7-a -fno-common -ffixed-r8 -msoft-float -I$(UBOOTOBJ)include2 -I$(UBOOTOBJ)include -I$(UBOOT)include 

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

fel-boot-$(BOARD).elf: main.o util_printf.o early_print.o $(addprefix $(UBOOTOBJ),$(UBOOT_OBJS))
	$(CROSS_COMPILE)$(CC) -Tfel-boot.ld -static -nostartfiles $(CFLAGS) -Wl,-Map=$@.map $^ -o $@

fel-boot-$(BOARD).bin: fel-boot-$(BOARD).elf
	$(CROSS_COMPILE)objcopy -O binary $< $@

clean:
	rm -f *.o *.map *.elf
