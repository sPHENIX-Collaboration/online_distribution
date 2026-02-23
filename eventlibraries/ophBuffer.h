#ifndef __OPHBUFFER_H
#define __OPHBUFFER_H

#include "event_io.h"

#include "phenixTypes.h"
#include "oBuffer.h"
#include "oEvent.h"
#include "Event.h"


class  ophBuffer : public oBuffer {

public:

  //** Constructors

  //  ophBuffer( FILE *fpp, PHDWORD * , const int length
  //	 , const int iseq = 1, const int irun=1);

  ophBuffer() {};

  ophBuffer (int fd, PHDWORD * where, const int length,
		const int irun=1 , const int iseq=0 );
  
  ophBuffer (const char *filename, PHDWORD * where, const int length,
	   int &status,
	   const int irun=1 , const int iseq=0 );


  ~ophBuffer();

  //  this creates a new event on the next address
  int nextEvent( const unsigned int evtsize, const int etype =0, const int evtseq =0);


  // frame and packet adding

  int addRawEvent(unsigned int *);

  int addEvent(Event *);

  int addFrame(PHDWORD *);

  int  addPacket( const Packet *p);

  int addUnstructPacketData(PHDWORD * data, 
		    const int length,
		    const int id,
		    const int wordsize,
		    const int hitformat);

  int writeout ();

  // now the re-sizing of buffer
  int setMaxSize( const int size);

  // and the query
  int getMaxSize() const;

  // and the query`
  unsigned long long getBytesWritten() const;

  int addEoB();


protected:

  int prepare_next();


  buffer_ptr bptr;
  buffer_ptr bptr_being_written;

  buffer_ptr bptr0;
  buffer_ptr bptr1;
  PHDWORD *data_ptr;
  int current_index;
  int max_length;
  int max_size;
  unsigned int left;
  int sequence;
  int runnumber;
  unsigned long long byteswritten;
  oEvent *current_event;
  int eventsequence;
  int current_etype;
  int has_end;
  int dirty;
  int fd;
  int our_fd;
  int good_object;


};

#endif

