#!/bin/sh -ex
fel write 0x2000 fel-boot-${1}.bin
fel exe 0x2000
fel write 0x4a000000 ../u-boot.bin
fel exe 0x4a000000
