#ifndef __PACKET_IDLL1V2_H__
#define __PACKET_IDLL1V2_H__


#include "packet_w124.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_idll1v2 : public  Packet_w4 {
#else
class  Packet_idll1v2 : public  Packet_w4 {
#endif

public:
  Packet_idll1v2( PACKET_ptr);
  ~Packet_idll1v2();

  int    iValue(const int sample, const int ch);
  int    iValue(const int ,const char * what);
  void  dump ( OSTREAM& os = COUT) ;


protected:
  int decode ();

  enum TRIGGERTYPE {
    NONE = 0,
    MBD = 1,
    EMCAL = 2,
    JET = 3
  };

  TRIGGERTYPE _trigger_type;
  int _monitor;
  int _evt_nr;
  int _clock;

  int _slot_nr;
  int _card_nr;

  int _nsamples;

  int _nfibers;
  int _nsums;

  int _nchannels;
  int _is_decoded;

  int array[256][32] = {0};

  int itrig_sums[24][16][40] = {0};

};


#endif /* __PACKET_IDLL1V1_H__ */
