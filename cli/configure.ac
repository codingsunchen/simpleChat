#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(client, 1.0, 610198285@qq.com)
AC_CONFIG_SRCDIR([fun.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE()

# Checks for programs.
AC_PROG_CXX
#AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T

# Checks for library functions.
AC_CHECK_FUNCS([memset socket])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
