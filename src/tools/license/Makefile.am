INCLUDES=-I$(top_srcdir)/src/tools/license

lib_LTLIBRARIES = libzclicense.la
bin_PROGRAMS = licensetool

libzclicense_la_CFLAGS=-DOS_LINUX
libzclicense_la_SOURCES =md5.c zctool_license.c

licensetool_CFLAGS=-DOS_LINUX
licensetool_LDADD=-L$(top_srcdir)/src/tools/license -lzclicense
licensetool_SOURCES =md5.c zctool_license.c zctool_test.c


