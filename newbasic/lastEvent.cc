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

#define coutfl cout << __LINE__ << "  " << __FILE__ << " "


#define RCDAQEVENTITERATOR 1
#define FILEEVENTITERATOR 2
#define TESTEVENTITERATOR 3
#define ONCSEVENTITERATOR 4

#if defined(SunOS) || defined(Linux) || defined(OSF1)
void sig_handler(int);
#else
void sig_handler(...);
#endif

std::vector<int> packetSelection;



int rangeParser ( const std::string string, std::vector<int> &selection);
int subeventid=0;



// we make it a global variable so the signal; handler can get at it.

void dump(Event *);
void dlist(Event *);
  
void exitmsg()
{
  COUT << "** usage: lastEvent -div file" << std::endl;
  exit(0);
}

void evtcountexitmsg()
{

  exit(0);
}


void exithelp()
{
  COUT << std::endl;
  COUT << "lastEvent finds the last event(s) in a given file in an efficient way." << std::endl;
  COUT << "  List of options: " << std::endl;
  //  COUT << " -e <event number>" << std::endl;
  //COUT << " -c <number> get nth event (-e gives event with number n)" << std::endl;
  COUT << "  -n <number> show the last n events. At most the number of events in the last buffer will be shown." << std::endl;
  COUT << "  -i identify the event (default unless -d is given)" << std::endl;
  COUT << "  -d dump this event" << std::endl;
  COUT << "  -l list the packets (as is dlist)" << std::endl;
  COUT << "  -p dump these packet ids - same selections as with ddump" << std::endl;

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

  int dump_flag = 0;
  int identify_flag = 0;
  int list_flag = 0;
  unsigned int repeatcount=1;


  //  extern char *optarg;
  extern int optind;
  int c;

  
  while ((c = getopt(argc, argv, "hildvn:p:")) != EOF)
    switch (c) 
      {
      case 'd':
	dump_flag=1;
        break;

      case 'i':
	identify_flag=1;
        break;

      case 'l':
	list_flag=1;
        break;

      case 'n':
	if ( !sscanf(optarg, "%d", &repeatcount) ) exitmsg();
	break;

      case 'p':
	if ( rangeParser ( optarg, packetSelection) ) exitmsg();
	subeventid=1;  // yes, select
	break;

      case 'h':
	exithelp();
	break;
      }

  if ( ! dump_flag) identify_flag=1;

  
  unsigned int allocatedsize = 0;
  PHDWORD *bp = 0;

  if ( optind+1 > argc) exitmsg();
	
  int fd = open(argv[optind], O_RDONLY | O_LARGEFILE);
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
	  //cout << "found buffer at " << current_position << " record  " << current_position / BUFFERBLOCKSIZE << endl;
	  break;
	}
    }

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

  
  if ( evt_vector.size() > 0)
    {
      //coutfl << "repeatcount " << repeatcount << " vector size " << evt_vector.size() << endl;
      if ( repeatcount > evt_vector.size()) repeatcount = evt_vector.size();
      for ( unsigned int i = evt_vector.size() - repeatcount; i< evt_vector.size(); i++)
	{
	  //cout << setw(4) << i << " ";
	  if (identify_flag) evt_vector[i]->identify();
	  if (list_flag)     dlist(evt_vector[i]);
	  if (dump_flag)     dump(evt_vector[i]);
	}
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

  if ( subeventid)
    {
      Packet *s;
      std::vector<int>::const_iterator vit;
      for (vit= packetSelection.begin(); vit!= packetSelection.end(); ++vit)
	{
	  if ( (s = evt->getPacket(*vit)) )  // get the subevent
	    {
	      s->dump();
	      delete s;
	    }
	}
    }
  else
    {
      Packet *p[10000];
      int nw = evt->getPacketList(p, 10000);
      for (int i=0; i<nw; i++)
	{
	  p[i]->dump();
	  delete p[i];
	}
      
    }

}

void dlist(Event * evt)
{
  Packet *p[10000];
  int nw = evt->getPacketList(p, 10000);
  for (int i=0; i<nw; i++)
    {
      p[i]->identify();
      delete p[i];
    }
  
}



int rangeParser ( const std::string string, std::vector<int> &selection)
{
  std::vector<std::string>::const_iterator it, itr;
  std::vector<std::string> strs,r;

//  std::vector<int>::const_iterator vit;
  int low,high,i;
  boost::split(strs,string, boost::is_any_of(","));

  for (it= strs.begin(); it!= strs.end(); ++it)
    {
      boost::split(r,*it,boost::is_any_of("-"));

      itr = r.begin();
      low = high =boost::lexical_cast<int>(r[0]);
      itr++;
      if(itr!=r.end())
	{
	  high = boost::lexical_cast<int>(r[1]);
	}
      for(i=low;i<=high;++i)
	{
	  selection.push_back(i);
	}
    }

  //  for(vit= selection.begin(); vit!= selection.end(); ++vit)
  //  {
  //    std::cout<<*vit<<std::endl;
  //  }
  return 0;

}
