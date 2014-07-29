
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = m4 autogen.sh include/common.h include/endian_compat.h include/types.h

if HOST_CPU_ARM
  MAYBE_SRC = src
endif

SUBDIRS = libfex tools binaries scripts man $(MAYBE_SRC)

dist_doc_DATA = COPYING README

docdir = $(datadir)/doc/@PACKAGE@-@VERSION@
