#
# tests/Makefile
#

BOARDS_URL := https://github.com/linux-sunxi/sunxi-boards/archive/master.zip
BOARDS_DIR := sunxi-boards

check: check_all_fex

check_all_fex: $(BOARDS_DIR)/README unify-fex
	./test_all_fex.sh $(BOARDS_DIR)

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
