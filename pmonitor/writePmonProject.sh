#!/bin/bash
#
# writePmonProject.sh - create a new pmonitor project
# (replaces writePmonProject.pl, same output)
#
#   writePmonProject.sh <projectname>
#
# creates in the current directory
#   <projectname>.cc         pinit() and process_event()
#   <projectname>.h
#   <projectname>LinkDef.h
#   <projectname>.Makefile   (and a Makefile link to it, if there is no Makefile yet)
#   <projectname>.C          ROOT macro that loads the library and opens a file
#   <projectname>.sh         starts root with that macro

if [ $# -lt 1 ]
then
    echo "Usage: writePmonProject.sh <projectname>"
    echo "   e.g writePmonProject.sh MyAnalysis "
    exit 1
fi

projectname=$1
PROJECTNAME=$(echo "$projectname" | tr '[:lower:]' '[:upper:]')

echo "creating project $projectname"

cat > "$projectname.cc" <<EOF

#include <iostream>
#include <pmonitor/pmonitor.h>
#include "$projectname.h"

#include <TH1.h>
#include <TH2.h>

int init_done = 0;

using namespace std;

//TH1F *h1;
//TH2F *h2;


int pinit()
{

  if (init_done) return 1;
  init_done = 1;

  // h1 = new TH1F ( "h1","test histogram", 400, -50, 50);
  // h2 = new TH2F ( "h2","test histogram 2D", 100, -50.5, 49.5, 100, -500, 500);

  return 0;

}

int process_event (Event * e)
{

  Packet *p = e->getPacket(1003);
  if (p)
    {

      //  h1->Fill ( p->iValue(3)/1000. );
      //  h2->Fill ( p->iValue(0), p->iValue(1) );

      delete p;

    }
  return 0;
}

EOF

cat > "$projectname.h" <<EOF
#ifndef __${PROJECTNAME}_H__
#define __${PROJECTNAME}_H__

#include <pmonitor/pmonitor.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>

int process_event (Event *e); //++CINT

#endif /* __${PROJECTNAME}_H__ */
EOF

# the Makefile's own $(...) must reach the file unexpanded, hence \$
cat > "$projectname.Makefile" <<EOF
PACKAGE = $projectname

ROOTFLAGS = \$(shell root-config --cflags)
ROOTLIBS = \$(shell root-config --glibs)


CXXFLAGS = -I.  \$(ROOTFLAGS) -I\$(ONLINE_MAIN)/include -I\$(OFFLINE_MAIN)/include
RCFLAGS = -I.  -I\$(ONLINE_MAIN)/include -I\$(OFFLINE_MAIN)/include

LDFLAGS = -Wl,--no-as-needed  -L\$(ONLINE_MAIN)/lib -L\$(OFFLINE_MAIN)/lib -lpmonitor -lEvent -lNoRootEvent -lmessage  \$(ROOTLIBS) -fPIC



HDRFILES = \$(PACKAGE).h
LINKFILE = \$(PACKAGE)LinkDef.h


ADDITIONAL_SOURCES =
ADDITIONAL_LIBS =


SO = lib\$(PACKAGE).so

\$(SO) : \$(PACKAGE).cc \$(PACKAGE)_dict.C \$(ADDITIONAL_SOURCES) \$(LINKFILE)
	\$(CXX) \$(CXXFLAGS) -o \$@ -shared  \$<  \$(ADDITIONAL_SOURCES) \$(PACKAGE)_dict.C \$(LDFLAGS)  \$(ADDITIONAL_LIBS)


\$(PACKAGE)_dict.C : \$(HDRFILES) \$(LINKFILE)
	rootcint -f \$@  -c \$(RCFLAGS) \$^


.PHONY: clean

clean:
	rm -f \$(SO) \$(PACKAGE)_dict.C \$(PACKAGE)_dict.h *.pcm

EOF

if [ ! -e Makefile ]
then
    ln -s "$projectname.Makefile" Makefile
fi

cat > "${projectname}LinkDef.h" <<EOF
#ifdef __CINT__

#pragma link C++ defined_in "$projectname.h";

#endif /* __CINT__ */
EOF

cat > "$projectname.C" <<EOF
#include "$projectname.h"
R__LOAD_LIBRARY(lib$projectname.so)

void $projectname(const char * filename)
{
  if ( filename != NULL)
    {
      pfileopen(filename);
    }
}
EOF

cat > "$projectname.sh" <<EOF
#! /bin/bash

FILE="\$1"

if [ -z "\$FILE" ] ; then

    root --web=off -l $projectname.C\\(0\\)
else

    root --web=off -l $projectname.C\\(\\"\$FILE\\"\\)
fi
EOF
