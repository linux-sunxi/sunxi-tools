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

sun3i_MACHID=0x1094
sun4i_MACHID=0x1008
sun5i_MACHID=0x102A

BOOT_HEADS = boot_head_sun3i \
	boot_head_sun4i \
	boot_head_sun5i

ELFS = $(addsuffix .elf, $(BOOT_HEADS))
TARGETS = $(addsuffix .bin, $(BOOT_HEADS))

SOURCES = boot_head.S
LDSCRIPT = boot_head.lds
DEFINES=-DMACHID=$($(subst boot_head_,,$(subst .elf,,$@))_MACHID)
include ../baremetal.rules
