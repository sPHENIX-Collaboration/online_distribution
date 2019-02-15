AUTOMAKE_OPTIONS = foreign

PACKAGE = pmonitor

include_HEADERS = \
  pmonitor.h \
  pmonstate.h \
  pmondisplay.h \
  pmongui.h \
  pMutex.h

LINKFILE = pmonitorLinkDef.h

pkginclude_HEADERS = $(include_HEADERS)

noinst_HEADERS = $(LINKFILE)

#BUILT_SOURCES = pmonitor_dict.C

#ROOTCINT =  $(ROOTSYS)/bin/rootcint 
ROOTCINT =  rootcint 

AM_CPPFLAGS =  -I$(includedir) -I$(ONLINE_MAIN)/include -I@ROOTINC@

lib_LTLIBRARIES = libpmonitor.la

LDFLAGS = -Wl,--no-as-needed 

if ! MAKEROOT6
  ROOT5_DICTS = \
    pmonitor_dict.C
endif

libpmonitor_la_SOURCES = \
  $(ROOT5_DICTS) \
  pmonitor.cc \
  pmongui.cc \
  pmonitor.h \
  pmonstate.h \
  pmondisplay.h \
  pmongui.h \
  pMutex.cc


if FROG

libpmonitor_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lEvent -lmessage \
  $(ROOTLIBS) -lThread -lpthread \
  -L$(OFFLINE_MAIN)/lib 

else


libpmonitor_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -lEvent -lmessage \
  $(ROOTLIBS) -lThread -lpthread 

endif


pmonitor_dict.C: pmonitor.h pmonstate.h pmondisplay.h pmongui.h $(LINKFILE)
	$(ROOTCINT) -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

bin_SCRIPTS = writePmonProject.pl

clean-local:
	rm *_dict.*
	rm -f *.pcm
