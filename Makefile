CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c99

TOOLS = bin2fex fex2bin

.PHONY: all clean

all: $(TOOLS)

clean:
	@rm -vf $(TOOLS)

.gitignore: Makefile
	@for x in $(TOOLS) '*.o' '*.swp'; do \
		echo $$x; \
	done > $@

