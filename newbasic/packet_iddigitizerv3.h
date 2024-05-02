#ifndef __PACKET_IDDIGITIZERV3_H__
#define __PACKET_IDDIGITIZERV3_H__


#include "packet_w124.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_iddigitizerv3 : public  Packet_w4 {
#else
class  Packet_iddigitizerv3 : public  Packet_w4 {
#endif

public:
  Packet_iddigitizerv3( PACKET_ptr);
  ~Packet_iddigitizerv3();

  int    iValue(const int sample, const int ch);
  int    iValue(const int ,const char * what);
  long long   lValue(const int ,const char * what);
  void  dump ( OSTREAM& os = COUT) ;


protected:
  int decode ();
  unsigned int decode_FEM ( unsigned int *k, const int fem_nr, const int len);

  int _broken;

#define NR_FEMS 4

  // header info per packet
 
  unsigned int _evtnr;
  unsigned int _clock_rollover;
  unsigned int _evtrollover;
  int _module_address;
  unsigned int _xmit_clock;

  // info per FEM
  int _fem_slot[NR_FEMS];
  int _fem_evtnr[NR_FEMS];
  int _fem_clock[NR_FEMS];

  int _fem_checksum_MSB[NR_FEMS];
  int _fem_checksum_LSB[NR_FEMS];
  int _fem_calculated_checksum_MSB[NR_FEMS];
  int _fem_calculated_checksum_LSB[NR_FEMS];

  int _nsamples;
  int _nr_modules;

  int _nchannels;
  int _is_decoded;

  bool isZeroSuppressed[NR_FEMS*64];
  bool _AnyChannelisSuppressed;

  int adc[32][NR_FEMS*64];
  int pre_post[2][NR_FEMS*64];


};


#endif /* __PACKET_IDDIGITIZERV3_H__ */
