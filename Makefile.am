SUBDIRS=include lib src test
EXTRA_DIST = NEWS NEWS.xml NEWS.xsl

all-local: NEWS

NEWS: NEWS.xsl NEWS.xml
	${XSLTPROC} -o $@ NEWS.xsl NEWS.xml

ISSUEFILES = $$(find ${top_srcdir} -type f '(' \
	-name '*.cc' -or -name '*.h' -or -name '*.sql' \
	')' ) \
	${top_srcdir}/configure.ac
issues: todo fixme xxx
todo fixme xxx:
	@grep --color=auto -in '$@:' ${ISSUEFILES} || true
