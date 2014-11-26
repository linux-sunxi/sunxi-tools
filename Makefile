# Copyright (C) 2012 Alejandro Mery <amery@geeks.cl>
# Copyright (C) 2012,2013 Henrik Nordstrom <henrik@henriknordstrom.net>
# Copyright (C) 2013 Patrick Wood <patrickhwood@gmail.com>
# Copyright (C) 2013 Pat Wood <Pat.Wood@efi.com>
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

PWD=$(shell pwd)
.PHONY:tools baremetal

TOOLS = \
	bootinfo \
	fel \
	nand_part \
	fexc \
	phoenix_info \
	pio

CROSS_TOOLS = \
	meminfo

BAREMETAL = \
	boot_head \
	fel-sdboot \
	jtag-loop \
	fel-pio \
	fel-copy


tools:$(TOOLS)
$(TOOLS): FORCE
	$(MAKE) -C tools/$@

baremetal:$(BAREMETAL)
$(BAREMETAL): FORCE
	$(MAKE) -C baremetal/$@

cross-tools:$(CROSS_TOOLS)
$(CROSS_TOOLS):FORCE
	$(MAKE) -C tools/$@

tools-clean:
	for d in $(TOOLS); do $(MAKE) -C tools/$$d clean; done

baremetal-clean:
	for d in $(BAREMETAL); do $(MAKE) -C baremetal/$$d clean; done

cross-tools-clean:
	for d in $(CROSS_TOOLS); do $(MAKE) -C tools/$$d clean; done

tools-install:
	for d in $(TOOLS); do $(MAKE) -C tools/$$d install; done

tools-uninstall:
	for d in $(TOOLS); do $(MAKE) -C tools/$$d uninstall; done

FORCE:
