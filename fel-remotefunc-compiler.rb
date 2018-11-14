#!/usr/bin/env ruby
#
# (C) Copyright 2016 Siarhei Siamashka <siarhei.siamashka@gmail.com>
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
#

if ARGV.size < 2
  printf("Usage: #{$PROGRAM_NAME} [c_source_input] [marshalled_header_output]\n\n")

  printf("This script uses an ARM toolchain to compile native ARM code, and then\n")
  printf("automatically generates the necessary wrapper code for calling it from\n")
  printf("the sunxi-fel tool. Executing such compiled pieces of code natively on\n")
  printf("the device may be needed for the performance critical parts.\n")

  printf("\nExample input file:\n\n")
  printf("    unsigned sum(unsigned a, unsigned b)\n")
  printf("    {\n")
  printf("        return a + b;\n")
  printf("    }\n")
  printf("\n")
  printf("Using this example code inside of sunxi-fel:\n")
  printf("\n")
  printf("    uint32_t a = 1, b = 2, c;\n")
  printf("    aw_fel_remotefunc_prepare_sum(dev, a, b);\n")
  printf("    aw_fel_remotefunc_execute(dev, &c);\n")
  printf("    printf(\"%%d + %%d = %%d\\n\", a, b, c);\n\n")

  printf("If the returned result is not needed (a void function), then the second\n")
  printf("argument to the 'aw_fel_remotefunc_execute' function can be NULL.\n\n")
  exit(1)
end

def tool_exists(tool_name)
    `which #{tool_name} > /dev/null 2>&1`
    return $?.to_i == 0
end

def parse_stack_usage(filename)
  return unless File.exists?(filename)
  File.read(filename).strip.split("\n").map do |l|
    if l =~ /\:([^\:\s]+)\s+(\d+)\s+(\S+)/
      if $3 != "static"
        abort sprintf("Non-static stack usage for function '%s'\n", $1)
      end
      {function_name: $1, stack_usage: $2.to_i}
    else
      abort sprintf("Failed to parse stack usage information '%s'\n", l.strip)
    end
  end
end

toolchains = [
  "arm-none-eabi-",
  "arm-linux-gnueabihf-",
  "arm-none-linux-gnueabi-",
  "armv7a-hardfloat-linux-gnueabi-",
]

toolchain = toolchains.find { |toolchain| tool_exists("#{toolchain}gcc") }
abort "Can't find any usable ARM crosscompiler.\n" unless toolchain

# Compile the source file
system("#{toolchain}gcc -c -O3 -marm -march=armv7-a -mfloat-abi=soft -fstack-usage -fpic -o #{ARGV[0]}.o #{ARGV[0]}")
exit($?.to_i) if $?.to_i != 0

# Read the stack usage information
stack_usage = parse_stack_usage("#{ARGV[0]}.su")
if stack_usage.size != 1
  abort sprintf("Expected only one function in the source file, but got %s.\n",
                stack_usage.map {|a| "'" + a[:function_name] + "()'" }.join(", "))
end

`#{toolchain}size -A #{ARGV[0]}.o`.each_line do |l|
  if l =~ /(\S+)\s+(\S+)/
    if ($1 == ".data" || $1 == ".bss" || $1 == ".rodata") && $2.to_i > 0
      abort "Can't have non-empty '.data', '.bss' or '.rodata' section."
    end
  end
end

`#{toolchain}objdump -t #{ARGV[0]}.o`.each_line do |l|
  if l =~ /\*UND\*/
    abort "External references are not allowed: '#{l.strip}'.\n"
  end
end

function_name = stack_usage[0][:function_name]

# Read the source file and strip multiline C comments
sourcefile = File.read(ARGV[0]).gsub(/\/\*.*?\*\//m, "")

# Try to find the function and its arguments
unless sourcefile =~ /#{function_name}\((.*?)\)/m
  abort sprintf("Can't find the function '%s()' in the source file.\n",
                function_name)
end

# Extract the function argument names
function_args = $1.split(",").map {|a| if a.strip =~ /([^\*\s]+)$/ then $1 end }

# Check if there is any return value
have_retval = !(sourcefile =~ /void\s+#{function_name}/m)

###############################################################################
# Generate output file
###############################################################################

out = File.open(ARGV[1], "w")

out.printf("/* Automatically generated, do not edit! */\n\n")

out.printf("static void\n")
funcdecl = sprintf("aw_fel_remotefunc_prepare_#{function_name}(feldev_handle *dev,")
out.printf("%s\n", funcdecl)
out.printf("%s", function_args.map {|a|
           " " * funcdecl.index("(") + " uint32_t              " + a }.join(",\n"))
out.printf(")\n{\n")

out.printf("\tstatic uint8_t arm_code[] = {\n")
`#{toolchain}objdump -d #{ARGV[0]}.o`.each_line {|l|
    next unless l =~ /(\h+)\:\s+(\h+)\s+(\S+)\s+([^;]*)/
    addr   = $1
    opcode = $2
    p1     = $3
    p2     = $4.strip
    opcode = opcode.scan(/../).map {|a| "0x" + a }.reverse.join(", ")
    out.printf("\t\t%s, /* %4s:    %-8s %-34s \x2a/\n", opcode, addr, p1, p2)
}
out.printf("\t};\n")

out.printf("\tuint32_t args[] = {\n\t\t")
out.printf("%s\n\t};\n", function_args.join(",\n\t\t"))

out.printf("\taw_fel_remotefunc_prepare(dev, %d, arm_code, sizeof(arm_code), %d, args);\n",
           stack_usage[0][:stack_usage], function_args.size)

out.printf("}\n")
