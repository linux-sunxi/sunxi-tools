CC = gcc
CFLAGS = -g -O2 -Wall -Wextra -std=c99

TOOLS = bin2fex fex2bin

.PHONY: all clean

all: $(TOOLS)

clean:
	@rm -vf $(TOOLS)


$(TOOLS): Makefile sunxi-tools.h

fex2bin: script.c script.h
bin2fex: script.h

%: %.c %.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS)

.gitignore: Makefile
	@for x in $(TOOLS) '*.o' '*.swp'; do \
		echo $$x; \
	done > $@
