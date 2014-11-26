# Copyright (C) 2014 Alexey Edelev <semlanik@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

include ../baremetal.env

ELFS = fel-sdboot.elf
TARGETS = fel-sdboot.bin

SOURCES = fel-sdboot.c
LDSCRIPT = fel-sdboot.lds
EXTRA_DEPS = fel-sdboot.sunxi
include ../baremetal.rules

fel-sdboot.sunxi: fel-sdboot.bin
	-if $$(which mksunxiboot); then mksunxiboot $^ $@; fi
