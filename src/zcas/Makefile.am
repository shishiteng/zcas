
lib_LTLIBRARIES = libzcas.la
INSTALLdir=$(prefix)/include
INSTALL_HEADERS=$(top_srcdir)/src/zcas/zcas_struct.h\
		$(top_srcdir)/src/zcas/zcas_interface.h

INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/src/common -I$(top_srcdir)/src/tools/license -I/usr/local/include

libzcas_la_CFLAGS= -DOS_LINUX 

#libzcas_la_LIBDIR=$(prefix)/usr/local/lib

#libzcas_la_LDFLAGS= $(LIBXML_LIBS) $(LIBSSL_LIBS)

libzcas_la_LIBADD=-lzcfc  

libzcas_la_SOURCES =    zcas_part.c\
			zcas_paragraph.c\
			zcas_sentence.c\
			zcas_vector.c\
			zcas_result.c\
			zcas_init.c\
			zcas_summary.c\
			../common/zcas_common.c\
			../tools/license/zctool_license.c
