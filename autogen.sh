#!/bin/sh
test -d aux.d || mkdir aux.d
   aclocal \
&& autoheader \
&& automake -a \
&& autoconf \
&& ./configure "$@"
