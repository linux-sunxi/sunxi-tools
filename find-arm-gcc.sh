#
# Try to locate suitable ARM cross compilers available via $PATH
# If any are found, this function will output them as a TAB-delimited list
#
scan_path () {
IFS=":"
for path in $PATH; do
	find "$path" -maxdepth 1 -executable -name 'arm*-gcc' -printf '%f\t' 2>/dev/null
done
}

# Use only the first field from result, and convert it to a toolchain prefix
scan_path | cut -f 1 | sed -e 's/-gcc/-/'
