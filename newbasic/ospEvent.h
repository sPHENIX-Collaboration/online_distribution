#ifndef __OSPEVENT_H
#define __OSPEVENT_H


#include "phenixTypes.h"

#include "oncsSubConstants.h"
#include "oncsEvtStructures.h"
#include "oncsSubevent.h"


class  ospEvent {

public:

  //** Constructors
  ospEvent(PHDWORD *, const int maxlength
	 , const int irun, const int itype, const int eseq);

  virtual ~ospEvent() {}
  virtual int prepare_next();
  virtual int prepare_next( const int, const int);
  virtual void set_event_type(const int);

  virtual int addPacket ( const Packet * p);

  virtual int addUnstructPacketData(PHDWORD * data, 
		    const int length,
		    const int id,
		    const int wordsize,
		    const int hitformat);
protected:
  
  PHDWORD *event_base;
  int current;
  int in_frame;
  int max_length;
  int left;
  oncsevtdata_ptr evthdr;
};

#endif
