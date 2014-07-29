
man_MANS = sunxi-tools.7

EXTRA_DIST = sunxi-tools.7

install-data-hook:
	@cd $(DESTDIR)$(datadir)/man/man7 ; \
	 for file in *.7 ; do               \
	   gzip -9 $$file ;                 \
	 done
