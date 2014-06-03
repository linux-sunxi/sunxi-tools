.PHONY: all libs

CC = gcc
AR = ar

srctree	?= ..
O	?= $(srctree)/out

OBJDIR	:= $(O)/lib
DEPS	:= $(OBJDIR)/deps.mk

INCLUDES = -I$(srctree)/include
CFLAGS	 = -O2
CFLAGS  += -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200112L
CFLAGS  += $(INCLUDES)

SRCS	:=

all: libs

include libfex/Makefile

# *.a
%.a:
	@mkdir -vp $(@D)
	$(AR) rcs $@ $^

# *.o
%.o: SRC=$(patsubst $(OBJDIR)/%.o,%.c,$@)
%.o:
	@mkdir -vp $(@D)
	$(CC) $(CFLAGS) -c -o $@ $(SRC)

# dependencies
$(DEPS): Makefile libfex/Makefile
	mkdir -p $(@D)
	for x in $(SRCS); do \
		$(CC) -MM -MT $(OBJDIR)/$${x%.c}.o $$x $(INCLUDES); \
	done > $@~
	mv $@~ $@

include $(DEPS)
