//
// testEventIterator   mlp 9/11/96
//
// this iterator delivers test-type Event objects which can
// be used to debug the analysis software which uses the events
// delivered. The events contain 1 frame and one packet


#include <time.h>

#include "testEventiterator.h"
#include <stdlib.h>

#include "oncsEvent.h"

testEventiterator::~testEventiterator()
{
  delete R;
}

testEventiterator::testEventiterator()
{
  R = new simpleRandom(876565);
  current_event = 0;

  typedef struct evt_data
  {
    int evt_length;
    int evt_type;
    int evt_sequence;
    int run_number;
    int date;
    int time;
    int reserved[2];
    int data[];
  } *e_ptr;

typedef struct subevt_data
 {
   int   sub_length;
   short sub_id;
   short sub_type;
   short sub_decoding;
   short sub_padding;
   int   reserved;
   int   data[];
 } *s_ptr;



 current_event = 0;
  
  e_ptr e = (e_ptr) e_data;

  e->evt_length    = 8;
  e->evt_type      = 1;
  e->evt_sequence  = 1;
  e->run_number    = 1331;
  e->date          = 0;
  e->time          = time(0);
  e->reserved[0]   = 0;
  e->reserved[1]   = 0;

  e_sequence = & e->evt_sequence;
  e_time = &e->time;

  
  s_ptr s1001 = (s_ptr) e->data;

  s1001->sub_length    = 4;
  s1001->sub_id        = 1001;
  s1001->sub_type      = 4;
  s1001->sub_decoding  = 6;
  s1001->sub_padding   = 0;
  s1001->reserved      = 0;

  
  for (int i = 0; i < 20; i++)
    {
      s1001->data[i] = i;
      s1001->sub_length++;
    }
  
  e->evt_length += s1001->sub_length;
  

  s_ptr s1002 = (s_ptr) &s1001->data[20];
  s1002->sub_length    = 4;
  s1002->sub_id        = 1002;
  s1002->sub_type      = 2;
  s1002->sub_decoding  = 5;
  s1002->sub_padding   = 0;
  s1002->reserved      = 0;
  
  short *x = (short *) &s1002->data;
  
  for (int i = 0; i < 64; i++)
    {
      x[i] = i;
    }
  
  s1002->sub_length+=32;
  e->evt_length += s1002->sub_length;
  

  s_ptr s1003 = (s_ptr) &s1002->data[32];
  s1003->sub_length    = 8;   // we have the length
  s1003->sub_id        = 1003;
  s1003->sub_type      = 4;
  s1003->sub_decoding  = 6;
  s1003->sub_padding   = 0;
  s1003->reserved      = 0;

  e_1003values = &(s1003->data[0]);
  
  s1003->data[0] = 0;
  s1003->data[1] = 0;
  s1003->data[2] = 0;
  s1003->data[3] = 0;
  
  e->evt_length += s1003->sub_length;
  
}


void  
testEventiterator::identify (OSTREAM &os) const
{ 
  os << getIdTag()  << std::endl;

};

const char * 
testEventiterator::getIdTag () const
{ 
  return " -- testEventiterator (standard) ";
};



Event *
testEventiterator::getNextEvent()
{

  *e_sequence =  ++current_event;
  *e_time = time(0);
  float scale = 10;

  for ( int i = 0; i < 4; i++)
    {
      e_1003values[i] = R->gauss(0 ,  scale  );
      scale *= 10.;
    }

  Event *e = new  oncsEvent(e_data);
  e->convert();

  return e;
}
