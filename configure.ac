AC_INIT([posixy], [0.0.1], [nirenjan@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CC

AC_PROG_MKDIR_P
AC_PROG_LN_S

AM_CONDITIONAL([LINUX], [test "`uname -s`" = Linux])

AC_CONFIG_FILES([
    Makefile
])
AC_OUTPUT
