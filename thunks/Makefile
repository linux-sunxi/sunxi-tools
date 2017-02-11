#
# build "preprocessed" .h files for inclusion of ARM scratch code
#

SPL_THUNK := fel-to-spl-thunk.h
THUNKS := clrsetbits.h
THUNKS += memcpy.h
THUNKS += readl_writel.h
THUNKS += rmr-thunk.h
THUNKS += sid_read_root.h

all: $(SPL_THUNK) $(THUNKS)
# clean up object files afterwards
	rm -f *.o

# This empty prerequisite enforces a rebuild of all the headers on every run
FORCE:

# If not specified explicitly: try to guess a suitable ARM toolchain prefix
CROSS_COMPILE ?= $(shell ../find-arm-gcc.sh)

AS := $(CROSS_COMPILE)as
OBJDUMP := $(CROSS_COMPILE)objdump

AWK_O_TO_H := LC_ALL=C awk -f objdump_to_h.awk

# The SPL thunk requires a different output format. The "style" variable for
# awk controls this, and causes the htole32() conversion to be omitted.
fel-to-spl-thunk.h: fel-to-spl-thunk.S FORCE
	$(AS) -o $(subst .S,.o,$<) $<
	$(OBJDUMP) -d $(subst .S,.o,$<) | $(AWK_O_TO_H) -v style=old > $@

$(THUNKS): %.h: %.S FORCE
	$(AS) -o $(subst .S,.o,$<) $<
	$(OBJDUMP) -d $(subst .S,.o,$<) | $(AWK_O_TO_H) > $@
