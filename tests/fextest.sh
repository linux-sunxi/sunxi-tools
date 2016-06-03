#!/bin/bash
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
