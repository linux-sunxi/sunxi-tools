#!/bin/bash
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

echo $0 $*
FEX2BIN=../fex2bin
BIN2FEX=../bin2fex
FEX=$1
BIN=${FEX/%.fex/.bin}
REVERSE=${FEX/%.fex/.new}
${FEX2BIN} ${FEX} ${BIN}
${BIN2FEX} ${BIN} ${REVERSE}
# preprocess .fex, compare it to the bin2fex output
if ./unify-fex ${FEX} | diff -uwB - ${REVERSE}; then
	# if successful, clean up the output files
	rm -f ${BIN} ${REVERSE}
else
	echo '***'
	echo "*** ERROR processing ${FEX}"
	echo '***'
	exit 1
fi
