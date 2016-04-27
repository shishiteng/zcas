INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/src/zcas -I$(top_srcdir)/src/common

bin_PROGRAMS = zcas

zcas_CFLAGS=-DOS_LINUX
zcas_LDFLAGS=-Wl,-rpath=./lib/x64:../lib/x64:../../lib/x64
zcas_LDADD=-L$(top_srcdir)/src/zcas/ -lzcas
zcas_SOURCES =zcas_test.c
