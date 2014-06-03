.PHONY: all clean

CC = gcc
AR = ar

srctree	?= ..
O	?= $(srctree)/out

OBJDIR	:= $(O)/tools
TOOLS	:= fexc fex2bin bin2fex
SRCS	:= fexc.c

INCLUDES = -I$(srctree)/include
CFLAGS	 = -O2
CFLAGS  += -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200112L
CFLAGS  += $(INCLUDES)

DEPS	:= $(OBJDIR)/deps.mk

all: $(addprefix $(O)/,$(TOOLS))
	echo $^

clean:
	rm -vf $(DEPS)

# fexc
$(O)/fexc: $(OBJDIR)/fexc.o $(O)/libfex.a
	$(CC) -o $@ $^ $(CFLAGS)

%.o:
	mkdir -p $(@D)
	$(CC) -c -o $@ $(filter %.c,$^) $(CFLAGS)

$(O)/fex2bin $(O)/bin2fex: $(O)/fexc
	ln -svnf fexc $@

# dependencies
$(DEPS): Makefile
	mkdir -p $(@D)
	for x in $(SRCS); do \
		$(CC) -MM -MT $(OBJDIR)/$${x%.c}.o $$x $(INCLUDES); \
	done > $@~
	mv $@~ $@

include $(DEPS)
