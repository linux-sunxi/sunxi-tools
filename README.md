# sunxi-tools
[![License](http://img.shields.io/badge/License-GPL-green.svg)](LICENSE.md)
[![Build Status](https://travis-ci.org/linux-sunxi/sunxi-tools.svg?branch=master)](https://travis-ci.org/linux-sunxi/sunxi-tools)
[![Releases](https://img.shields.io/github/release/linux-sunxi/sunxi-tools.svg)](https://github.com/linux-sunxi/sunxi-tools/releases)

Copyright (C) 2012  Alejandro Mery <amery@geeks.cl>
<br>For a full list of contributors, see
[this link](https://github.com/linux-sunxi/sunxi-tools/contributors)
or use the command `git shortlog -se --no-merges`.

Tools to help hacking Allwinner A10 (aka sun4i) based devices and
its successors ([sunxi][]), that's why the 'x' in the package name.

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

### uart0-helloworld-sdboot
ARM native sdcard bootloader, which is only printing a short "hello"
message to the UART0 serial console. Because it relies on runtime
SoC type detection, this single image is bootable on a wide range of
Allwinner devices and can be used for testing. Additionally, it may
serve as a template/example for developing simple bare metal code
(LED blinking and other similar GPIO related things).

### fel-pio
ARM native helper (binary) for `fel-gpio`

### sunxi-pio
Manipulate PIO register dumps

### sunxi-nand-part
Tool for manipulating Allwinner NAND partition tables

### sunxi-nand-image-builder
Tool used to create raw NAND images (including boot0 images)

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

## Building

Compilation requires the development version of *libusb-1.0* (include header
and library) to be installed for `sunxi-fel`. Unless you explicitly pass
*LIBUSB_CFLAGS* and *LIBUSB_LIBS* to the make utility, `pkg-config` is also
needed.

Available build targets:

* `make tools`
builds tools that are useful on the host. This is what most people will want,
and our default target (when simply using `make`).

* `make target-tools`
builds tools that are intended for the target (Allwinner SoC), using a
cross-compiler. The toolchain prefix *CROSS_COMPILE* defaults to `arm-none-eabi-`,
adjust it if needed.
<br>_Hint:_ When compiling 'natively' on the target platform you may
simply use an empty toolchain prefix here (`make target-tools CROSS_COMPILE=`
or `make all CROSS_COMPILE=`).

* `make all`
builds both *tools* and *target-tools*.

* `make install-tools`
builds *tools* and then copies/installs them to a filesystem location. The
destination is affected by settings for `DESTDIR`, `PREFIX` and possibly
`BINDIR`. For details, please refer to the *Makefile*.
You may use `make install` as a shortcut for this.

* `make install-target-tools`
builds *target-tools* and then copies/installs them to a filesystem location
selected by `DESTDIR`, `PREFIX` and possibly `BINDIR` - see `make install-tools`
above.

* `make install-all`
builds and installs both *tools* and *target-tools*.

* `make misc`
builds miscellaneous (host) utilities that are not part of our 'standard' suite.
Currently this means `phoenix_info` and `sunxi-nand-image-builder`.

* `make install-misc`
builds *misc* and installs the resulting binaries.

## License
This software is licensed under the terms of GPLv2+ as defined by the
Free Software Foundation, details can be read in the [LICENSE.md](LICENSE.md)
file.

[sunxi]: http://linux-sunxi.org
[fel mode]: http://linux-sunxi.org/FEL
