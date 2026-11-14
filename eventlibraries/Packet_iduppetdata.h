#ifndef __PACKET_IDUPPETDATA_H__
#define __PACKET_IDUPPETDATA_H__


#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_iduppetdata : public  Packet_w4 {
#else
class  Packet_iduppetdata : public  Packet_w4 {
#endif

public:
  Packet_iduppetdata( subevtdata_ptr);
  ~Packet_iduppetdata();

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
  long long *tval;
  unsigned int serialnumber;
  unsigned int udpheader1;
  unsigned int udpheader2;
  int d_timelength;
};



#endif /* __PACKET_IDUPPETDATA_H__ */
