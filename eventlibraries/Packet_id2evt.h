#ifndef __PACKET_ID2EVT_H__
#define __PACKET_ID2EVT_H__

#include "Packet_A.h"


#ifndef __CINT__
class WINDOWSEXPORT Packet_id2evt : public Packet_w2 {
#else
class  Packet_id2evt : public Packet_w2 {
#endif

public:
  Packet_id2evt( subevtdata_ptr);

  // iValue(0,"CHANNELS") is the number of values ("channels" of an ADC);
  // any other "what" gives value ch, as before
  using Packet_A::iValue;
  int iValue(const int ch, const char *what);

protected:
  int *decode (int *);
};


#endif /* __PACKET_ID2EVT_H__ */
