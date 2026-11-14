#include <string.h>

#include "Event_A.h"
#include "oncsStructures.h"
#include "packet_collection.h"
#include "oncsSubConstants.h"

// the constructor first ----------------
Event_A::Event_A (int *data)
{
  is_data_type = 0;
  hasMap = 0;
  errorcode = 0;
  EventData = (oncsevtdata_ptr) data;
  originBuffer = 0;
}

Event_A::~Event_A ()
{
  if (is_data_type) delete [] (int *) EventData;
}

// the info-type calls
unsigned int
Event_A::getEvtLength() const
{
  return EventData->evt_length;
}

int
Event_A::getEvtType() const
{
  return EventData->evt_type;
}

int
Event_A::getEvtSequence() const
{
  return EventData->evt_sequence;
}

time_t
Event_A::getTime () const
{
  return EventData->time;
}

int
Event_A::getRunNumber() const
{
  return EventData->run_number;
}

// PHTimeStamp *
// Event_A::getTimeStamp() const
// {
//   return 0;
// }


void Event_A::identify (OSTREAM &os) const
{
  os << std::dec << " -- Event " << SETW(5)  << EventData->evt_sequence;

  os << " Run: "  << SETW(5)  << EventData->run_number;

  os << " length: "     << SETW(5)  <<EventData->evt_length;

  os << " type: "      << SETW(2)  << EventData->evt_type
     << " (" << get_evt_mnemonic(EventData->evt_type) << ")"
     << "  " << getTime()
     <<  std::endl;


};

int Event_A::convert()
{
  if (is_data_type) return -1;

  int *tmp;

  tmp = new int[getEvtLength()];
  int *from= (int *)  EventData;
  int *to=tmp;
  for (unsigned int k=0; k< getEvtLength(); k++)
    {
      *to++ = *from++;
    }

  EventData = (oncsevtdata_ptr) tmp;

  is_data_type = 1;
  return 0;

}

int Event_A::is_pointer_type() const
{
  if (is_data_type) return 0;
  else return 1;
}


Packet*
Event_A::getPacket (const int id, const int hitFormat)
{
  PHDWORD *pp;

  if (!hasMap) createMap();

  if ( errorcode) return 0;

  pp = pmap[id];
  if (!pp) return 0;

  return makePacket(pp,hitFormat);

}

Packet*
Event_A::getPacket (const int id)
{
  return getPacket (id, 0);
}

int Event_A::createMap()
{
  int i;
  subevtdata_ptr sevt_ptr;

  int  datalength = EventData->evt_length - EVTHEADERLENGTH;

  // loop through the subevents and see if we locate the id

  for (i=0; i<datalength; i+=  EventData->data[i])
    {
      if ( EventData->data[i] <= 0) 
	{
	  std::cout << "found 0-length packet" << std::endl;
	  errorcode =-1;
	  break;
	}

      // each data[i] is the start of a subevent;
      // we map it on a subevent_ptr

      sevt_ptr = (subevtdata_ptr) &EventData->data[i];

      // now we see what type of subevent we are supposed
      // to return

      pmap[sevt_ptr->sub_id] = (PHDWORD *) &EventData->data[i];
      //      std::cout << __FILE__ << "  " << __LINE__ << " subid, adr " << sevt_ptr->sub_id << "  " << pmap[sevt_ptr->sub_id] << "  " << *(pmap[sevt_ptr->sub_id]) << std::endl;
    }
  hasMap = 1;

  return 0;
}

Packet *Event_A::makePacket(PHDWORD *pp, const int hitFormat)
{
  int wanted_hitformat;

  subevtdata_ptr sevt_ptr = (subevtdata_ptr) pp;

  if (hitFormat)  wanted_hitformat = hitFormat;
  else wanted_hitformat  = sevt_ptr->sub_decoding;

  switch (wanted_hitformat)
    {

    case (ID4EVT):
      return new
	Packet_id4evt( sevt_ptr);
      break;

    case (ID2EVT):
      return new
	Packet_id2evt( sevt_ptr );
      break;

    case (ID8EVT):
      return new
	Packet_id8evt( sevt_ptr );
      break;

    case (IDCSTR):
      return new
	Packet_idcstr( sevt_ptr );
      break;

    case (IDRTCLOCK):
      return new
	Packet_idrtclock( sevt_ptr );
      break;

    case (IDSIS3300):
      return new
	Packet_idsis3300( sevt_ptr );
      break;

    case (IDSIS3300R):
      return new
	Packet_idsis3300r( sevt_ptr );
      break;

    case (IDCAENV792):
      return new
	Packet_idcaenv792( sevt_ptr );
      break;

    case (IDCAENV1742):
      return new
	Packet_idcaenv1742( sevt_ptr );
      break;

    case (IDCAENV1742CALIB):
      return new
	Packet_idcaenv1742calib( sevt_ptr );
      break;


    case (IDRCPETDATA):
      return new
	Packet_idrcpetdata( sevt_ptr );
      break;


    case (IDBSPETDATA):
      return new
	Packet_idbspetdata( sevt_ptr );
      break;

    case (IDUPPETDATA):
      return new
	Packet_iduppetdata( sevt_ptr );
      break;

    case (IDUPPETDATA_V104):
      return new
	Packet_iduppetdata_v104( sevt_ptr );
      break;

    case (IDUPPETPARAMS):
      return new
	Packet_iduppetparams( sevt_ptr );
      break;

    case (IDSRSV01):
      return new
	Packet_idsrs_v01( sevt_ptr );
      break;

    case (IDFNALMWPC):
      return
	new Packet_idfnalmwpc( sevt_ptr );
      break;

    case (IDFNALMWPCV2):
      return
	new Packet_idfnalmwpcv2( sevt_ptr );
      break;

    case (IDDRS4V1):
      return new
	Packet_iddrs4v1( sevt_ptr );
      break;


    case (IDH2GCROC3):
      return new
	Packet_idh2gcroc3( sevt_ptr );
      break;

    case (IDH2GCROC3_10G):
      return new
	Packet_idh2gcroc3_10G( sevt_ptr );
      break;

    case (IDCALOROC1B):
      return new
	Packet_idcaloroc1b( sevt_ptr );
      break;

    case (IDASTROV1):
      return new
	Packet_idastrov1( sevt_ptr );
      break;

    default:
      switch (sevt_ptr->sub_type)
	{
	case 1:
	  return  new Packet_w1(sevt_ptr);
	  break;
	case 2:
	  return new Packet_w2(sevt_ptr);
	  break;
	case 4:
	  return new Packet_w4(sevt_ptr);
	  break;
	case 8:
	  return new Packet_w8(sevt_ptr);
	  break;
	default:
	  return new Packet_w4(sevt_ptr);
	}

      return 0;
    }

  return 0;
}


int Event_A::getPacketList( Packet* sl[], const int ne)
{


  if (!hasMap) createMap();
  if ( errorcode) return 0;

  std::map <int, PHDWORD *>::const_iterator it;


  int entries = 0;

  for ( it = pmap.begin() ; it != pmap.end(); ++it)
    {
      if ( entries == ne) break;
      PHDWORD *p = it->second;
      //std::cout << __FILE__ << "  " << __LINE__ << " subid, adr " << it->first << "  " << it->second << "  " << *(it->second) << std::endl;

      sl[entries++] = makePacket(p);
    }

  return entries;
}

std::vector<Packet *>  Event_A::getPacketVector()
{

  std::vector<Packet *> v;

  if (!hasMap) createMap();
  if ( errorcode) return v;


  std::map <int, PHDWORD *>::const_iterator it;


  for ( it = pmap.begin() ; it != pmap.end(); ++it)
    {
      PHDWORD *p = it->second;
      //std::cout << __FILE__ << "  " << __LINE__ << " subid, adr " << it->first << "  " << it->second << "  " << *(it->second) << std::endl;

      v.push_back(makePacket(p));
    }

  return v;
}




// existSubevent (const int)

int
Event_A::existPacket (const int id)
{
  int i;
  subevtdata_ptr sevt_ptr;

  int  datalength = EventData->evt_length - EVTHEADERLENGTH;

  // loop through the subevents and see if we locate the id

  for (i=0; i<datalength; i+=  EventData->data[i])
    {

      // each data[i] is the start of a subevent;
      // we map it on a subevent_ptr

      sevt_ptr = (subevtdata_ptr) &EventData->data[i];

      // now we see what type of subevent we are supposed
      // to return

      if ( sevt_ptr->sub_id == id) return 1;

    }
  return 0;
}


// the Copy routine
int
Event_A::Copy (int * array, const unsigned int length, int *nw, const char *what)
{

    if (length< getEvtLength() )
    {
      *nw = 0;
      return -1;
    }
  char *to = (char *) array;
  char *from;
  unsigned int l;
  if ( strcmp (what, "DATA") ==0  )
    {
      from= (char *)  &EventData->data[0];
      l = getEvtLength() - EVTHEADERLENGTH;
    }
  else
    {
      from= (char *) EventData;
      l = getEvtLength();
    }
  //  for (i=0; i<l ; i++) *to++ = *from++;
  //
  *nw = l;
  memcpy (to, from, l*4);
  return 0;
}
