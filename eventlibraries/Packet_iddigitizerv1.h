#ifndef __PACKET_IDDIGITIZERV1_H__
#define __PACKET_IDDIGITIZERV1_H__

#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_iddigitizerv1 : public  Packet_w4 {
#else
class  Packet_iddigitizerv1 : public  Packet_w4 {
#endif

public:
  Packet_iddigitizerv1( subevtdata_ptr);
  ~Packet_iddigitizerv1();

  int    iValue(const int sample, const int ch);
  int    iValue(const int ,const char * what);
  void  dump ( OSTREAM& os = COUT) ;


protected:
  int digitizer_decode ();


  int _nsamples;
  int _l1_delay;
  int _slot_nr;
  int _nr_modules;
  int _clock;
  int _evtnr;
  int _nchannels;
  int _slot_nr_from_data;

  int _is_decoded;

  int array[32][128];

};


#endif /* __PACKET_IDDIGITIZERV1_H__ */
