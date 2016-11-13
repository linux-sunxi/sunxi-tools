#!/bin/sh
FEXFILES=fexfiles.lst
find $1 -name '*.fex' > ${FEXFILES}
while read fex; do
	./fextest.sh ${fex} || exit
done <${FEXFILES}
rm -f ${FEXFILES}
