AUTOMAKE_OPTIONS = foreign

PACKAGE = pmonitor

include_HEADERS = \
  pmonitor.h \
  pmonstate.h \
  pMutex.h


LINKFILE = pmonitorLinkDef.h

pkginclude_HEADERS = $(include_HEADERS)

noinst_HEADERS = $(LINKFILE)

#BUILT_SOURCES = pmonitor_Dict.C

#ROOTCINT =  $(ROOTSYS)/bin/rootcint 
ROOTCINT =  rootcint 

AM_CPPFLAGS =  -I$(includedir)  @ROOTCFLAGS@

lib_LTLIBRARIES = libpmonitor.la

LDFLAGS = -Wl,--no-as-needed 

if ! MAKEROOT6
  ROOT5_DICTS = \
    pmonitor_Dict.C
endif

libpmonitor_la_SOURCES = \
  $(ROOT5_DICTS) \
  pmonitor.cc \
  pmonitor.h \
  pmonstate.h \
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

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
   pmonitor_Dict_rdict.pcm

pmonitor_Dict.C: pmonitor.h pmonstate.h  $(LINKFILE)
	$(ROOTCINT) -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) -I$(includedir) $^

bin_SCRIPTS = writePmonProject.pl


# get the dependency for the pcm files
%_Dict_rdict.pcm: %_Dict.C ;


# clean cache dir on Solaris 5.8
clean-local:
	rm -rf SunWS_cache
	rm -f *Dict* $(BUILT_SOURCES) *.pcm

