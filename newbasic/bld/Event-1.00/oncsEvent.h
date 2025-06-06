#ifndef __ONCS_EVENT_H
#define __ONCS_EVENT_H

#include "Event.h"
#include "phenixTypes.h"
#include "EvtConstants.h"
#include "oncsEvtStructures.h"
#include <map>

#ifndef __CINT__
class WINDOWSEXPORT oncsEvent : public Event {
#else
class  oncsEvent : public Event {
#endif

public:
  // constructors and destructors
  oncsEvent(int *);
  ~oncsEvent();

  unsigned int getEvtLength() const;
  int getEvtType() const;
  int getEvtSequence() const;
  int getRunNumber() const;
  //PHTimeStamp * getTimeStamp() const;
  
  void identify(std::ostream& os = std::cout) const;


  Packet* getPacket(const int);
  Packet* getPacket(const int, const int hitFormat);

  int getPacketList(Packet*[], const int);
  std::vector<Packet *> getPacketVector();

  int existPacket (const int packetid);

  int Copy ( int *destination, const unsigned int length, int *nw, const char *what="");

  int is_pointer_type() const;
  int convert ();

  int getDate() { return 0;};
  time_t getTime() const;
  Packet * makePacket(PHDWORD *pp, const int hitFormat = 0);

  void setOriginBuffer(const int n) {originBuffer =n;};
  int getOriginBuffer() const {return originBuffer;};


protected:
  int is_data_type;  // 0 is pointer based --  1 is data based

  oncsevtdata_ptr EventData;

  int hasMap;
  int errorcode;
  virtual int createMap();
  std::map <int, PHDWORD *> pmap;

  int originBuffer;
};

#endif
