.PHONY: all libs

CC = gcc
AR = ar

srctree	?= ..
O	?= $(srctree)/out

OBJDIR	:= $(O)/lib
DEPS	:= $(OBJDIR)/deps.mk
SRCS	:=

INCLUDES = -I$(srctree)/include
CFLAGS	 = -O2
CFLAGS  += -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200112L
CFLAGS	+= -fPIC
CFLAGS  += $(INCLUDES)

all: libs

include libfex/Makefile

# libraries
libs: $(LIBS) $(SOLIBS)

$(LIBS):
	@mkdir -vp $(@D)
	$(AR) rcs $@ $^

$(SOLIBS):
	@mkdir -vp $(@D)
	$(CC) -shared -Wl,-soname,$(@F) -o $@ $^
	x=$@; while [ "$$x" = "$${x%.so}" ]; do \
	  x=$${x%.*}; \
	  ln -snvf $(@F) $$x; \
	done

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
