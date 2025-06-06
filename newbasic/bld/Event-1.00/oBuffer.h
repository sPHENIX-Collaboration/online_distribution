#ifndef __OBUFFER_H
#define __OBUFFER_H

#include "phenixTypes.h"
#include "Event.h"


class  oBuffer {

public:


  oBuffer();

  /*  oBuffer (int fd, PHDWORD * where, const int length, */
  /* 		const int irun=1 , const int iseq=0 ); */
  

  /* oBuffer (const char *filename, PHDWORD * where, const int length, */
  /* 	   int &status, */
  /* 	   const int irun=1 , const int iseq=0 ); */


  virtual  ~oBuffer();

  //  this creates a new event on the next address
  virtual int nextEvent( const unsigned int evtsize, const int etype =0, const int evtseq =0) = 0;


  // frame and packet adding

  virtual int addRawEvent(unsigned int *) =0;

  virtual int addEvent(Event *) = 0;

  // we have this here since not all subclasses have this
  virtual int addFrame(PHDWORD *) {return 0;}; 

  virtual int  addPacket( const Packet *p) = 0;

  virtual int addUnstructPacketData(PHDWORD * data, 
		    const int length,
		    const int id,
		    const int wordsize,
		    const int hitformat) = 0;

  virtual int writeout () = 0;

  // now the re-sizing of buffer
  virtual int setMaxSize( const int size) = 0;

  virtual int getMaxSize() const =0 ;

  virtual unsigned long long getBytesWritten() const = 0;

  virtual int addEoB() = 0;


protected:

  //  virtual int prepare_next();

  typedef struct 
  { 
    unsigned int Length;
    int ID;
    int Bufseq;
    int Runnr;
    PHDWORD data[1];
  } *buffer_ptr;


};

#endif

