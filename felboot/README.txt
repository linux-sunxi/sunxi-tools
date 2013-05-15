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

1. You need to build u-boot sunxi-current SPL first for the same CPU generation, i.e. cubieboard.

Change the paths in Makefile to reflect where your copy of u-boot and toolchain is, or specify the paths when runnign make

make


To adopt for another board:

1a) If the board is supported by u-boot SPL then change UBOOT_OBJS to include the dram spefications for your board and remove dram.o from fel-boot.elf: line.

1b) Else update dram.c with the right parameters for your board.

2. Change early_printf.c to define the right UART number for your board.

3. make

