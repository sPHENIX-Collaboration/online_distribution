#ifndef __PACKET_IDCSTR_H__
#define __PACKET_IDCSTR_H__

#include "Packet_A.h"


#ifndef __CINT__
class WINDOWSEXPORT Packet_idcstr : public Packet_w1 {
#else
class  Packet_idcstr : public Packet_w1 {
#endif

public:
  Packet_idcstr( subevtdata_ptr);
  ~Packet_idcstr();
  int iValue (const int i);
  // iValue(0,"CHANNELS") is the number of characters
  int iValue (const int i, const char *what);
  void  dump ( OSTREAM& os = COUT) ;


protected:
  int *decode (int *);
  unsigned char *sarray;
  int allocated_length;
};


#endif /* __PACKET_IDCSTR_H__ */
