AM_CPPFLAGS = -I$(top_srcdir)/include


bin_PROGRAMS = sunxi-tools
sunxi_tools_SOURCES = sunxi-tools.c common.c fexc.c script_bin.c script.c script_fex.c script_uboot.c common.h fexc.h script_bin.h script_fex.h script.h script_uboot.h
sunxi_tools_LDFLAGS = $(LTLIBINTL)

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

astyle:
	for x in $(sunxi_tools_SOURCES); do astyle -A2 -j $$x; done
