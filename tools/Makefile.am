
AM_CFLAGS = -g -O0 -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200112L -I$(top_srcdir) -I$(top_srcdir)/include
AM_LDFLAGS = -L$(top_srcdir)/libfex

bin_PROGRAMS = fexc fel bootinfo pio nand-part phoenix_info

fexc_SOURCES = fexc.c
fexc_CFLAGS  = $(CFLAGS) $(AM_CFLAGS)
fexc_LDFLAGS = $(LDFLAGS) $(AM_LDFLAGS)
fexc_LDADD   = $(top_builddir)/libfex/.libs/libfex.a

fel_SOURCES = fel.c
fel_CFLAGS  = $(CFLAGS) $(AM_CFLAGS) $(LIBUSB_CFLAGS)
fel_LDFLAGS = $(LDFLAGS) $(AM_LDFLAGS) $(LIBUSB_LIBS)

bootinfo_SOURCES = bootinfo.c
bootinfo_CFLAGS  = $(CFLAGS) $(AM_CFLAGS)
bootinfo_LDFLAGS = $(LDFLAGS) $(AM_LDFLAGS)

pio_SOURCES = pio.c
pio_CFLAGS  = $(CFLAGS) $(AM_CFLAGS)
pio_LDFLAGS = $(LDFLAGS) $(AM_LDFLAGS)

nand_part_SOURCES = nand-part-main.c
nand_part_CFLAGS  = $(CFLAGS) $(AM_CFLAGS)
nand_part_LDFLAGS = $(LDFLAGS) $(AM_LDFLAGS)
nand_part_LDADD   = $(top_builddir)/tools/liba10.a \
                    $(top_builddir)/tools/liba20.a

noinst_LIBRARIES = liba10.a liba20.a

liba10_a_SOURCES = nand-part.c
liba10_a_CFLAGS  = $(CFLAGS) $(AM_CFLAGS) -DA10
liba20_a_SOURCES = nand-part.c
liba20_a_CFLAGS  = $(CFLAGS) $(AM_CFLAGS) -DA20

phoenix_info_SOURCES = phoenix_info.c
phoenix_info_CFLAGS  = $(CFLAGS) $(AM_CFLAGS)
phoenix_info_LDFLAGS = $(LDFLAGS) $(AM_LDFLAGS)

noinst_HEADERS = fexc.h          \
                 nand-common.h   \
                 nand-part-a10.h \
                 nand-part-a20.h


install-exec-hook:
	@( cd $(DESTDIR)$(bindir) ; \
	   $(LN_S) -f *fexc$(EXEEXT) fex2bin$(EXEEXT) ; \
	   $(LN_S) -f *fexc$(EXEEXT) bin2fex$(EXEEXT) ; \
	 )
