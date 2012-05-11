CC = gcc
CFLAGS = -g -O2 -Wall -Wextra
CFLAGS += -std=c99 -D_POSIX_C_SOURCE=200112L

TOOLS = fexc bin2fex fex2bin

.PHONY: all clean

all: $(TOOLS)

clean:
	@rm -vf $(TOOLS)


$(TOOLS): Makefile common.h

fex2bin bin2fex: fexc
	ln -s $< $@

fexc: script.h script.c \
	script_bin.h script_bin.c \
	script_fex.h script_fex.c

%: %.c %.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS)

.gitignore: Makefile
	@for x in $(TOOLS) '*.o' '*.swp'; do \
		echo $$x; \
	done > $@
