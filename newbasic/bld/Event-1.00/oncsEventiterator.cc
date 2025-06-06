//
// oncsEventIterator   mlp 4/19/1997
//
// this iterator reads events froma data file. 



#include "oncsEventiterator.h"
#include <stdio.h>
#include "oncsEvent.h"
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


// there are two similar constructors, one with just the
// filename, the other with an additional status value
// which is non-zero on return if anything goes wrong. 


oncsEventiterator::~oncsEventiterator()
{
     if (fd) close (fd);
     if (thefilename != NULL) delete [] thefilename;
     if (bp != NULL ) delete [] bp;
     if (bptr != NULL ) delete bptr;
}  


oncsEventiterator::oncsEventiterator(const char *filename)
{
  fd  = open (filename, O_RDONLY | O_LARGEFILE);
  bptr = 0;
  bp = 0;
  allocatedsize = 0;
  if (fd > 0) 
    {
      thefilename = new char[strlen(filename)+1];
      strcpy (thefilename, filename);
      last_read_status = 0;
      current_index = 0;
    }
  else
    last_read_status = 1;

}  

oncsEventiterator::oncsEventiterator(const char *filename, int &status)
{
  fd  = open (filename, O_RDONLY | O_LARGEFILE);
  bptr = 0;
  bp = 0;
  allocatedsize = 0;
  if (fd >0 ) 
    {
      thefilename = new char[strlen(filename)+1];
      strcpy (thefilename, filename);
      status = 0;
      last_read_status = 0;
      current_index = 0;
    }
  else
    {
      status = 1;
      last_read_status = 1;
    }
}  

void  
oncsEventiterator::identify (OSTREAM &os) const
{ 
  os << getIdTag() << std::endl;

};

const char * oncsEventiterator::getIdTag () const
{ 
  static char line[180];
  strcpy (line, " -- oncsEventiterator reading from ");
  strcat (line, thefilename);
  return line;
};


// and, finally, the only non-constructor member function to
// retrieve events from the iterator.

Event * oncsEventiterator::getNextEvent()
{
  Event *evt = 0;


  // if we had a read error before, we just return
  if (last_read_status) return NULL;

  // see if we have a buffer to read
  if (bptr == 0) 
    {
      if ( (last_read_status = read_next_buffer()) !=0 )
	{
	  return NULL;
	}
    }

  while (last_read_status == 0)
    {
      if (bptr) evt =  bptr->getEvent();
      if (evt) return evt;

      last_read_status = read_next_buffer();
    }

  return NULL;

}

// -----------------------------------------------------
// this is a private function to read the next buffer
// if needed. 

int oncsEventiterator::read_next_buffer()
{
  if (bptr) 
    {
      delete bptr;
      bptr = 0;
    }

  //  COUT << "reading next buffer" << std::endl; 

  // set the pointer to char to the destination buffer
  char *cp = (char *) initialbuffer;

  unsigned int xc;

  // read the first record
  xc = read ( fd, cp, 8192);

  // error of EoF?
  if ( xc < 8192 ) return -1;

  // get the length into a dedicated variable
  if (initialbuffer[1] == ONCSBUFFERID || initialbuffer[1] == PRDFBUFFERID ) // we check for both for legacy data
    {
      buffer_size = initialbuffer[0];
    }
  else
    {
      unsigned int id = oncsBuffer::i4swap(initialbuffer[1]);
      if (id == ONCSBUFFERID || id == PRDFBUFFERID ) // we check for both for legacy data
	{
	  buffer_size = oncsBuffer::i4swap(initialbuffer[0]);
	}
      else
	{
	  return 1;
	}
    }
  int i;
  if (bp) 
    {
      if  (buffer_size > allocatedsize*4)
	{
	  delete [] bp;
	  i = (buffer_size + BUFFERBLOCKSIZE -1) /BUFFERBLOCKSIZE;
	  allocatedsize = i * 2048;
	  bp = new int[allocatedsize];
	}
    }
  else
    {
      i = (buffer_size + BUFFERBLOCKSIZE) /BUFFERBLOCKSIZE;
      allocatedsize = i * BUFFERBLOCKSIZE/4;
      bp = new int[allocatedsize];
    }

  // for (i = 0; i<2048; i++ ) bp[i] = initialbuffer[i];
  memcpy ( bp, initialbuffer, BUFFERBLOCKSIZE);

  unsigned int read_so_far =  BUFFERBLOCKSIZE;
  
  cp = (char *) bp;

  // and update the destination buffer pointer
  cp += BUFFERBLOCKSIZE;

  // we calculate how many BUFFERBLOCKSIZE-sized records we need to read
  // we have already one, so this is the number of records -1.
  // normally we would round up  (buffer_size + BUFFERBLOCKSIZE -1) /BUFFERBLOCKSIZE
  int records_to_read =  (buffer_size -1) /BUFFERBLOCKSIZE;
  unsigned int bytes_to_read   =  records_to_read * BUFFERBLOCKSIZE;
  
  xc = read ( fd, cp, bytes_to_read);
  if ( xc < bytes_to_read )
    {
      COUT << "error in buffer, salvaging" << std::endl;
      bp[0] = read_so_far + xc; 
    }

  // and initialize the current_index to be the first event
  bptr = new oncsBuffer ( (PHDWORD *) bp, (PHDWORD) allocatedsize );
  return 0;
}

