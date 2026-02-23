#ifndef __OSPBUFFER_H__
#define __OSPBUFFER_H__

#include "oBuffer.h"
#include "ospEvent.h"


class  ospBuffer : public oBuffer {

public:

  //** Constructors

  ospBuffer() {};
  
  ospBuffer (const char *filename, PHDWORD * where,
	     const int length,
	     int &status,
	     const int irun,
	     const int iseq);
  
  ospBuffer (int fdin, PHDWORD * where, 
	     const int length,
	     const int irun=1, 
	     const int iseq=0 );

   ~ospBuffer();

  int nextEvent( const unsigned int evtsize, const int etype, const int evtseq);

  int addRawEvent(unsigned int *);

  int addEvent(Event *);

  int addFrame(PHDWORD *) { return 0;};

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

  int _broken;
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
  ospEvent *current_event;
  int eventsequence;
  int current_etype;
  int has_end;
  int dirty;
  int fd;
  int our_fd;
  int good_object;


};



#endif /* __OSPBUFFER_H__ */

