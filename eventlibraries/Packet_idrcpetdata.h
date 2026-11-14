#ifndef __PACKET_IDRCPETDATA_H__
#define __PACKET_IDRCPETDATA_H__


#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_idrcpetdata : public  Packet_w4 {
#else
class  Packet_idrcpetdata : public  Packet_w4 {
#endif

public:
  Packet_idrcpetdata( subevtdata_ptr);
  ~Packet_idrcpetdata();

  //  int    iValue(const int);
  int    iValue(const int,const char *);
  double dValue(const int channel);
  long long  lValue(const int channel);
  void  dump ( OSTREAM& os = COUT) ;

protected:
  int *decode (int *);
  int samples;
  int evnr;
  double *d_time;
  long long *lval;
  int d_timelength;
};



#endif /* __PACKET_IDRCPETDATA_H__ */
