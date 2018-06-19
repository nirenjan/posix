ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = posixy

# Source files for posixy
HANDLERS = \
		src/handlers/basename.c \
		src/handlers/cat.c \
		src/handlers/dirname.c \
		src/handlers/false.c \
		src/handlers/logname.c \
		src/handlers/sleep.c \
		src/handlers/tee.c \
		src/handlers/true.c

posixy_SOURCES =    src/main.c $(HANDLERS)


posixy_CFLAGS = -I$(top_srcdir)/src -g '-DPROGNAME="posixy"'
if LINUX
posixy_LDFLAGS = -Wl,--export-dynamic
endif

posixy_LDADD = -ldl

# Extra files that need to be in the distribution
EXTRA_DIST = README.md LICENSE install-links

# Install rule for creating symbolic links
install-exec-local:
	$(MKDIR_P)	$(DESTDIR)${bindir}
	$(top_srcdir)/install-links install "$(LN_S)" $(DESTDIR)${bindir} $(HANDLERS)

uninstall-hook:
	$(top_srcdir)/install-links uninstall $(DESTDIR)${bindir} $(HANDLERS)
