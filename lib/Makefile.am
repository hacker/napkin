lib_LTLIBRARIES = libnapkin.la

INCLUDES = -I${top_builddir}/include/ -I${top_srcdir}/include/ \
	   ${MODULES_CFLAGS}
LIBS = ${MODULES_CFLAGS}

libnapkin_la_SOURCES = \
	st-decode.cc st-download.cc \
	hypnodata.cc \
	util.cc
libnapkin_la_LDFLAGS = -version-info 0:0:0
