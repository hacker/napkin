#!/bin/sh
test -d aux.d || mkdir aux.d
   libtoolize -f \
&& aclocal \
&& autoheader \
&& automake -a \
&& autoconf \
&& ./configure "$@"
