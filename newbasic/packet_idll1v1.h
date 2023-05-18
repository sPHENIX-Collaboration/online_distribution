#ifndef __PACKET_IDLL1V1_H__
#define __PACKET_IDLL1V1_H__


#include "packet_w124.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_idll1v1 : public  Packet_w4 {
#else
class  Packet_idll1v1 : public  Packet_w4 {
#endif

public:
  Packet_idll1v1( PACKET_ptr);
  ~Packet_idll1v1();

  int    iValue(const int sample, const int ch);
  int    iValue(const int ,const char * what);
  void  dump ( OSTREAM& os = COUT) ;


protected:
  int decode ();

  int _evt_nr;
  int _clock;

  int _slot_nr;
  int _card_nr;

  int _nsamples;

  int _even_checksum;
  int _odd_checksum;

  int _calculated_even_checksum;
  int _calculated_odd_checksum;

  int _even_checksum_ok;
  int _odd_checksum_ok;

  int _nchannels;
  int _is_decoded;

  int array[24*13 + 13][32];
  int itrig_charge[4][8][32];
  int itrig_nhit[4][32];
  int itrig_time[4][4][32];

  int triggerwords[8][32];
};


#endif /* __PACKET_IDLL1V1_H__ */
