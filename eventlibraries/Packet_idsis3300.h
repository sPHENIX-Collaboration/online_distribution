#ifndef __PACKET_IDSIS3300_H__
#define __PACKET_IDSIS3300_H__


#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_idsis3300 : public  Packet_w4 {
#else
class  Packet_idsis3300 : public  Packet_w4 {
#endif

public:
  Packet_idsis3300( subevtdata_ptr);

  int    iValue(const int,const int);
  int    iValue(const int,const char *);
  void  dump ( OSTREAM& os = COUT) ;

protected:
  int *decode (int *);
  int samples;
  int wraparound;
};



#endif /* __PACKET_IDSIS3300_H__ */
