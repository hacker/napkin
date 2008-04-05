AC_INIT([napkin], [0.0], [napkin-bugs@klever.net])
AC_CONFIG_AUX_DIR([aux.d])
AC_CONFIG_SRCDIR([src/napkin.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([dist-bzip2])

AC_PROG_INSTALL
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AC_HEADER_STDC

AC_PATH_PROG([XSLTPROC],[xsltproc],[true])

PKG_CHECK_MODULES([MODULES],[gtkmm-2.4 sqlite3],,[
 AC_MSG_ERROR([not all dependencies could be satisfied])
])

AC_MSG_CHECKING([whether to enable debugging code])
ndebug=true
AC_ARG_ENABLE([debug],
 AC_HELP_STRING([--enable-debug],[enable debugging/development code]),
 [ test "$enableval" = "no" || ndebug=false ]
)
if $ndebug ; then
 AC_MSG_RESULT([no])
 CPPFLAGS="${CPPFLAGS}-DNDEBUG"
else
 AC_MSG_RESULT([yes])
fi

nitpick=false
AC_MSG_CHECKING([whether to enable compiler nitpicking])
AC_ARG_ENABLE([nitpicking],
 AC_HELP_STRING([--enable-nitpicking],[make compiler somewhat overly fastidious about the code it deals with]),
 [ test "$enableval" = "no" || nitpick=true ]
)
if $nitpick ; then
 AC_MSG_RESULT([yes])
 CPP_NITPICK="-pedantic -Wall -Wextra -Wundef -Wshadow \
  -Wunsafe-loop-optimizations -Wconversion -Wmissing-format-attribute \
  -Wredundant-decls -ansi"
 # -Wlogical-op -Wmissing-noreturn 
 C_NITPICK="$CPP_NITPICK"
 CXX_NITPICK="$C_NITPICK"

 CPPFLAGS="$CPPFLAGS $CPP_NITPICK"
 CFLAGS="$CFLAGS $C_NITPICK"
 CXXFLAGS="$CXXFLAGS $CXX_NITPICK"
else
 AC_MSG_RESULT([no])
fi


AC_CONFIG_FILES([
 Makefile
  include/Makefile
  lib/Makefile
  src/Makefile
  test/Makefile
])
AC_OUTPUT

dnl vim:set ft=m4 sw=1:
