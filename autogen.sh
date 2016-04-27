#! /bin/sh


aclocal

autoheader
libtoolize --automake --copy --debug --force
automake --foreign --add-missing --copy

autoconf

