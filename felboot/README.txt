fel-boot.bin is a FEL bootstrap program.

fel write 0x2000 fel-boot.bin
fel exe 0x2000
fel write 0x4a000000 u-boot.bin
fel exe 0x4a000000

optionally load kernel + initramfs before fel exe of u-boot

fel write 0x2000 fel-boot.bin
fel exe 0x2000
fel write 0x4a000000 u-boot.bin
fel write 0x43000000 ../script.bin
fel write 0x44000000 ../uImage
fel write 0x4c000000 ../initramfs.img
fel exe 0x4a000000

Build instructions:

0. You need to build u-boot sunxi-current SPL first for the your board.

1. Specify needed configuration when when running make

	BOARD=		boardname
	CROSS_COMPILE=	compiler prefix
	UBOOT=		u-boot sources
	UBOOTOBJ=	u-boot built tree

make BOARD=cubieboard CROSS_COMPILE=arm-linux-gnueabihf- UBOOT=~/SRC/u-boot/ UBOOTOBJ=~/SRC/u-boot/build/'$(BOARD)'/

Other options
	NO_PRINTF=1	Disable printf, reducing the code size a bit

Defaults:

# Target board name. This should match the dram_<boardname>.c in
# u-boot/board/sunxi/
BOARD=eoma68

# Path to your tool chain
CROSS_COMPILE=ikk/toolchains/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-

# U-boot main source path
UBOOT=/home/henrik/SRC/u-boot/

# U-boot object path (O=... when building u-boot).
UBOOTOBJ=$(UBOOT)build/$(BOARD)/
