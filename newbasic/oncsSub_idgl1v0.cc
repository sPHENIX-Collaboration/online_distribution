#include "oncsSub_idgl1v0.h"

#include <string.h>

using namespace std;

oncsSub_idgl1v0::oncsSub_idgl1v0(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{

  BCO = 0;
  packet_nr = 0;

  memset (scalers, 0, sizeof(scalers) );
  is_decoded = 0;
  
}
  
int oncsSub_idgl1v0::decode ()
{
  if (is_decoded) return 0;
  is_decoded = 1;
  
  unsigned short *buffer = ( unsigned short *)  &SubeventHdr->data;

  
  // the first value is alaways "face" - we start at iValue(1)
  // and (1) and (2) are a 32bit uint32.
  packet_nr = ( buffer[1] << 16) | buffer[2];
  
  
  // from here on out we have 64bit values
  
  
  int i = 3;  // 3,4,5,6 are the BCO
  BCO = 0;
  for ( int j = 0; j < 3; j++)
    {
      BCO |= buffer[i+j];
      BCO<<=16;
    }
  BCO |= buffer[i+3];

  int index = 0;
  unsigned long long s;
  // here start the scalers
  for ( i = 7 ; i < 7+3*4*64; i+=4)
    {
      s = 0;
      for ( int j = 0; j < 3; j++)
	{
	  s |=  buffer[i+j];
	  s<<=16;
	}
      s |=  buffer[i+3];
      
      
      //cout << __FILE__ << " " << __LINE__ << " " << setw(3) << index << "  " << hex << setw(10) << s  << dec <<"   " << s << endl;
      scalers[index++] = s;
      
    }
  
  
  return 0;  
}

int oncsSub_idgl1v0::iValue(const int i)
{
  decode();
  return packet_nr;
}

long long oncsSub_idgl1v0::lValue(const int i, const int k)
{
  decode();
  if ( i < 0 || i >=64 || k < 0 || k >2) return 0;
  return scalers[3*i + k];
}

long long oncsSub_idgl1v0::lValue(const int i, const char *what)
{
  decode();
  
  if ( strcmp(what,"BCO") == 0)
    {
      return BCO;
    }
  
  if ( i < 0 || i >=64) return 0;

  if ( strcmp(what,"TRIGGERRAW") == 0)
    {
      return lValue(i, 0);
    }

  if ( strcmp(what,"TRIGGERLIVE") == 0)
    {
      return lValue(i, 1);
    }

  if ( strcmp(what,"TRIGGERSCALED") == 0)
    {
      return lValue(i, 2);
    }

  return 0;

}


void oncsSub_idgl1v0::dump(std::ostream &os)
{

  os << "packet nr:   " << iValue(0) << endl;
  os << "Beam Clock:  " << lValue(0, "BCO") << endl << endl;
  os << "Trg #                  raw             scaled               live" << endl;
  os << "----------------------------------------------------------------" << endl;

  int i;
  
  for (i = 0; i< 64; i++)
    {
      if ( lValue(i, 0) ||  lValue(i, 1) ||  lValue(i, 2) )
	{
	  os << setw(3) << i << "    ";
	  os << " " << setw(18) << lValue(i,0)
	     << " " << setw(18) << lValue(i,1)
	     << " " << setw(18) << lValue(i,2)
	     << endl;
	}
    }
  os << endl;
}




