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
  
  // the BCO
  int i = 3;  // 3,4,5,6 are the BCO
  BCO = 0;
  for ( int j = 0; j < 3; j++)
    {
      BCO |= buffer[i+j];
      BCO<<=16;
    }
  BCO |= buffer[i+3];

  // 7,8,9,10 are the bunch number
  BunchNumber = 0;
  i = 7;
  for ( int j = 0; j < 3; j++)
    {
      BunchNumber |= buffer[i+j];
      BunchNumber<<=16;
    }
  BunchNumber |= buffer[i+3];
  BunchNumber &= 0x7f;
  
  //cout << "bunch number " << BunchNumber << endl;

  unsigned long long tag = 0;

  // 11,12,13,14 are 0x12345678ab - let's check
  i = 11;
  for ( int j = 0; j < 3; j++)
    {
      tag |= buffer[i+j];
      tag<<=16;
    }
  tag |= buffer[i+3];
  
  if ( tag != 0x123456789abcdef)
    {
      cout << " wrong tag " << hex << "0x" << tag << dec << endl;
    }


  TriggerInput = 0;
  TriggerVector = 0;

  // 15,16,17,18 are the trigger input 
  i = 15;
  for ( int j = 0; j < 3; j++)
    {
      TriggerInput |= buffer[i+j];
      TriggerInput<<=16;
    }
  TriggerInput |= buffer[i+3];
  
  // 19,20,21,22 are the trigger vector
  i = 19;
  for ( int j = 0; j < 3; j++)
    {
      TriggerVector |= buffer[i+j];
      TriggerVector<<=16;
    }
  TriggerVector |= buffer[i+3];
  
  //  cout << "TriggerInput " << hex << "0x" << TriggerInput << dec << endl;
  //  cout << "TriggerVector " << hex << "0x" << TriggerVector << dec << endl;

  // 23,24,25,26 are 0xdeadbeefbas5eba11 - let's check
  i = 23;
  tag = 0;
  for ( int j = 0; j < 3; j++)
    {
      tag |= buffer[i+j];
      tag<<=16;
    }
  tag |= buffer[i+3];

  if ( tag != 0xdeadbeefba5eba11)
    {
      cout << " wrong tag " << hex << "0x" << tag << dec << endl;
    }
  
  
  int index = 0;
  unsigned long long s;
  // here start the scalers
  for ( i = 27 ; i < 27+3*4*64; i+=4)
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
  
  if ( strcmp(what,"TriggerVector") == 0)
    {
      return TriggerVector;
    }

  if ( strcmp(what,"TriggerInput") == 0)
    {
      return TriggerInput;
    }
  
  if ( strcmp(what,"BunchNumber") == 0)
    {
      return BunchNumber;
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

  os << "packet nr:       " << iValue(0) << endl;
  os << "Beam Clock:      " << lValue(0, "BCO") << endl;
  os << "Trigger Input:   " << lValue(0, "TriggerInput") << endl;
  os << "Trigger Vector:  " << lValue(0, "TriggerVector") << endl;
  os << "Bunch Number:    " << lValue(0, "BunchNumber") << endl << endl;
  os << "Trg #                  raw              live              scaled" << endl;
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




