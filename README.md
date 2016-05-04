# sunxi-tools
[![License](http://img.shields.io/badge/License-GPL-green.svg)][COPYING]
[![Build Status](https://travis-ci.org/linux-sunxi/sunxi-tools.svg?branch=master)](https://travis-ci.org/linux-sunxi/sunxi-tools)

Copyright (C) 2012  Alejandro Mery <amery@geeks.cl>

Tools to help hacking Allwinner A10 (aka sun4i) based devices and possibly
it's successors, that's why the 'x' in the package name.

### sunxi-fexc
`.fex` file (de)compiler

	Usage: ./sunxi-fexc [-vq] [-I <infmt>] [-O <outfmt>] [<input> [<output>]]

	infmt:  fex, bin  (default:fex)
	outfmt: fex, bin  (default:bin)

### bin2fex
compatibility shortcut to call `sunxi-fexc` to decompile a _script.bin_
blob back into `.fex` format used by Allwinner's SDK to configure
the boards.

### fex2bin
compatiblity shortcut to call `sunxi-fexc` to compile a `.fex` file
into the binary form used by the legacy 3.4 kernel ("linux&#8209;sunxi").

### sunxi-fel
script interface for talking to the FEL USB handler built in to
the CPU. You activate [FEL mode] by pushing the _uboot_ / _recovery_
button at poweron. See http://linux-sunxi.org/FEL/USBBoot for
a detailed usage guide.

### fel-gpio
Simple wrapper (script) around `fel-pio` and `sunxi-fel`
to allow GPIO manipulations via FEL

### fel-sdboot
ARM native sdcard bootloader forcing the device into FEL mode

### fel-pio
ARM native helper (binary) for `fel-gpio`

### sunxi-pio
Manipulate PIO register dumps

### sunxi-nand-part
Tool for manipulating Allwinner NAND partition tables

### jtag-loop.sunxi
ARM native boot helper to force the SD port into JTAG and then stop,
to ease debugging of bootloaders.

### sunxi-bootinfo
Dump information from Allwinner boot files (_boot0_ / _boot1_)

	--type=sd	include SD boot info
	--type=nand	include NAND boot info (not implemented)

### phoenix_info
gives information about a phoenix image created by the
phoenixcard utility and optionally extracts the embedded boot
code & firmware file from their hidden partitions.

### sunxi-meminfo
Tool for reading DRAM settings from registers. Compiled as a
static binary for use on android and other OSes.
To build this, get a toolchain and run:

	make CROSS_COMPILE=arm-linux-gnueabihf- sunxi-meminfo

### sunxi-script_extractor
A simple tool, which can be executed on a rooted Android device
to dump the _script.bin_ blob from RAM via reading _/dev/mem_.
To build this, get a toolchain and run:

	make CROSS_COMPILE=arm-linux-gnueabihf- sunxi-script_extractor
---

## License
This software is licensed under the terms of GPLv2+ as defined by the
Free Software Foundation, details can be read in the [COPYING][] file.

[copying]: COPYING
[fel mode]: http://linux-sunxi.org/FEL
