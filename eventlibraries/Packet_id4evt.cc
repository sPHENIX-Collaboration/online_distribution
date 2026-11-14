#include "Packet_id4evt.h"
#include <string.h>

Packet_id4evt::Packet_id4evt(subevtdata_ptr data)
  :Packet_w4 (data){}
  
int *Packet_id4evt::decode ( int *nwout)
{
  int *p,*k;
  int olength;
  int temp[MAX_OUTLENGTH];

  int dlength = ( getLength()-4) - getPadding();
  int i;
  int *SubeventData = &SubeventHdr->data;

  int status = decode_id4evt( temp, SubeventData, dlength
			  ,MAX_OUTLENGTH, &olength);

  if (status || olength<=0 ) return NULL;
 
  p = new int[olength];
  k = p;
  for (i =0; i<olength; i++) 
    {
      *k++ = temp[i];
      //     std::cout << i << "  " << temp[i] << std::endl;
    } 
  *nwout = olength;
  return p;
}







int Packet_id4evt::iValue(const int ch, const char *what)
{
  if ( strcmp(what, "CHANNELS") == 0) return getArraylength(what);
  return Packet_A::iValue(ch, what);
}
