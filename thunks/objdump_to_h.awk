# labels
/[[:xdigit:]]+ <\w+>:/ {
	# (Note: using $0 instead of $2 would also include the address)
	if (style=="old")
		printf "\t/* %s */\n", $2
	else
		printf "\t\t/* %s */\n", $2
}

# disassembly lines
/[[:xdigit:]]+:/ {
	if (style=="old")
		printf "\t0x%s, /* %9s    %-10s", $2, $1, $3
	else
		printf "\t\thtole32(0x%s), /* %5s  %-5s", $2, $1, $3

	for (i = 4; i <= NF; i++)
		if ($i == ";") {
			# strip comment (anything after and including ';')
			NF = i - 1
			break
		}
	# clear $1 to $3, which re-calculates $0 (= remainder of line)
	$3 = ""
	$2 = ""
	$1 = ""
	gsub("^\\s+", "") # strip leading whitespace

	if (style=="old")
		printf " %-28s */\n", $0
	else
		printf " %-23s */\n", $0
}
