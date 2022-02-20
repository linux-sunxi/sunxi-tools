#
# tests/Makefile
#
# Copyright (C) 2016  Bernhard Nortmann <bernhard.nortmann@web.de>
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

BOARDS_URL := https://github.com/linux-sunxi/sunxi-boards/archive/master.zip
BOARDS_DIR := sunxi-boards

check: check_all_fex coverage

# Conversion cycle (.fex -> .bin -> .fex) test for all sunxi-boards
check_all_fex: $(BOARDS_DIR)/README unify-fex
	./test_all_fex.sh $(BOARDS_DIR)

coverage:
	# Usage help / invocation with no args
	../sunxi-fexc -? 2> /dev/null ; exit 0
	# Improve code coverage for corner cases (e.g. erroneous parameters)
	./test_fex2bin_corner_cases.sh
	./test_bin2fex_corner_cases.sh

# Retrieve and extract sunxi-boards archive (containing all .fex)
$(BOARDS_DIR).zip:
	curl -fLsS -o $@ $(BOARDS_URL)
$(BOARDS_DIR)/README: $(BOARDS_DIR).zip
	@echo Extracting $< ...
	unzip -q $<
	mv sunxi-boards-master $(BOARDS_DIR)
	touch -r $(BOARDS_DIR) $<
	cat patches/*.patch | patch -p1

unify-fex: unify-fex.c
	$(CC) -Wall -Werror -o $@ $<

clean:
	rm -rf $(BOARDS_DIR).zip $(BOARDS_DIR) unify-fex

#
# Dedicated rule for Travis CI test of sunxi-boards. This assumes that the
# sunxi-tools source (archive) was extracted into a subdir below the working
# directory, meaning that BOARDS_DIR should be "../.."
#
sunxi-boards_CI: unify-fex
	# compile sunxi-fexc, link bin2fex and fex2bin
	make -C .. bin2fex fex2bin
	# apply patches to BOARDS_DIR, ignore mismatches
	cat patches/*.patch | patch --forward -r- -p2 -d $(BOARDS_DIR) || true
	# and finally run the tests
	./test_all_fex.sh $(BOARDS_DIR)
