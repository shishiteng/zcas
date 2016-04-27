AUTOMAKE_OPTIONS=


SUBDIRS=src/zcas \
        src/test \
	src/tools/license

EXTRA_DIST =conf/\
	    data/dict\
	    data/license\
            include/\
            lib/

dictdir=$(prefix)/data/dict
dict_DATA=data/dict/*

confdir=$(prefix)/conf
conf_DATA=conf/*

licensedir=$(prefix)/data/license
license_DATA=data/license/*

INCLUDES=-I$(top_srcdir)/src/zcas\
         -I$(top_srcdir)/src/common\
         -I$(top_srcdir)/src/test\
	 -I$(top_srcdir)/src/tools/license


export INCLUDES 
