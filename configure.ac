#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT
AM_INIT_AUTOMAKE([ifconf], [`cat VERSION`])
AC_CONFIG_SRCDIR([ifconf.c])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_CANONICAL_HOST
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_CHECK_FUNCS([inet_ntoa socket strerror])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "ifconf has been configured with the following options:"
echo "   compiler       : $CC"
echo "   opt_options    : $opt_options"
echo "   host_cpu       : $host_cpu"
echo "   host_vendor    : $host_vendor"
echo "   host_os        : $host_os"
echo "   CFLAGS         : $CFLAGS"
echo "   CPPFLAGS       : $CPPFLAGS"
echo "   lib            : $LIBS"
echo "   defs           : $DEFS"
echo "   include        : $INCLS"
echo "   prefix         : $prefix"
echo "   exec_prefix    : $exec_prefix"
echo "   bindir         : $bindir"
echo "   sbindir        : $sbindir"
echo "   libexecdir     : $libexecdir"
echo "   sysconfdir     : $sysconfdir"
echo "   libdir         : $libdir"
echo "   infodir        : $infodir"
echo "   mandir         : $mandir"
echo "   datadir        : $datadir"

