#!/bin/bash
#
# === Test errors / corner cases of "bin2fex", improving on code coverage ===
#
BIN2FEX=../bin2fex
TESTFILE=sunxi-boards/sys_config/a10/a10-olinuxino-lime
# use sunxi-fexc in "fex2bin" mode, testing explicit parameters at the same time
FEX2BIN="../sunxi-fexc -v -q -I fex -O bin"

${FEX2BIN} ${TESTFILE}.fex ${TESTFILE}.bin
# have bin2fex explicitly read /dev/stdin, to force use of fexc.c's "read_all()"
cat ${TESTFILE}.bin | ${BIN2FEX} /dev/stdin > /dev/null
rm -f ${TESTFILE}.bin
