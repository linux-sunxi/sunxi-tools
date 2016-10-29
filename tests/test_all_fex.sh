#!/bin/sh
FEXFILES=fexfiles.lst
if test -z $2; then
	FIND=find
else
	FIND=$2
fi
${FIND} $1 -name '*.fex' > ${FEXFILES}
while read fex; do
	./fextest.sh ${fex} || exit
done <${FEXFILES}
rm -f ${FEXFILES}
