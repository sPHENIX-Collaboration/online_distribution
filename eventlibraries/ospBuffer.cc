
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ospBuffer.h"
#include "ospEvent.h"

#include "BufferConstants.h"
#include "EventTypes.h"


// the constructor first ----------------


// the constructor first ----------------
ospBuffer::ospBuffer (const char *filename, PHDWORD * where, const int length, int &status
		, const int irun, const int iseq)
{
  status = 0;
  our_fd = 1;

  fd =  open(filename, O_WRONLY | O_CREAT | O_EXCL | O_LARGEFILE , 
		  S_IRWXU | S_IROTH | S_IRGRP );

  if ( fd < 0) 
    {
      status =1;
      good_object = 0;
      return;
    }
  good_object = 1;
  bptr = ( buffer_ptr) where;
  data_ptr = &(bptr->data[0]);
  max_length = length;
  max_size = max_length;
  left = max_size - BUFFERHEADERLENGTH;
  bptr->ID = -64;
  bptr->Bufseq = iseq;
  bptr->Runnr = 0;
  current_event = 0;
  current_etype = DATAEVENT;
  sequence = iseq;
  eventsequence = 0;
  runnumber = irun;
  byteswritten = 0;

  prepare_next ();
}

ospBuffer::ospBuffer (int fdin, PHDWORD * where, const int length
		, const int irun, const int iseq)
{
  fd = fdin;
  our_fd = 0;
  good_object = 1;
  bptr = ( buffer_ptr) where;
  data_ptr = &(bptr->data[0]);
  max_length = length;
  max_size = max_length;
  left = max_size - BUFFERHEADERLENGTH;
  bptr->ID = -64;
  bptr->Bufseq = iseq;
  bptr->Runnr = 0;
  current_event = 0;
  current_etype = DATAEVENT;
  sequence = iseq;
  eventsequence = 0;
  runnumber = irun;
  byteswritten = 0;

  prepare_next ();
}


// ---------------------------------------------------------
int ospBuffer::nextEvent( const unsigned int evtsize, const int etype, const int evtseq)
  
{

  if (current_event) delete current_event;
  current_event = 0;

  if (evtsize > max_size - EVTHEADERLENGTH) return -1;
  if (evtsize <=0) return -2;

  if (evtsize > left-EOBLENGTH)
    {
      writeout();
      prepare_next();
    }

  if (etype >0)   current_etype = etype;

  if (evtseq > 0)  eventsequence = evtseq;
  else eventsequence++;


  current_event = new ospEvent(&(bptr->data[current_index]), evtsize
			     ,bptr->Runnr, current_etype, eventsequence);

  left -= EVTHEADERLENGTH;
  current_index += EVTHEADERLENGTH;
  bptr->Length  += EVTHEADERLENGTH*4;
  bptr->Bufseq++;

  dirty = 1;
  return 0;
}




// ---------------------------------------------------------
int ospBuffer::addRawEvent( unsigned int *data)
{
  
  if ( ! good_object) return -1;
  int wstatus;

  unsigned int nw = data[0];

  if ( nw > left-EOBLENGTH)
    {
      wstatus = writeout();
      prepare_next();
      if (wstatus) return wstatus;
    }

  memcpy (  (char *) &(bptr->data[current_index]), (char *) data, 4*nw);
	
  left -= nw;
  current_index += nw;
  bptr->Length  += nw*4;
  bptr->Bufseq++;
  dirty =1;
  return 0; 
}


// ---------------------------------------------------------
int ospBuffer::addEvent( Event *Evt)
{
  
  if ( ! good_object) return -1;
  int nw;

  int wstatus;

  runnumber = Evt->getRunNumber();

  if ( Evt->getEvtLength() > left-EOBLENGTH)
    {
      wstatus = writeout();
      prepare_next();
      if (wstatus) return wstatus;
    }

  Evt->Copy( (int *) &(bptr->data[current_index]), Evt->getEvtLength(), &nw);
	
  left -= nw;
  current_index += nw;
  bptr->Length  += nw*4;
  bptr->Bufseq++;
  dirty =1;
  return 0;
}


// ----------------------------------------------------------
int ospBuffer::addPacket( const Packet *p) 
{
  int len;

  if ( ! good_object) return 0;

  len = current_event->addPacket(p);
  if (len < 0) return 0;
    
  left -= len;
  current_index += len;
  bptr->Length  += len*4;
  

  return len;
}




// ----------------------------------------------------------
int ospBuffer::addUnstructPacketData(PHDWORD * data, 
		    const int length,
		    const int id,
		    const int wordsize,
		    const int hitformat)
{
  int len;

  if ( ! good_object) return 0;

  len = current_event->addUnstructPacketData(data, length
					     ,id , wordsize , hitformat);
  if (len < 0) return 0;
    
  left -= len;
  current_index += len;
  bptr->Length  += len*4;
  
  // for (int k = 0; k<current_index; k++)
  //  COUT << k << " " << bptr->data[k] << std::endl;

  //  COUT << "------------------" << std::endl;


  return len;
}

// ---------------------------------------------------------
int ospBuffer::prepare_next()
{
  
  // re-initialize the event header length
  bptr->Length =  BUFFERHEADERLENGTH*4;
  bptr->ID = 0xffffc0c0;
  bptr->Bufseq = 0;
  sequence++;
  bptr->Runnr = runnumber;

  current_index = 0;
  left=max_size - BUFFERHEADERLENGTH ;
  has_end = 0;
  dirty = 1;
  return 0;
}

// ----------------------------------------------------------
int ospBuffer::setMaxSize(const int size)
{
  if (size < 0) return -1;
  if (size == 0) max_size = max_length;
  else
    {
      max_size = (size + ( BUFFERBLOCKSIZE - size%BUFFERBLOCKSIZE) ) /4;
      if (max_size > max_length)
	{
	  max_size = max_length;
	  return -2;
	}
    }
  return 0;
}

// ----------------------------------------------------------
int ospBuffer::getMaxSize() const
{
  return max_size;
}

// ----------------------------------------------------------
unsigned long long ospBuffer::getBytesWritten() const
{
  return byteswritten;
}

// ----------------------------------------------------------
// 
//
int ospBuffer::writeout()
{

  //  void *writeThread(void *);

  if ( ! good_object || fd <= 0 ) return -1;


      if (! dirty) return 0;
      
      if (! has_end) addEoB();

      if (fd < 0) return 0;


      unsigned int ip =0;
      char *cp = (char *) bptr;

      while (ip < bptr->Length)
	{
	  int n = write ( fd, cp, BUFFERBLOCKSIZE);
	  if ( n != BUFFERBLOCKSIZE)
	    {
	      std::cout << " could not write output, bytes written: " << n << std::endl;
	      return 0;
	    }
	  cp += BUFFERBLOCKSIZE;
	  ip+=BUFFERBLOCKSIZE;
	}
      dirty = 0;
      byteswritten += ip;
      return 0;
#ifdef WITHTHREADS
  
    }
  else
    {
      //wait for a potential old thread to complete
      if (ThreadId) 
	{
	  pthread_join(ThreadId, NULL);
	  byteswritten += thread_arg[2];  // the number of bytes written from previosu thread
	}
      if (! dirty) return 0;
      
      if (! has_end) addEoB();
     
      if (fd <  0) return 0;

      //swap the buffers around
      buffer_ptr tmp = bptr_being_written;
      bptr_being_written = bptr;
      bptr = tmp;
      dirty = 0;
      // now fork off the write thread

      thread_arg[0] = (int) fd;
      thread_arg[1] = (int)  bptr_being_written;
      thread_arg[2] = 0;

      //      COUT << "starting write thread" << std::endl;
      int s = pthread_create(&ThreadId, NULL, ophBuffer::writeThread, (void *) thread_arg);
      //COUT << "create status is " << s << std::endl;

      return 0;

    }
#endif
}



// ----------------------------------------------------------
int ospBuffer::addEoB()
{
  if (has_end) return -1;
  bptr->data[current_index++] = 2;  
  bptr->data[current_index++] = 0;
  bptr->Length  += 2*4;

  has_end = 1;
  return 0;
}


// ----------------------------------------------------------
ospBuffer::~ospBuffer()
{
  writeout();
  if (our_fd) close (fd);

}



