CC = gcc
AR = ar

srctree	?= ../..
O	?= $(srctree)/out

OBJDIR	:= $(O)/libfex
LIBFEX	:= $(O)/libfex.a

SRCS	:= $(wildcard *.c)
OBJS	:= $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

INCLUDES = -I$(srctree)/include
CFLAGS	 = -O2
CFLAGS  += -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200112L
CFLAGS  += $(INCLUDES)

DEPS	:= $(OBJDIR)/deps.mk

# libfex.a
$(LIBFEX): $(OBJS)
	@mkdir -vp $(@D)
	$(AR) rcs $@ $^

# out/foo.o: foo.c
$(OBJS): SRC=$(patsubst $(OBJDIR)/%.o,%.c,$@)
$(OBJS):
	@mkdir -vp $(@D)
	$(CC) $(CFLAGS) -c -o $@ $(SRC)

# dependencies
$(DEPS): Makefile
	mkdir -p $(@D)
	for x in $(SRCS); do \
		$(CC) -MM -MT $(OBJDIR)/$${x%.c}.o $$x $(INCLUDES); \
	done > $@~
	mv $@~ $@

include $(DEPS)
