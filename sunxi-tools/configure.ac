#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([sunxi-tools], [2.0], [https://github.com/EddyBeaupre/sunxi-tools/issues])
AC_CONFIG_SRCDIR([src/sunxi-tools.c])
AC_CONFIG_HEADERS([config.h])

AC_DEFINE([PACKAGE_COPYRIGHT], ["2013 Eddy Beaupre <eddy@beaupre.biz>"], [Define to the copyright date and holder name])
AC_DEFINE([PACKAGE_LICENCE], ["GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html"], [Define to the licence type and description])
AC_DEFINE([PACKAGE_HOMEPAGE], ["https://github.com/EddyBeaupre/sunxi-tools"], [Define to the address where the sources for this package can be found.])
AC_DEFINE([PACKAGE_HELPPAGE], ["https://github.com/linux-sunxi/"], [Define to the address where general help can be found.])

LT_INIT()
AC_CONFIG_MACRO_DIR([m4])

# automake initialisation and check for minimal automake API version 1.9
AM_INIT_AUTOMAKE([1.9])
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

AM_GNU_GETTEXT_VERSION([0.18.1]) 
AM_GNU_GETTEXT([external])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([setlocale])

AC_CONFIG_FILES([Makefile po/Makefile.in src/Makefile])
AC_OUTPUT
