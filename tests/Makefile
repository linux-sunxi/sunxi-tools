#
# tests/Makefile
#

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
