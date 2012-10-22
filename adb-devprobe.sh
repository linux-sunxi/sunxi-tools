#!/bin/bash

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
