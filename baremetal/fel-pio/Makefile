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

ELFS = fel-pio.elf
TARGETS = fel-pio.bin
EXTRA_DEPS = fel-pio.nm

SOURCES = fel-pio.c
LDSCRIPT = fel-pio.lds

include ../baremetal.rules
fel-pio.nm: fel-pio.elf
	$(NM) $^ | grep -v " _" >$@
