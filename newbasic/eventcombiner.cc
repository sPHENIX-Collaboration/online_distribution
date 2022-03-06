#include "fileEventiterator.h"
#include "testEventiterator.h"
#include "phenixTypes.h"
#include "A_Event.h"
#include "ogzBuffer.h"
#include "ophBuffer.h"
#include "ospBuffer.h"
#include "EventTypes.h"
#include "oEvent.h"

#include <cstdlib>
#include <unistd.h>
#include <cstdio>

#include <string>
#include <vector>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#define DDEVENTITERATOR 1
#define FILEEVENTITERATOR 2
#define TESTEVENTITERATOR 3
#define DDPOOL 4
#define DFILE 5
#define DNULL 6

#if defined(SunOS) || defined(Linux) || defined(OSF1)
#include <cstring>
#else
#include <bstring.h>
#endif



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#if defined(SunOS) || defined(Linux) || defined(OSF1)
void sig_handler(int);
#else
void sig_handler(...);
#endif


void exitmsg()
{
  COUT << "** usage: eventcombiner outputfile inputfile1 inputfile2 ..." << std::endl;
  COUT << "          eventcombiner -h for more help" << std::endl;
  exit(0);
}

void exithelp()
{
  COUT <<  std::endl;
  COUT << " Syntax:"<< std::endl;
  COUT <<  std::endl;
  COUT << "      eventcombiner [-v] [-i] [-n number] [-u] [-p] [-h]  inputfile1 inputfile2 ..."<< std::endl;
  COUT << " e.g  eventcombiner -v /export/rcfdata/dcm_data/built_evt/rc_3612.prdfz /export/rcfdata/dcm_data/rc/*3612*" << std::endl;
  COUT << " will combine all the *3612* (from one run number) together in the file. "<< std::endl;
  COUT << " Options:" << std::endl;
  COUT << "  -v   verbose, without that it does its work silently" << std::endl;
  COUT << "  -i   identify the events as they are processed, good for debugging" << std::endl;
  COUT << "  -e <event number> start with event number (number in evt. header)" << std::endl;
  COUT << "  -c <event number> start with nth event" << std::endl;
  COUT << "  -n <number>   stop after so many events" << std::endl;
  COUT << "  -p  read and write legacy PHENIX format data, default is sPHENIX "<< std::endl;
  //  COUT << "  -u  write uncompressed data, default is compressed "<< std::endl;
  COUT << "  -f  force output file overwrite, normally you cannot overwrite an existing file (safety belt)"<< std::endl;
  COUT << "  -x  ignore event numbers (allow non-matching evt nrs to be combined, DANGEROUS)"<< std::endl;
  COUT << "  -h  this message" << std::endl;
  exit(0);
}

void evtcountexitmsg()
{
  COUT << "** cannot specify both -e and -c!" << std::endl;
  COUT << "    type  eventcombiner -h  for more help" << std::endl;
  exit(0);
}

oBuffer *ob;
int fd;
int file_open = 0;

int 
main(int argc, char *argv[])
{
  int c;
  int status;

  int eventnumber =0;
  int countnumber =0;
  int forceflag =0;
  int verbose = 0;
  int identify = 0;
  int maxevents = 0;
  int eventnr = 0;
  int ignoreeventnr = 0;
  //  int gzipcompress = 0;
  int legacyphenix = 0;
  
  extern char *optarg;
  extern int optind;

  PHDWORD  *buffer;

  // initialize the pointers to 0;
  fd = 0;
  ob = 0;

  //  COUT << "parsing input" << std::endl;
  int buffer_size = 256*1024*64;  // makes 32MB (specifies how many dwords, so *4)



  while ((c = getopt(argc, argv, "n:c:e:vipfhx")) != EOF)
    {

      switch (c) 
	{

	case 'v':   // verbose
	  verbose = 1;
	  break;

	case 'i':   // identify
	  identify = 1;
	  break;

	case 'f':   // force
	  forceflag = 1;
	  break;

	// case 'g':   // do not gzip-compress
	//   gzipcompress = 1;
	//   break;

	case 'p':   // do not gzip-compress
	  legacyphenix = 1;
	  break;

	case 'x':   // do not gzip-compress
	  ignoreeventnr = 1;
	  break;

	case 'e':
	  if ( !sscanf(optarg, "%d", &eventnumber) ) exitmsg();
	  break;
	  
	case 'c':
	  if ( !sscanf(optarg, "%d", &countnumber) ) exitmsg();
	  break;

	case 'n':   // number of events
	  if ( !sscanf(optarg, "%d", &maxevents) ) exitmsg();
	  break;

	case 'h':
	  exithelp();
	  break;

	default:
	  break;
						
	}
    }

  if (argc < 3) exitmsg();

  if ( eventnumber && countnumber) evtcountexitmsg();

  vector<Eventiterator *> it;
  vector<Event *>evt;

  int index;
  
  string  filename;

  filename =argv[optind];

  // try if the output file exists
  
  fd =  open(filename.c_str(), O_RDONLY | O_LARGEFILE);
  if (fd > 0 ) 
    {
      if ( ! forceflag) 
	{
	  COUT << "file " << filename << " exists - I won't override it" << std::endl;
	  COUT << " use -f to force" << std::endl;
	  exit(1);
	}
      close (fd);
    }
  for ( index =  optind+1; index < argc; index++ ) 
    {
      COUT << "reading from file " << argv[index] << std::endl;
      Eventiterator *itx  = new fileEventiterator(argv[index], status);
      if (status) 
	{ 
	  COUT << "could not open " <<  argv[index] << std::endl;
	  exit(1);
	}
      it.push_back(itx);

    }

    buffer = new PHDWORD [buffer_size];


    int go_on = 1;

    unlink (filename.c_str());
    
    fd = open(filename.c_str(),
	      O_WRONLY | O_CREAT | O_EXCL | O_LARGEFILE , 
	      S_IRWXU | S_IROTH | S_IRGRP );


    if ( fd < 0 ) 
      {
	COUT << "Could not open file: " <<  filename << std::endl;
	exit (1);
      }
    if (verbose) COUT << "Opened file: " <<  filename << std::endl;
    
    if (legacyphenix) 
      {
	ob = new ophBuffer (fd, buffer, buffer_size);
      }
    else
      {
	ob = new ospBuffer (fd, buffer, buffer_size);
      }
    
    int count = 0;

    while ( ( maxevents == 0 || eventnr < maxevents)  && go_on)
      {
	int total_length = 0;
	int enr = 0;

	vector<Eventiterator *>::iterator it_itr;
	Event *e  = 0;
	
 	for (it_itr = it.begin(); it_itr != it.end(); ++it_itr)
	  {
	    e  = (*it_itr)->getNextEvent();
	    if ( !e )
	      {
		go_on = 0;
	      }
	    else
	      {
		total_length += e->getEvtLength();
	      }
	    evt.push_back(e);
	  }
	if (! go_on ) break;

	int *out = new int[total_length];

	int nwout;
	int current = 0;
	int take_this = 1;
	
	if ( eventnumber && e->getEvtSequence() < eventnumber)  take_this = 0;

	if ( countnumber && count+1 < countnumber)  take_this = 0;
	
	vector<Event *>::iterator evt_itr;
	for (evt_itr = evt.begin(); evt_itr != evt.end(); ++evt_itr)
	  {
	    if (evt_itr == evt.begin() )  // event from 1st input file 
	      {	
		enr = (*evt_itr)->getEvtSequence();
		(*evt_itr)->Copy ( out , total_length , &nwout);
		current  = nwout;
		delete (*evt_itr);
	      }
	    else
	      {
		if (take_this ==0  || (ignoreeventnr ==0 && (*evt_itr)->getEvtSequence() != enr ))
		  {
		    take_this = 0;
		  }
		else
		  {
		     (*evt_itr)->Copy (  &out[current] , total_length-current , &nwout, "DATA");
		    current += nwout;
		    out[0] +=  nwout;
		    delete  (*evt_itr);
		  }
	      }
	  }
    
	if (take_this) 
	  {
	    Event *E = new oncsEvent(out);
	    if (identify) E->identify();
	    
	    ob->addEvent(E);
	    delete E;
	    eventnr++;
	  }
	count++;
	delete [] out;
	evt.clear();
	
      }
    vector<Eventiterator *>::iterator it_itr;
    for (it_itr = it.begin(); it_itr != it.end(); ++it_itr)
      {
	delete ( (*it_itr) ) ;
      }
    
    delete ob;
    close (fd);
}
