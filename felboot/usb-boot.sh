#!/bin/sh -ex
fel write 0x2000 fel-boot-${1}.bin
fel exe 0x2000
fel write 0x4a000000 ../u-boot.bin
fel write 0x43000000 ../script.bin
fel write 0x44000000 ../uImage
fel write 0x4c000000 ../initramfs.img
fel exe 0x4a000000
