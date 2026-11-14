#ifndef __PACKET_ID8EVT_H__
#define __PACKET_ID8EVT_H__

#include "Packet_A.h"

// A plain list of signed 64-bit values (long long), the 8-byte sibling
// of ID2EVT and ID4EVT. The values are read with lValue(i); 
// iValue(0,"CHANNELS") is the number of values.

#ifndef __CINT__
class WINDOWSEXPORT Packet_id8evt : public  Packet_w8 {
#else
class  Packet_id8evt : public  Packet_w8 {
#endif

public:
  Packet_id8evt( subevtdata_ptr);

  using Packet_A::iValue;
  int    iValue(const int, const char *);
  long long  lValue(const int);
  int    getArraylength(const char *what="");

protected:
  int getNrValues() const;
};


#endif /* __PACKET_ID8EVT_H__ */
