#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([sunxi-tools], [1.3], [https://github.com/linux-sunxi/sunxi-tools])
AC_CONFIG_SRCDIR([tools/fexc.c])

AC_CANONICAL_SYSTEM

# automake initialisation and check for minimal automake API version 1.9
AM_INIT_AUTOMAKE([1.9])
LT_INIT

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AM_PROG_AS

AM_PROG_CC_C_O


AC_CHECK_PROGS(CC_FOR_BUILD, gcc cc)
AC_CHECK_PROGS(CXX_FOR_BUILD, g++)
AC_CHECK_PROGS(OBJCOPY, objcopy)
AC_CHECK_PROGS(OBJDUMP, objdump)
AC_CHECK_PROGS(AR, ar)
AC_CHECK_PROGS(NM, nm)

AC_SUBST([CCC_FOR_BUILD])
AC_SUBST([CXX_FOR_BUILD])
AC_SUBST([OBJCOPY])
AC_SUBST([OBJDUMP])
AC_SUBST([AR])
AC_SUBST([NM])

AM_CONDITIONAL([HOST_CPU_ARM], [test "$host_cpu" = "arm"])

case "$host_cpu" in
  arm*)
    THUMB_CFLAGS="$THUMB_CFLAGS -mno-thumb-interwork"
    ;;
esac

AC_SUBST([THUMB_CFLAGS])


AC_PROG_LN_S
AC_PROG_MKDIR_P

# Checks for libraries.
PKG_CHECK_MODULES([LIBUSB], libusb-1.0 >= 1.0.0,, AC_MSG_ERROR([*** Required libusb-1.0 >= 1.0.0 not installed ***]))

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/mount.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

dnl Following macro generates '#define malloc rpl_malloc' it's lead to link stage error:
dnl AC_FUNC_MALLOC
dnl AC_FUNC_REALLOC

# Checks for library functions.
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap strchr strerror strtol strtoul])

AC_CONFIG_FILES([Makefile libfex/Makefile tools/Makefile src/Makefile binaries/Makefile scripts/Makefile man/Makefile])
AC_OUTPUT
