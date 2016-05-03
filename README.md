# sunxi-tools
[![License](http://img.shields.io/badge/License-GPL-green.svg)][COPYING]
[![Build Status](https://travis-ci.org/linux-sunxi/sunxi-tools.svg?branch=master)](https://travis-ci.org/linux-sunxi/sunxi-tools)

Copyright (C) 2012  Alejandro Mery <amery@geeks.cl>

Tools to help hacking Allwinner A10 (aka sun4i) based devices and possibly
it's successors, that's why the 'x' in the package name.

<pre>
sunxi-fexc:
	`.fex` file (de)compiler

	Usage: ./sunxi-fexc [-vq] [-I &lt;infmt>] [-O &lt;outfmt>] [&lt;input> [&lt;output>]]

	infmt:  fex, bin  (default:fex)
	outfmt: fex, bin  (default:bin)

bin2fex:
	compatibility shortcut to call `sunxi-fexc` to decompile a script.bin
	blob back into `.fex` format used by allwinner's SDK to configure
	the boards.

fex2bin:
	compatiblity shortcut to call `sunxi-fexc` to compile a `.fex` file
	into the binary form used by the sun4i kernel.

sunxi-fel:
	script interface for talking to the FEL USB handler built in to
	th CPU. You activate FEL mode by pushing the usboot/recovery
	button at poweron. See http://linux-sunxi.org/FEL/USBBoot for
	a detailed usage guide.

fel-gpio:
	Simple wrapper around fel-pio and fel to allos GPIO manipulations
	via FEL
	
fel-sdboot:
	ARM native sdcard bootloader forcing the device into FEL mode

fel-pio:
	ARM native helper for fel-gpio

sunxi-pio:
	Manipulate PIO register dumps

sunxi-nand-part:
	Tool for manipulating Allwinner NAND partition tables

jtag-loop.sunxi:
	ARM native boot helper to force the SD port into JTAG
	and then stop, to ease debugging of bootloaders.

sunxi-bootinfo:
	Dump information from Allwinner boot files (boot0/boot1)
	--type=sd	include SD boot info
	--type=nand	include NAND boot info (not implemented)

phoenix_info:
	gives information about a phoenix image created by the
	phoenixcard utility and optionally extracts the embedded boot
	code & firmware file from their hidden partitions.

sunxi-meminfo:
	Tool for reading DRAM settings from registers. Compiled as a
	static binary for use on android and other OSes. To build this,
	get a toolchain, and run:
		make CROSS_COMPILE=arm-linux-gnueabihf- sunxi-meminfo

sunxi-script_extractor:
	A simple tool, which can be executed on a rooted Android device
	to dump the script.bin blob from RAM via reading /dev/mem.
	To build this, get a toolchain, and run:
		make CROSS_COMPILE=arm-linux-gnueabihf- sunxi-script_extractor
</pre>

This software is licensed under the terms of GPLv2+ as defined by the
Free Software Foundation, details can be read in the [COPYING][] file.

[copying]: COPYING