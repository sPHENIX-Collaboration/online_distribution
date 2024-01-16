#ifndef  __EVENTRECEIVERCLIENT_H__
#define  __EVENTRECEIVERCLIENT_H__


#include <string>
#include <netinet/in.h>


class Event;

class eventReceiverClient
{
 public:
  eventReceiverClient( const std::string hostname, const int flags=0);
  ~eventReceiverClient();

  Event *getEvent(const int eventnumber, const int flag = 0);
  void setVerbosity(const int v) { _verbosity = v;};
  int getVerbosity() const { return _verbosity;};
  int getStatus() const { return _broken;};
  
 protected:

  int _sockfd;
  int _flags;
  int _broken;
  int _verbosity;
  
  std::string _hostname;
  struct sockaddr_in _serveraddr; 

  struct hostent *_phost;


  
};

#endif
