#include <stdlib.h>
#include <signal.h>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "fileEventiterator.h"
#include "oncsEventiterator.h"
#include "Event.h"

#include "gzbuffer.h"
#include "oncsBuffer.h"

#include <iostream>
#include <iomanip>

using namespace std;

#ifdef HAVE_GETOPT_H
#include "getopt.h"
#endif

#include<vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>



#define RCDAQEVENTITERATOR 1
#define FILEEVENTITERATOR 2
#define TESTEVENTITERATOR 3
#define ONCSEVENTITERATOR 4

#if defined(SunOS) || defined(Linux) || defined(OSF1)
void sig_handler(int);
#else
void sig_handler(...);
#endif

// we make it a global variable so the signal; handler can get at it.

void dump(Event *);

void exitmsg()
{
  COUT << "** usage: ddump -ecnstdfghiIFTOv datastream" << std::endl;
  COUT << "    type  ddump -h   for more help" << std::endl;
  exit(0);
}

void evtcountexitmsg()
{

  exit(0);
}


void exithelp()
{

  exit(0);
}

#if defined(SunOS) || defined(Linux) || defined(OSF1)
void sig_handler(int i)
#else
  void sig_handler(...)
#endif
{
  COUT << "sig_handler: signal seen " << std::endl;
  exit(0);
}


int 
main(int argc, char *argv[])
{

  
  if (argc < 2) exitmsg();


  //  extern char *optarg;
  //extern int optind;
  PHDWORD *bp = 0;

  int fd = open(argv[1], O_RDONLY | O_LARGEFILE);
  if (fd == -1)
    {
      COUT << "Could not open input stream" << std::endl;
      exit(1);
    }

  off64_t file_size = lseek64(fd, 0, SEEK_END);
  off64_t current_position = file_size;

  unsigned int buffer_size = 0;
  unsigned int initialbuffer[BUFFERBLOCKSIZE];
  char *cp = (char *) initialbuffer;

  unsigned int xc;


  while (current_position > 0)
    {


      current_position -= BUFFERBLOCKSIZE;
      if (current_position < 0)
	{
	  buffer_size += current_position; // Adjust buffer size for the last read
	  current_position = 0; // Ensure we don't seek before the beginning
	}
      
        // Seek to the current position
        lseek64(fd, current_position, SEEK_SET);

        // Read data into the buffer
        ssize_t bytes_read = read(fd, initialbuffer, BUFFERBLOCKSIZE);
        if (bytes_read == -1)
	  {
            perror("Error reading from the file");
            close(fd);
            return 1;
	  }

      // get the buffer length into a dedicated variable
      if (initialbuffer[1] == BUFFERMARKER || initialbuffer[1]== GZBUFFERMARKER 
	  ||  initialbuffer[1]== LZO1XBUFFERMARKER || initialbuffer[1]== ONCSBUFFERMARKER) 
	{
	  buffer_size = initialbuffer[0];
	}
      else
	{
	  unsigned int  marker = buffer::u4swap(initialbuffer[1]);
	  if (marker == BUFFERMARKER || marker == GZBUFFERMARKER || marker ==  LZO1XBUFFERMARKER || marker == ONCSBUFFERMARKER)
	    {
	      buffer_size = buffer::u4swap(initialbuffer[0]);
	    }
	}
      if ( buffer_size)
	{
	  cout << "found buffer at " << current_position << " record  " << current_position / BUFFERBLOCKSIZE << endl;
	  break;
	}
    }

  unsigned int allocatedsize;
  buffer *bptr = 0;
  
  int i;
  if (bp) 
    {
      // this tests if we have enough space in the existing buffer
      if  (buffer_size > allocatedsize*4)   // no, we delete and make a bigger one
	{
	  delete [] bp;
	  i = (buffer_size +BUFFERBLOCKSIZE-1) /BUFFERBLOCKSIZE;
	  allocatedsize = i * BUFFERBLOCKSIZE/4;
	  bp = new PHDWORD[allocatedsize];
	}
    }
  else
    {
      i = (buffer_size +BUFFERBLOCKSIZE-1) /BUFFERBLOCKSIZE;
      allocatedsize = i * BUFFERBLOCKSIZE/4;
      bp = new PHDWORD[allocatedsize];

    }

  memcpy ( bp, initialbuffer, BUFFERBLOCKSIZE);
 
  cp = (char *) bp;

  // and update the destination buffer pointer
  cp += BUFFERBLOCKSIZE;

  PHDWORD read_so_far =  BUFFERBLOCKSIZE;

  int errorinread=0;

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
      errorinread =1;
    }

  // and initialize the current_index to be the first event

  if ( ( initialbuffer[1]== GZBUFFERMARKER || 
       buffer::u4swap(initialbuffer[1])== GZBUFFERMARKER ||
       initialbuffer[1]== LZO1XBUFFERMARKER || 
       buffer::u4swap(initialbuffer[1])== LZO1XBUFFERMARKER )
       && errorinread  )
    {
      bptr = 0;
      return -3;
    }

  i = buffer::makeBuffer( bp, allocatedsize, &bptr);

  Event *evt;

  std::vector<Event *> evt_vector;
  
  while ( (evt =  bptr->getEvent()) )
    {
      evt_vector.push_back(evt);
    }

  if ( evt_vector.size() > 1)
    {
      evt_vector[evt_vector.size() -1]->identify();
      dump(evt_vector[evt_vector.size() -2]);
    }
  
  for ( unsigned int i = 0; i < evt_vector.size(); i++)
    {
      delete evt_vector[i];
    }
  evt_vector.clear();

  
   return 0;
}

void dump(Event * evt)
{
  int dumpstyle = EVT_HEXADECIMAL;
  int generic = 0;
  Packet *p[10000];
  int nw = evt->getPacketList(p, 10000);
  for (int i=0; i<nw; i++)
    {
      if (generic) p[i]->gdump(dumpstyle);
      else p[i]->dump();
      delete p[i];
    }
}
