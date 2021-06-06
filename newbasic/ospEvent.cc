#include "ospEvent.h"
#include "EventTypes.h"

#include <time.h>
#include <string.h>

// the constructor first ----------------
ospEvent::ospEvent (PHDWORD * where, const int length
		, const int irun, const int etype, const int evtseq)
{
  evthdr = ( oncsevtdata_ptr ) where;
  evthdr->evt_type = etype;
  max_length = length;
  prepare_next (evtseq, irun);
}

void ospEvent::set_event_type(const int etype)
{
   evthdr->evt_type = etype;
}

int ospEvent::prepare_next()
{
  // re-initialize the event header length
  evthdr->evt_length =  EVTHEADERLENGTH;

  // if < 0, just increment the current seq. number
  evthdr->evt_sequence++;

  // reset the current data index, and the leftover counter
  current = 0;
  left=max_length - EVTHEADERLENGTH ;
  evthdr->date = time(0);
  evthdr->time = 0;

  return 0;

}

int ospEvent::prepare_next( const int evtseq 
			  , const int irun )
{
  // re-initialize the event header length
  evthdr->evt_length =  EVTHEADERLENGTH;

  // if < 0, just increment the current seq. number

  evthdr->evt_sequence = evtseq;

  // if > 0, adjust the run number, else just keep it.
  evthdr->run_number=irun;
  
  // reset the current data index, and the leftover counter
  current = 0;
  left=max_length - EVTHEADERLENGTH ;
  evthdr->date = time(0);
  evthdr->time = 0;

  return 0;
}

int  ospEvent::addPacket( const Packet *p)
{

  int* packetstart;
  int packetlength  = p->getLength(); 

  
  packetstart = &(evthdr->data[current]);

  p->copyMe( (int *)packetstart, packetlength );

  if (packetlength >0)  
    { 
      evthdr->evt_length += packetlength;
      current +=  packetlength;
      left -= packetlength;
      return packetlength;
    }
  else return  -1;
}



int  ospEvent::addUnstructPacketData(PHDWORD * data, 
		    const int length,
		    const int id,
		    const int wordsize,
		    const int hitformat)
{

//  int* packetstart;
  
  subevtdata_ptr sevt =  (subevtdata_ptr) &(evthdr->data[current]);
  sevt->sub_length =  SEVTHEADERLENGTH;
  sevt->sub_id = id;
  sevt->sub_type=wordsize;
  sevt->sub_decoding = hitformat;
  sevt->reserved[0] = 0;
  sevt->reserved[1] = 0;
  
  memcpy(&sevt->data , data, length * sizeof (int) );
  sevt->sub_length += length;
  
  evthdr->evt_length +=  sevt->sub_length;
  current +=  sevt->sub_length;
  left -=  sevt->sub_length;
  return  sevt->sub_length;

}

