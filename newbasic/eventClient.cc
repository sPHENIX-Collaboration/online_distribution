#include <string.h> 
#include <stdio.h> 
#include <iostream>
#include <iomanip>

#include "eventReceiverClient.h"
#include <Event/Event.h>

#ifdef HAVE_GETOPT_H
#include "getopt.h"
#endif

using namespace std;

// Driver code 
int 
main(int argc, char *argv[])
{ 


  if ( argc < 2)
    {
      cout << " usage: " << argv[0] << "  event_nr [hostname] " << endl;
      return 1;
    }
  int evtnr = atoi(argv[1]);
  string host;
  if ( argc >= 3)
    {
      host = argv[2];
    }
  else
    {
      host = "localhost";
    }
  
  eventReceiverClient *erc = new eventReceiverClient(host);
  if ( erc->getStatus())
    {
      delete erc;
      return 1;
    }

  Event *e = erc->getEvent(evtnr);
  if (e)
    {
      e->identify();
      delete e;
    }
  else
    {
      cout << "event " << evtnr << " not received" << endl;
    }
  delete erc;
      
  return 0; 
}
