#ifndef __PACKET_IDBSPETDATA_H__
#define __PACKET_IDBSPETDATA_H__


#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_idbspetdata : public  Packet_w4 {
#else
class  Packet_idbspetdata : public  Packet_w4 {
#endif

public:
  Packet_idbspetdata( subevtdata_ptr);
  ~Packet_idbspetdata();

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



#endif /* __PACKET_IDBSPETDATA_H__ */
