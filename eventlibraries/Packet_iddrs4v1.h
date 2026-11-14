#ifndef __PACKET_IDDRS4V1_H__
#define __PACKET_IDDRS4V1_H__

#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_iddrs4v1 : public  Packet_w4 {
#else
class  Packet_iddrs4v1 : public  Packet_w4 {
#endif

public:
  Packet_iddrs4v1( subevtdata_ptr);
  ~Packet_iddrs4v1();

  int      iValue(const int,const char *);
  float    rValue(const int,const char *);
  float    rValue(const int,const int );
  void  dump ( OSTREAM& os = COUT) ;

protected:
  int *decode (int *);
  float *wave;
  int samples;
  int enabled_channelmask;
  int channel_offset[4];

};


#endif /* __PACKET_IDDRS4V1_H__ */
