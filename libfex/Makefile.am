
AM_CFLAGS = -g -O0 -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200112L -I$(top_srcdir) -I$(top_srcdir)/include
AM_LDFLAGS = 

lib_LTLIBRARIES = libfex.la

libfex_la_SOURCES = $(top_srcdir)/include/libfex/script.h       script.c       \
                    $(top_srcdir)/include/libfex/script_uboot.h script_uboot.c \
                    $(top_srcdir)/include/libfex/script_bin.h   script_bin.c   \
                    $(top_srcdir)/include/libfex/script_fex.h   script_fex.c

libfex_la_includedir = $(includedir)/libfex
libfex_la_include_HEADERS   = $(top_srcdir)/include/libfex/script.h       \
                              $(top_srcdir)/include/libfex/script_uboot.h \
                              $(top_srcdir)/include/libfex/script_bin.h   \
                              $(top_srcdir)/include/libfex/script_fex.h

libfex_la_CFLAGS  = $(CFLAGS) $(AM_CFLAGS)
libfex_la_LDFLAGS = $(LDFLAGS) $(AM_LDFLAGS)
