ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = posixy

# Source files for posixy
posixy_SOURCES =    src/main.c \
                    src/handlers/cat.c \
                    src/handlers/basename.c \
                    src/handlers/tee.c \
                    src/handlers/true_false.c


posixy_CFLAGS = -I$(top_srcdir)/src -g -Wl,--export-dynamic '-DPROGNAME="posixy"'
posixy_LDADD = -ldl

# Extra files that need to be in the distribution
EXTRA_DIST = README.md LICENSE
