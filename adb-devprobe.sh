#!/bin/bash

# Copyright (C) 2012  Henrik Nordstrom <henrik@henriknordstrom.net>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

adb shell insmod /vendor/modules/sunxi-dbgreg.ko >/dev/null

dump_io()
{
  module=$1
  addr=$2
  len=$3

  for ((i = 0; i < len; i+=4)) {
    printf "%x %s " $((addr + i)) $module
    adb shell "echo `printf %x $((addr + i))` > /sys/devices/virtual/misc/sunxi-reg/rw/address; cat /sys/devices/virtual/misc/sunxi-reg/rw/value"
    echo
  }
}

dump_io SRAM 0xf1c00000 0x100
dump_io DRAM 0xf1c01000 0x400
dump_io CCM  0xf1c20000 0x400
dump_io PIO  0xf1c20800 0x400

dump_pmu()
{
  for ((i = 0; i <0x100; i+=2)) {
    adb shell "echo `printf 0x%x $i` > /sys/bus/i2c/devices/0-0034/axp20_reg; cat /sys/bus/i2c/devices/0-0034/axp20_regs"
  }
}

dump_pmu
