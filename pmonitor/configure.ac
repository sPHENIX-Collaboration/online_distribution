dnl Process this file with autoconf to produce a configure script.
AC_INIT(pmonitor,[1.00])
AC_CONFIG_SRCDIR([configure.ac])

dnl Checks for C++ compiler and sets default flags
AC_PROG_CXX(cxx CC g++)

AC_CANONICAL_HOST
AC_CONFIG_AUX_DIR([.])

AM_INIT_AUTOMAKE

LT_INIT([disable-static])

AC_PROG_INSTALL


dnl   no point in suppressing warnings people should 
dnl   at least see them, so here we go for g++: -Wall
if test $ac_cv_prog_gxx = yes; then
  CXXFLAGS="$CXXFLAGS -Wall -Werror -Wno-vexing-parse -Wno-unused-parameter"
fi

ROOTCFLAGS=`root-config --cflags`
ROOTLIBS=`root-config --libs`
ROOTINC=`root-config --incdir`
ROOTCFLAGS=`root-config --cflags`
AC_SUBST(ROOTLIBS)
AC_SUBST(ROOTINC)
AC_SUBST(ROOTCFLAGS)

CXXFLAGS="$CXXFLAGS $ROOTCFLAGS"

dnl test for root 6
if test `root-config --version | awk '{print $1=6.?"1":"0"}'` = 1; then
   CINTDEFS=" -noIncludePaths  -inlineInputHeader "
   AC_SUBST(CINTDEFS)
   AC_DEFINE(HAVE_ROOT6)
fi
AM_CONDITIONAL([MAKEROOT6],[test `root-config --version | awk '{print $1>=6.?"1":"0"}'` = 1])

AC_CHECK_FILE( $OFFLINE_MAIN/include/FROG.h,have_frog=yes, have_frog=no)
AC_MSG_RESULT([$have_frog])

AM_CONDITIONAL(FROG, test "$have_frog" = yes )

if test $have_frog = yes; then
  AC_DEFINE(HAVE_FROG)
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
