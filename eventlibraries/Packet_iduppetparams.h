#ifndef __PACKET_IDUPPETPARAMS_H__
#define __PACKET_IDUPPETPARAMS_H__


#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_iduppetparams : public  Packet_w4 {
#else
class  Packet_iduppetparams : public  Packet_w4 {
#endif

public:
  Packet_iduppetparams( subevtdata_ptr);
  ~Packet_iduppetparams();

  //  int    iValue(const int);
  int    iValue(const int,const char *);
  int    iValue(const int);
  void  dump ( OSTREAM& os = COUT) ;

protected:
  int *decode (int *);

};



#endif /* __PACKET_IDUPPETPARAMS_H__ */
