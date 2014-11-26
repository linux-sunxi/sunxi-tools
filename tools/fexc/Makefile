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

include ../tools.env

TARGET = fecx
SOURCES = \
	fexc.c \
	script_bin.c \
	script.c \
	script_fex.c \
	script_uboot.c

EXTRA_DEPS = \
	fex2bin \
	bin2fex

HEADERS = \
	include/script.h \
	include/script_uboot.h \
	include/script_bin.h \
	include/script_fex.h

INSTALL_DIR = bin

include ../tools.rules
install: $(TARGET) fex2bin bin2fex
	mkdir -p $(PREFIX)/$(INSTALL_DIR)
	install -o root -m 755 -t $(PREFIX)/$(INSTALL_DIR) $(TARGET) fex2bin bin2fex
uninstall:
	rm -f $(addprefix $(PREFIX)/$(INSTALL_DIR)/,$(TARGET) fex2bin bin2fex)
	rmdir -p $(PREFIX)/$(INSTALL_DIR)
fex2bin bin2fex:
	ln -s $(TARGET) $@
