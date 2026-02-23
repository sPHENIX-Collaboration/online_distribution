#include <string.h> 
#include <stdio.h> 
#include <iostream>
#include <iomanip>

#include "eventReceiverClient.h"
#include "Event.h"

#ifdef HAVE_GETOPT_H
#include "getopt.h"
#endif

using namespace std;


void exitmsg()
{
  cout << " usage:   eventClient [-t <timeout in s> -v ]   event_nr [hostname] " << endl;
  exit(1);
}


void exithelp()
{

  cout << std::endl;
  cout << "eventClient receives events from the eventServer process" << std::endl;
  cout << "  usage:   eventClient [-t <timeout in s> -v ]   event_nr [hostname] " << endl;
  cout << std::endl;
  cout << "  List of options: " << std::endl;
  cout << " -p <port number>  (default 8080)" << std::endl;
  cout << " -t <s> timeout" << std::endl;
  cout << " -v verbose" << std::endl;
  exit(0);
}



// Driver code 
int 
main(int argc, char *argv[])
{ 
  int ThePort = 8080;

  int verbosity = 0;
  int timeout = 0;

  int c;

  while ((c = getopt(argc, argv, "t:p:v")) != EOF)
    switch (c) 
      {
      case 't':
	if ( !sscanf(optarg, "%d", &timeout) ) exitmsg();
	break;

      case 'p':
	if ( !sscanf(optarg, "%d", &ThePort) ) exitmsg();
	break;

      case 'v':   // verbose
	verbosity++;
	break;

      case 'h':
	exithelp();
	break;
      }

  if ( optind  >= argc) exitmsg(); 
 

  int evtnr = atoi(argv[optind]);

  string host;
  if ( argc > optind+1)
    {
      host = argv[optind+1];
    }
  else
    {
      host = "localhost";
    }
  
  eventReceiverClient  *erc = new eventReceiverClient(host,0,ThePort);
  if ( erc->getStatus())
    {
      delete erc;
      return 1;
    }

  if (timeout) erc->setUserTimeout(timeout);

  erc->setVerbosity(verbosity);

  Event *e = erc->getEvent(evtnr);
  if (e)
    {
      e->identify();
      delete e;
    }
  else
    {
      cout << "event " << evtnr << " not received, timeout = "<< erc->hadTimeout() << endl;
    }
  delete erc;
      
  return 0; 
}
