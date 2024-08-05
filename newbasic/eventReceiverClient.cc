#include "eventReceiverClient.h"

#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <stdio.h> 
#include <iostream>
#include <iomanip>

#include "oncsEvent.h"

using namespace std;

#define PORT	 8080 
#define MAXSIZE 1024*1024

eventReceiverClient::eventReceiverClient( const std::string hostname, const int flags, const int port)
{

  _hostname = hostname;
  _port = port;
  _flags = flags;
  _broken = 0;
  _had_timeout = 0;
  _verbosity = 0;

  _timeout = 300000;  // 300ms default timeout
  _user_timeout = 0;  // 300ms default timeout
 
  //int  buffer[MAXSIZE];

  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  
  hints.ai_family = AF_INET;
  struct addrinfo *result;

  char port_str[512];
  sprintf(port_str, "%d", _port);

  int status = getaddrinfo(_hostname.c_str(), port_str,
			   &hints,
			   &result);


  if ( status < 0)
    {
      cerr  << _hostname << ": " << gai_strerror(status) << endl;
      _broken =1;
      return;
    }

  struct sockaddr_in* ipv4;
  ipv4 = ( struct sockaddr_in*)result->ai_addr;
      

  // for (rp = result; rp != NULL; rp = rp->ai_next)
  //   {
  //     //      char ip_str[INET_ADDRSTRLEN];
  //     //inet_ntop(AF_INET, &ipv4->sin_addr, ip_str, INET_ADDRSTRLEN);
  //     //cout << __FILE__<< " " << __LINE__ << " " << _hostname << " " << ip_str << endl;
      
  //   }

  freeaddrinfo(result);
  
  // Creating socket file descriptor 
  if ( (_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
    { 
      perror("socket creation failed"); 
      _broken = 2;
    } 

  memcpy(&_serveraddr, ipv4, sizeof(*ipv4)); 

  // if (inet_aton(_hostname.c_str(), &_serveraddr.sin_addr)==0)
  //   {
  //     cerr << "invalid IP address " << _hostname << endl;
  //     _broken =3;
  //   }

  _serveraddr.sin_family = AF_INET; 
  _serveraddr.sin_port = htons(_port); 

}

eventReceiverClient::~eventReceiverClient()
{
  if ( _sockfd > 0) close(_sockfd);
}

Event *eventReceiverClient::getEvent(const int eventnumber, const int flag )
{
  
  int sendbuffer[2] = {0};
  int buffer[MAXSIZE];
  
  if (_verbosity) cout << "requesting event " << eventnumber << endl;

  int f= flag;
  if (!f) f = _flags;
  sendbuffer[0] = eventnumber;
  sendbuffer[1] = f;
  
  sendto(_sockfd, (const char *)sendbuffer, 2*sizeof(int), 
	 0, (const struct sockaddr *) &_serveraddr, 
	 sizeof(_serveraddr)); 


  struct timeval tv;
  fd_set myset;

  if ( _user_timeout )
    {
      tv.tv_sec = _user_timeout;
      tv.tv_usec = 0;
    }
  else
    {
      tv.tv_sec = 0;
      tv.tv_usec = _timeout;
    }

  FD_ZERO(&myset);
  FD_SET(_sockfd, &myset);

  
  //coutfl << "waiting for data, highest_fd =  " << _sockfd << endl;
  int retval = select(_sockfd+1, &myset, 0, 0, &tv);

  if (retval <=0)
    {
      if (_verbosity) cout << "timeout receiving event " << eventnumber << endl;
      _had_timeout = 1;
      return 0;
    }
  else
    {

      socklen_t len; 
      int n = recvfrom(_sockfd, (char *)buffer, MAXSIZE*sizeof(int), 
		       MSG_WAITALL, (struct sockaddr *) &_serveraddr, 
		       &len);
      _had_timeout = 0;
  
      if (_verbosity) cout << " received " << n << " bytes" << std::endl;
      if ( buffer[0] == 0)
	{
	  if (_verbosity) std::cout << "Event " << sendbuffer[0] << " not found"  << endl;
	  return NULL;
	}
      
      if ( n < 4*buffer[0] ) // idiot check - we have received fewer data than the event says it has
	{
	  if (_verbosity) std::cout << " corrupt Event, need " << sendbuffer[0]*4  << " bytes but got " << n << endl;
	  return NULL;
	}	  

      Event *e = new oncsEvent(buffer);
      e->convert();
      if (_verbosity) e->identify();
      return e;
    }
  return 0;
}
