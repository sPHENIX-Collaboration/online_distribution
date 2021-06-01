AC_INIT(Event,[1.00])
AC_CONFIG_SRCDIR([configure.ac])


AM_CONDITIONAL(VXWORKS, false)
AM_CONDITIONAL(LINUX, false)
AM_CONDITIONAL(NEWIO, false)

AC_CANONICAL_HOST

dnl search for C++ compiler and set default flags
AC_PROG_CXX(cxx CC g++)

case "$host" in
  *-*-linux*)
    CPPFLAGS="$CPPFLAGS -DLinux"
    LT_INIT([disable-static])
    AM_CONDITIONAL(LINUX, true)
  ;;
  *-*-osf*)
    CPPFLAGS="$CPPFLAGS -DOSF1 -D_BSD"
    LIBS="-lm"
    LT_INIT([disable-static])
  ;;
  *-*-solaris2.6)
  ;;
  *-*-solaris2.8)
    CPPFLAGS="$CPPFLAGS -DSunOS"
dnl Chris: default flags with optimizer screw shared lib
dnl  CXXFLAGS="+p -mt -g -xildoff"
    CXXFLAGS="$CXXFLAGS +p -mt -fast -xO5"
    LIBS="-lm -lc -lsocket -lrt -lnsl -lresolv"
    LT_INIT([disable-shared])
  ;;
  powerpc-wrs-vxworks)
    AM_CONDITIONAL(VXWORKS, true)
  ;;
  *-*-irix5*)
    CPPFLAGS="-DIRIX"
  ;;
  *-*-irix6*)
    CPPFLAGS="-DIRIX64"
  ;;
  *-*-cyg*)
    CPPFLAGS=" -DNT"
  ;;
  *)
    AC_MSG_ERROR([default compiler unknown for this system!])
  ;;
esac

AM_INIT_AUTOMAKE(Event,1.00)

AC_PROG_INSTALL

dnl   no point in suppressing warnings people should 
dnl   at least see them, so here we go for g++: -Wall
if test $ac_cv_prog_gxx = yes; then
  CXXFLAGS="$CXXFLAGS -Wall"
fi

AC_HEADER_STDC
AC_CHECK_HEADERS(getopt.h)

AC_CHECK_FUNCS(setenv)

ROOTGLIBS=`root-config --glibs`
ROOTINC=`root-config --incdir`
AC_SUBST(ROOTGLIBS)
AC_SUBST(ROOTINC)
dnl test for root 6
if test `root-config --version | awk '{print $1>=6.?"1":"0"}'` = 1; then
   CINTDEFS=" -noIncludePaths  -inlineInputHeader "
   AC_SUBST(CINTDEFS)
   AC_DEFINE(HAVE_ROOT6)
fi
AM_CONDITIONAL([MAKEROOT6],[test `root-config --version | awk '{print $1>=6.?"1":"0"}'` = 1])

LZOLIB="-llzo2"
AC_SUBST(LZOLIB)


dnl sorry - we want to find out if the prototype is in the header.
dnl We cannot use AC_CHECK_FUNCS(strptime) because it always returns true
dnl so we try to compile the two lines and see what we get. 
dnl With gcc-2.95++ we should get an error and can define the
dnl prototypes ourselves. 

AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING([for strptime])
AC_TRY_COMPILE([#include <time.h>],[
   tm newTime;
   strptime("12-04-2000", "%A %h %d %H:%M:%S %Y", &newTime);],
have_strptime_prototype=yes, have_strptime_prototype=no)
AC_MSG_RESULT([$have_strptime_prototype])
if test $have_strptime_prototype = yes; then
	AC_DEFINE(HAVE_STRPTIME_PROTOTYPE)
fi
AC_LANG_RESTORE

dnl for the message library, we must use a custom check.
dnl there are two distict types of systems, one where 
dnl a new streambuf is given to cout as a parameter, 
dnl ( cout.rdbuf(x) ), the other by assignment (cout = x)

AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING([whether cout accepts a streambuf arg])
AC_TRY_COMPILE([#include <iostream>],[
  std::streambuf *x = std::cout.rdbuf();
  std::cout.rdbuf(x);],
rdbuf_accepts_streambuf=yes, rdbuf_accepts_streambuf=no)
AC_MSG_RESULT([$rdbuf_accepts_streambuf])
if test $rdbuf_accepts_streambuf = yes; then
	AC_DEFINE(RDBUF_ACCEPTS_STREAMBUF)
fi
AC_LANG_RESTORE

AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING([whether we have a new iostream])
AC_TRY_COMPILE([#include <iostream>],[
  std::streambuf *x = std::cout.rdbuf();
  x->pubsync();],
new_iostream=yes, new_iostream=no)
AC_MSG_RESULT([$new_iostream])
if test $new_iostream = yes; then
	AM_CONDITIONAL(NEWIO, true)
fi
AC_LANG_RESTORE

AC_ARG_ENABLE(demos, 
	[  --enable-demos          build demos [default=yes]],
	[case "${enableval}" in
		yes) demos=true ;;
		no)  demos=false ;;
		*) AC_MSG_ERROR(bad value ${enableval} for --enable-demos) ;;
		esac], 
	demos=true)
AM_CONDITIONAL(DEMOS, test "$demos" = true)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
