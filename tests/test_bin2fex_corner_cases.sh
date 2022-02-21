#!/bin/bash
#
# === Test errors / corner cases of "bin2fex", improving on code coverage ===
#
# Copyright (C) 2016  Bernhard Nortmann <bernhard.nortmann@web.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

BIN2FEX=../bin2fex
TESTFILE=sunxi-boards/sys_config/a10/a10-olinuxino-lime
# use sunxi-fexc in "fex2bin" mode, testing explicit parameters at the same time
FEX2BIN="../sunxi-fexc -v -q -I fex -O bin"

${FEX2BIN} ${TESTFILE}.fex ${TESTFILE}.bin
# have bin2fex explicitly read /dev/stdin, to force use of fexc.c's "read_all()"
cat ${TESTFILE}.bin | ${BIN2FEX} /dev/stdin > /dev/null
rm -f ${TESTFILE}.bin
