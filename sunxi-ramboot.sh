# U-boot RAM boot script
ramdisk=
if iminfo 0x4c000000; then
	ramdisk=0x4c000000
fi
setenv bootargs console=ttyS0,115200 rdinit=/sbin/init panic=10
bootm 0x44000000 $ramdisk
