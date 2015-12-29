#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([sunxi-tools], [V1.4], [https://github.com/linux-sunxi/sunxi-tools])
AC_CONFIG_SRCDIR([sunxi-nand.c])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LN_S

AC_MSG_CHECKING([whether we are building for the arm target])
AS_IF([test x$host_cpu == xarm], [armbuild=true])
AS_IF([test x$host_cpu == xarmv7l], [armbuild=true])
AS_IF([test x$armbuild = xtrue], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])])
AM_CONDITIONAL([ARM_BUILD], [test x$armbuild = xtrue])

# Checks for libraries.

CPPFLAGS_SAVE="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -I/usr/include/libusb-1.0"
LDFLAGS_SAVE="$LDFLAGS"
LDFLAGS="$LDFLAGS -lusb-1.0"
AC_SEARCH_LIBS([libusb_bulk_transfer], [usb-1.0], [USB_LDFLAGS="-lusb-1.0"])
AS_IF([test x$USB_LDFLAGS != x], [AC_CHECK_HEADERS([libusb.h], [USB_CFLAGS="-I/usr/include/libusb-1.0"])])
CPPFLAGS="$CPPFLAGS_SAVE"
LDFLAGS="$LDFLAGS_SAVE"

AC_MSG_CHECKING([whether we can build sunxi-fel])
AS_IF([test x$USB_CFLAGS != x], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])])
AM_CONDITIONAL([BUILD_SUNXIFEL], [test x$USB_CFLAGS != x])

# Checks if we can build the bare-metal tools.
AC_PATH_PROG([ARM_GCC], [arm-none-eabi-gcc])
AC_PATH_PROG([ARM_CPY], [arm-none-eabi-objcopy])
AC_PATH_PROG([ARM_NM], [arm-none-eabi-nm])
AC_MSG_CHECKING([whether we can build the bare metal tools])
AS_IF([test x$cross_compiling = xyes], [baremetal=false], AS_IF([test -z "$ARM_NM"], [baremetal=false], AS_IF([test -z "$ARM_CPY"], [baremetal=false], AS_IF([test -z "$ARM_GCC"], [baremetal=false], [baremetal=true]))))
AS_IF([test x$baremetal = xtrue], [AC_MSG_RESULT([yes])], [AC_MSG_RESULT([no])])

AM_CONDITIONAL([BUILD_BAREMETAL], [test x$baremetal = xtrue])

AC_SUBST([USB_LDFLAGS])
AC_SUBST([USB_CFLAGS])
AC_SUBST([ARM_GCC]) 
AC_SUBST([ARM_CPY])
AC_SUBST([ARM_NM])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
