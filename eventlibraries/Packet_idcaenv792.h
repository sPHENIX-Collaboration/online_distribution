#ifndef __PACKET_IDCAENV792_H__
#define __PACKET_IDCAENV792_H__


#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_idcaenv792 : public  Packet_w4 {
#else
class  Packet_idcaenv792 : public  Packet_w4 {
#endif

public:
  Packet_idcaenv792( subevtdata_ptr);

  int    iValue(const int);
  int    iValue(const int,const char *);
  void  dump ( OSTREAM& os = COUT) ;

protected:
  int *decode (int *);
  int samples;
  int evnr;
};



#endif /* __PACKET_IDCAENV792_H__ */
