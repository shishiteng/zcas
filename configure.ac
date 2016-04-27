#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([include/zcas_struct.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB       #使用了静态库编译,需要此宏定义
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([float.h inttypes.h libintl.h limits.h locale.h malloc.h memory.h stddef.h stdint.h stdlib.h string.h strings.h sys/time.h unistd.h wchar.h wctype.h])

AM_INIT_AUTOMAKE
# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AM_GNU_GETTEXT
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit gettimeofday memset pow realpath regcomp select setlocale sqrt strchr strdup strstr])

AC_CONFIG_FILES([Makefile
                 src/zcas/Makefile
                 src/test/Makefile
		 src/tools/license/Makefile])

AC_OUTPUT
