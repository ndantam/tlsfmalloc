#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([tlsf], [2.4.6], [ntd@gatech.edu])
AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_SRCDIR([src/tlsf.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AC_PROG_LIBTOOL

# Enable maximum warnings
AX_CFLAGS_WARN_ALL

AC_CHECK_HEADERS([stdio.h])

dnl The AX_CHECK_COMPILER_FLAGS maco was renamed to AX_CHECK_COMPILE_FLAG
dnl Checking for both versions
m4_ifdef([AX_CHECK_COMPILE_FLAG],
         [AC_DEFUN([APPEND_CFLAG],
                   [AX_CHECK_COMPILE_FLAG([$1], [CFLAGS="$1 $CFLAGS"])])])

m4_ifdef([AX_CHECK_COMPILER_FLAGS],
         [AC_DEFUN([APPEND_CFLAG],
                   [AX_CHECK_COMPILER_FLAGS([$1], [CFLAGS="$1 $CFLAGS"])])])

m4_ifdef([APPEND_CFLAG],
         [AC_LANG(C)
          APPEND_CFLAG([-Wextra])
          APPEND_CFLAG([-Wwrite-strings])
          APPEND_CFLAG([-Wstrict-prototypes])
          APPEND_CFLAG([-Wmissing-prototypes])
          APPEND_CFLAG([-Wno-long-long])
          APPEND_CFLAG([-Wstrict-aliasing=2])])

# Debugging
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug], [Build for debugging])])

AS_IF([test "x$enable_debug" = "xyes"],
      [AC_LANG([C])
       APPEND_CFLAG([-fstack-check])],
      [enable_debug=no
       AC_LANG([C])
       APPEND_CFLAG([-DNDEBUG])])

AC_ARG_ENABLE([sbrk],
              [AS_HELP_STRING([--disable-sbrk], [Do not use sbrk() to grow heap])])

AS_IF([test "x$enable_sbrk" != "xno"],
      [AC_DEFINE([USE_SBRK],[1],[Use sbrk() to grow heap])
       enable_sbrk=yes],
      [enable_sbrk=no])

AC_ARG_ENABLE([mmap],
              [AS_HELP_STRING([--disable-mmap], [Do not use mmap() to grow heap])])

AS_IF([test "x$enable_mmap" != "xno"],
      [AC_DEFINE([USE_MMAP],[1],[Use mmap() to grow heap])
       enable_mmap=yes],
      [enable_mmap=no])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

AC_MSG_NOTICE([])
AC_MSG_NOTICE([CONFIGURATION SUMMARY])
AC_MSG_NOTICE([=====================])
AC_MSG_NOTICE([PREFIX:          $prefix])
AC_MSG_NOTICE([DEBUG:           $enable_debug])
AC_MSG_NOTICE([USE_SBRK:        $enable_sbrk])
AC_MSG_NOTICE([USE_MMAP:        $enable_mmap])
