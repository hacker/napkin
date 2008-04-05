bin_PROGRAMS = napkin

AM_CXXFLAGS = ${MODULES_CFLAGS} -I${top_srcdir}/include/ -I${srcdir}
LIBS = ${MODULES_LIBS} \
       ${top_builddir}/lib/libnapkin.la

noinst_HEADERS = sqlite.h db.h \
	sleep_timeline.h \
	widgets.h dialogs.h \
	sleep_history.h

napkin_SOURCES = napkin.cc \
     db.cc \
     sleep_timeline.cc \
     widgets.cc dialogs.cc \
     sleep_history.cc \
     schema.cc COPYING.cc
napkin_DEPENDENCIES = \
	${top_builddir}/lib/libnapkin.la

EXTRA_DIST = schema.sql

schema.cc: schema.sql
	(\
	  echo 'namespace napkin{const char *sql_bootstrap=' &&\
	  sed -e 's/^\s\+//' -e 's/\s*--.*$$//' -e 's/^/"/' -e 's/$$/"/' $< &&\
	  echo ';}'\
	) >$@
COPYING.cc: ${top_srcdir}/COPYING
	echo "const char * COPYING =" >$@ || (rm $@;exit 1)
	sed 's/"/\\"/g' $< | sed 's/^/\"/' | sed 's/$$/\\n\"/' >>$@ || (rm $@;exit 1)
	echo ";" >>$@ || (rm $@;exit 1)
