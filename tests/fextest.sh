#!/bin/bash
echo $0 $*
FEX2BIN=../fex2bin
BIN2FEX=../bin2fex
FEX=$1
BIN=${FEX/%.fex/.bin}
REVERSE=${FEX/%.fex/.new}
${FEX2BIN} ${FEX} ${BIN}
