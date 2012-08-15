/*
arm-none-linux-gnueabi-gcc  -g -fno-common -ffixed-r8 -msoft-float -fno-builtin -ffreestanding -nostdinc -mno-thumb-interwork -Wall -Wstrict-prototypes -fno-stack-protector -Wno-format-nonliteral -Wno-format-security -fno-toplevel-reorder  fel-loop.S -c

arm-none-linux-gnueabi-objcopy -O binary fel-loop.o fel-loop.bin

arm-none-linux-gnueabi-nm fel-loop.o
*/

.file "fel-loop.S"
.global entry
.text 
.code 32
.section ".start", "ax"

entry:
    ldr r0,=0x01c208b4
	ldr r1,=0x00444444
	str r1, [r0]
    b .
