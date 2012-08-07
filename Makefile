CC = gcc
CFLAGS = -g -O0 -Wall -Wextra
CFLAGS += -std=c99 -D_POSIX_C_SOURCE=200112L
CFLAGS += -Iinclude/

TOOLS = fexc bin2fex fex2bin bootinfo fel pio
TOOLS += nand-part

MISC_TOOLS = phoenix_info

.PHONY: all clean

all: $(TOOLS)

misc: $(MISC_TOOLS)

clean:
	@rm -vf $(TOOLS) $(MISC_TOOLS)


$(TOOLS): Makefile common.h

fex2bin bin2fex: fexc
	ln -s $< $@

fexc: fexc.h script.h script.c \
	script_bin.h script_bin.c \
	script_fex.h script_fex.c

bootinfo: bootinfo.c

LIBUSB = libusb-1.0
LIBUSB_CFLAGS = `pkg-config --cflags $(LIBUSB)`
LIBUSB_LIBS = `pkg-config --libs $(LIBUSB)`

fel: fel.c
	$(CC) $(CFLAGS) $(LIBUSB_CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS) $(LIBUSB_LIBS)

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.c,$^) $(LIBS)

.gitignore: Makefile
	@for x in $(TOOLS) '*.o' '*.swp'; do \
		echo "$$x"; \
	done > $@
