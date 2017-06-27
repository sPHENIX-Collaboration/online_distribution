dnl Process this file with autoconf to produce a configure script.
AC_INIT(pmonitor,[1.00])
AC_CONFIG_SRCDIR([configure.ac])

dnl Checks for C++ compiler and sets default flags
AC_PROG_CXX(cxx CC g++)

AM_INIT_AUTOMAKE


AC_CANONICAL_HOST

LT_INIT([disable-static])

AC_PROG_INSTALL


dnl   no point in suppressing warnings people should 
dnl   at least see them, so here we go for g++: -Wall
if test $ac_cv_prog_gxx = yes; then
  CXXFLAGS="$CXXFLAGS -Wall"
fi

ROOTLIBS=`root-config --libs`
ROOTINC=`root-config --incdir`
AC_SUBST(ROOTLIBS)
AC_SUBST(ROOTINC)

dnl test for root 6
if test `root-config --version | gawk '{print $1>=6.?"1":"0"}'` = 1; then
CINTDEFS=" -noIncludePaths  -inlineInputHeader "
AC_SUBST(CINTDEFS)
fi

AC_CHECK_FILE( $OFFLINE_MAIN/include/FROG.h,have_frog=yes, have_frog=no)
AC_MSG_RESULT([$have_frog])

AM_CONDITIONAL(FROG, test "$have_frog" = yes )

if test $have_frog = yes; then
  AC_DEFINE(HAVE_FROG)
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
