#
# build "preprocessed" .h files for inclusion of ARM scratch code
#

FEL_EXEC_THUNKS := fel-to-spl-thunk.h
FEL_EXEC_THUNKS += fel-to-secure-svc-smc-thunk.h
FEL_EXEC_THUNKS += fel-rx-dma-thunk.h
THUNKS := clrsetbits.h
THUNKS += memcpy.h
THUNKS += readl_writel.h
THUNKS += rmr-thunk.h
THUNKS += sid_read_root.h

all: $(FEL_EXEC_THUNKS) $(THUNKS)
# clean up object files afterwards
	rm -f *.o

# This empty prerequisite enforces a rebuild of all the headers on every run
FORCE:

# If not specified explicitly: try to guess a suitable ARM toolchain prefix
CROSS_COMPILE ?= $(shell ../find-arm-gcc.sh)

AS := $(CROSS_COMPILE)as
OBJDUMP := $(CROSS_COMPILE)objdump

AWK_O_TO_H := LC_ALL=C awk -f objdump_to_h.awk

# These thunks are copied into a uint32_t buffer and byte-swapped by fel.c.
# The "style" variable for awk controls this, and causes the htole32()
# conversion to be omitted.
$(FEL_EXEC_THUNKS): %.h: %.S FORCE
	$(AS) -o $(subst .S,.o,$<) -march=armv5te $<
	$(OBJDUMP) -d $(subst .S,.o,$<) | $(AWK_O_TO_H) -v style=old > $@

$(THUNKS): %.h: %.S FORCE
	$(AS) -o $(subst .S,.o,$<) -march=armv5te $<
	$(OBJDUMP) -d $(subst .S,.o,$<) | $(AWK_O_TO_H) > $@
