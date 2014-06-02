#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([sunxi-tools], [1.0], [https://github.com/linux-sunxi/sunxi-tools])
AC_CONFIG_SRCDIR([fel-copy.c])

# automake initialisation and check for minimal automake API version 1.9
AM_INIT_AUTOMAKE([1.9])

# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S
AC_CANONICAL_BUILD

if test x"$cross_compiling" != x"yes" ; then
  AC_CHECK_PROG(ARM_NONE_EABI_CHECK, arm-none-eabi-gcc, yes)
  if test x"$ARM_NONE_EABI_CHECK" != x"yes" ; then
    AC_CHECK_PROG(ARM_LINUX_GNUEABIHF_CHECK, arm-linux-gnueabihf-gcc, yes)
      if test x"$ARM_LINUX_GNUEABIHF_CHECK" != x"yes" ; then
        AC_MSG_ERROR([Please install GNU Tools for ARM Embedded Processors.])
      else
        XCOMPILER="arm-linux-gnueabihf-"
      fi
  else
    XCOMPILER="arm-none-eabi-"
  fi
fi

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

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset munmap strchr strerror strtol strtoul])

AC_SUBST([XCOMPILER])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
