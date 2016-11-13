#!/bin/bash
#
# === Test errors / corner cases of "fex2bin", improving on code coverage ===
#
FEX2BIN=../fex2bin

function expect () {
	OUT=`${FEX2BIN} 2>&1`
	if (! echo ${OUT} | grep -q "$1"); then
		echo ERROR: Expected substring \"$1\" not found in output:
		echo ${OUT}
		exit 1
	fi
	#echo ${OUT}
}

# missing section, CRLF line ending
echo -e "foobar\r\n" | expect "data must follow a section"

# malformed sections
expect "incomplete section declaration" <<-EOF
	[foobar
EOF
expect "invalid character at 5" <<-EOF
	[foo#bar]
EOF

# invalid entry
expect "invalid character at 4" <<-EOF
	[foo]
	bar
EOF

# bad port specifiers
expect "parse error at 12" <<-EOF
	[foo]
	bar = port:P@0
EOF
expect "invalid character at 14" <<-EOF
	[foo]
	bar = port:PA*
EOF
expect "port out of range at 14" <<-EOF
	[foo]
	bar = port:PA666
EOF
expect "value out of range at 17" <<-EOF
	[foo]
	bar = port:PA00<-1>
EOF
expect "invalid character at 18" <<-EOF
	[foo]
	bar = port:PA00<0 >
EOF

# bad <key> = <value> pairs
expect "invalid character at 8" <<-EOF
	[foo]
	bar = 0*
EOF
expect "value out of range" <<-EOF
	[foo]
	bar = 4294967296
EOF
expect "unquoted value 'bad', assuming string" <<-EOF
	[foo]
	bar = bad
EOF

# test truncation of very long identifiers
${FEX2BIN} > /dev/null <<-EOF
	[an_overly_long_section_name_to_truncate]
	an_overly_long_entry_name_to_truncate = 0
EOF
