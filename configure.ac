#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([submit_mail], [0.1], [don@donm.cc])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([1.14 foreign subdir-objects -Wall -Werror])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([lib/submit_mail/config.h])

# Checks for programs.
AM_PROG_AR
AC_PROG_CC_C99
AC_PROG_RANLIB

# Checks for libraries.
AC_SEARCH_LIBS([curl_global_init], [curl])

# Checks for header files.
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([strdup])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

