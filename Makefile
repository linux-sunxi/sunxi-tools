CC = gcc
CFLAGS = -g -O2 -Wall -Wextra -std=c99

TOOLS = bin2fex fex2bin

.PHONY: all clean

all: $(TOOLS)

clean:
	@rm -vf $(TOOLS)

$(TOOLS): sunxi-tools.h
bin2fex: bin2fex.h

.gitignore: Makefile
	@for x in $(TOOLS) '*.o' '*.swp'; do \
		echo $$x; \
	done > $@

