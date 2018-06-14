ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = posixy

# Source files for posixy
posixy_SOURCES =    src/main.c \
                    src/handlers/posix_cat.c \
                    src/handlers/posix_basename.c \
                    src/handlers/posix_true_false.c


posixy_CFLAGS = -I$(top_srcdir)/src -g
posixy_LDFLAGS = -ldl

# Extra files that need to be in the distribution
EXTRA_DIST = README.md LICENSE
