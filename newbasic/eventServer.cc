

#define coutfl cout << __FILE__<< "  " << __LINE__ << " "
#define cerrfl cerr << __FILE__<< "  " << __LINE__ << " "


#include <iostream>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h> 

#ifdef HAVE_GETOPT_H
#include "getopt.h"
#endif

#include <map>


#include "testEventiterator.h"
#include "fileEventiterator.h"
#include "rcdaqEventiterator.h"
#include "oncsEventiterator.h"

using namespace std;

#define PORT	 8080 
#define MAXSIZE 5120

map<int, Event*> EventMap;
Eventiterator *it;


#define RCDAQEVENTITERATOR 1
#define FILEEVENTITERATOR 2
#define TESTEVENTITERATOR 3
#define ONCSEVENTITERATOR 4

void exitmsg()
{
  cout << "** usage: gl1_server -nifTrOvh datastream" << std::endl;
  cout << "    type  gl1_server -h   for more help" << std::endl;
  exit(0);
}

float requested = 0; 
float sent = 0; 

void exithelp()
{

  cout << std::endl;
  cout << "   gl1_server serves events from a given datastream" << std::endl;
  cout << std::endl;
  cout << "  List of options: " << std::endl;
  cout << " -d <number> depth of buffer" << std::endl;
  cout << " -f (stream is a file)" << std::endl;
  cout << " -T (stream is a test stream)" << std::endl;
  cout << " -r (stream is a rcdaq monitoring stream)" << std::endl;
  cout << " -O (stream is a legacy ONCS format file)" << std::endl;
  cout << " -h this message" << std::endl;
  cout << endl;
  cout << " debug options" << endl;
  cout << " -s <number> sleep so many ticks (in units of usleep)" << std::endl;
  cout << " -i <print event identity>" << std::endl;
  cout << " -v verbose" << std::endl;
  cout << " -c count  print -v -or -i messages only every <count> occurences " << std::endl;
  exit(0);
}

unsigned int depth = 1000;
int go_on = 1;
int identify = 0;
int verbose = 0;
int repeatcount =1;
int ittype = RCDAQEVENTITERATOR;
int sleeptime = 0;
int old_runnumber = -9999;
int msgfrequency =1;

pthread_mutex_t MapSem;



void * EventLoop( void *arg)
{

  if ( identify) it->identify();
  int current_count = 0;

  while ( go_on)
    {
      Event *e = it->getNextEvent();
      if ( ! e)
	{
	  go_on = 0;
	  return 0;
	  
	}
      e->convert();

      pthread_mutex_lock( &MapSem);
      map<int, Event*>::iterator it = EventMap.begin();

      // if we find that our run number has changed, we clear out what we have
      if ( old_runnumber != e->getRunNumber())
	{
	  old_runnumber = e->getRunNumber();
	  requested = 0;
	  sent = 0;
	  for ( ; it != EventMap.end(); ++it)
	    {
	      delete it->second;
	    }
	  EventMap.clear();
	}

      // if the next event inserted would exceed the envisioned depth, we remove the oldest 
      if (EventMap.size() >= depth)
	{
	  map<int, Event*>::iterator it = EventMap.begin();
	  if ( verbose ) coutfl << "erasing event " << it->first << " depth = " << EventMap.size() << endl;
	  delete it->second;
	  EventMap.erase(it);
	}

      // ok, so now we insert...
      EventMap[e->getEvtSequence()] = e;

      // and unlock the map
      pthread_mutex_unlock( &MapSem);

      current_count++;

      if ( current_count >= msgfrequency && identify )
	{
	  e->identify();
	  current_count = 0;
	}

      if (sleeptime) usleep(sleeptime);
    }
  return 0;
}


int 
main(int argc, char *argv[])
{ 
  int sockfd; 
  int  buffer[MAXSIZE];

  int c;


  int status = -1;

  pthread_mutex_init( &MapSem, 0);

  while ((c = getopt(argc, argv, "d:s:c:ifTrOvh")) != EOF)
    switch (c) 
      {
      case 'd':
	if ( !sscanf(optarg, "%d", &depth) ) exitmsg();
	break;

      case 's':
	if ( !sscanf(optarg, "%d", &sleeptime) ) exitmsg();
	break;

      case 'c':
	if ( !sscanf(optarg, "%d", &msgfrequency) ) exitmsg();
	break;

      case 'i':
	identify = 1;
	break;

      case 'T':
	ittype = TESTEVENTITERATOR;
	break;

      case 'f':
	ittype = FILEEVENTITERATOR;
	break;

      case 'r':
	ittype = RCDAQEVENTITERATOR;
	break;

      case 'O':
	ittype = ONCSEVENTITERATOR;
	break;

      case 'v':   // verbose
	verbose++;
	break;

      case 'h':
	exithelp();
	break;
      }

  switch (ittype)
    {
    case RCDAQEVENTITERATOR:
      if ( optind+1>argc) 
	{
	  std::string host = "localhost";
    
	  if ( getenv("RCDAQHOST")  )
	    {
	      host = getenv("RCDAQHOST");
	    }
	  
	  it = new rcdaqEventiterator(host.c_str(), status);

	}
      else
	{
	  it = new rcdaqEventiterator(argv[optind], status);
	}
      break;

    case  TESTEVENTITERATOR:
      it = new testEventiterator();
      status =0;
      break;

    case  FILEEVENTITERATOR:
      if ( optind+1>argc) exitmsg();
      it = new fileEventiterator(argv[optind], status);
      break;
     
    case  ONCSEVENTITERATOR:
      if ( optind+1>argc) exitmsg();
      it = new oncsEventiterator(argv[optind], status);
      break;

      status = 1;
      break;
      
    default:
      exitmsg();
      break;
    }

  if (status)
    {
      delete it;
      it = 0;
      cout << "Could not open input stream" << std::endl;
      exit(1);
    }


  pthread_t ThreadEvt;

  status = pthread_create(&ThreadEvt, NULL, 
			  EventLoop, 
			  (void *) 0);
   
  if (status ) 
    {
      cout << "error in event thread create " << status << endl;
      exit(0);
    }

  
  
  struct sockaddr_in servaddr, cliaddr; 
  
  // Creating socket file descriptor 
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    { 
      perror("socket creation failed"); 
      exit(EXIT_FAILURE); 
  } 
  
  memset(&servaddr, 0, sizeof(servaddr)); 
  memset(&cliaddr, 0, sizeof(cliaddr)); 
	
  // Filling server information 
  servaddr.sin_family = AF_INET; // IPv4 
  servaddr.sin_addr.s_addr = INADDR_ANY; 
  servaddr.sin_port = htons(PORT); 
  
  // Bind the socket with the server address 
  if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
	    sizeof(servaddr)) < 0 ) 
    { 
      perror("bind failed"); 
      exit(EXIT_FAILURE); 
    } 
	
  socklen_t len;
  int n; 
  int current_count = 0;

  len = sizeof(cliaddr); //len is value/result 

  int recbuffer[10];
  
  while (1)
    {
      n = recvfrom(sockfd, (char *)recbuffer, 2*sizeof(int), 
		   MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
		   &len);
      requested += 1;
      if ( verbose && ++current_count >= msgfrequency)
	{
	  cout << "request from " << inet_ntoa(cliaddr.sin_addr) << " requesting " << recbuffer[0];
	}
      
     pthread_mutex_lock( &MapSem);
     
     map<int, Event*>::iterator it = EventMap.find(recbuffer[0]);
     if ( it == EventMap.end() )
       {
	 pthread_mutex_unlock( &MapSem);

	 buffer[0] = 0;
	 sendto(sockfd, (const char *) buffer, sizeof(int), 
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
		len);
	 if ( verbose && current_count >= msgfrequency)
	   {
	     cout << " Event not delievered";
	     if ( requested > 0) cout << " " << 100 * sent/requested << "%" << endl;
	     else cout << endl;
	     current_count = 0;
	   }
	}
     else
       {
	 int nw;
	 (it->second)->Copy(buffer,MAXSIZE,&nw,"");
	 pthread_mutex_unlock( &MapSem);
	 sendto(sockfd, (const char *) buffer, nw*sizeof(int), 
		MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
		len); 
	 sent += 1;
	 if ( verbose && current_count >= msgfrequency )
	   {
	     cout << " Event sent";
	     if ( requested > 0) cout << " " << 100 * sent/requested << "%" << endl;
	     current_count = 0;
	   }
       }
    }
  return 0; 
}
