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

  enum DETECTORTYPE {
    dNONE = 0,
    dEMCAL = 1,
    dHCAL = 2,
    dMBD = 3
  };

  TRIGGERTYPE _trigger_type;
  DETECTORTYPE _detector_type;

  int _monitor;
  int _evt_nr;
  int _clock;

  int _slot_nr;
  int _card_nr;
  int _ntrigger_words;
  int _nsamples;

  int _nfibers;
  int _nsums;

  int _nchannels;
  int _is_decoded;

  int array[256][20] = {0};

  int itrig_sums[24][16][20] = {0};

  int itrig_emcal_8x8map[2][12][20] = {0};
  int itrig_emcal_2x2map[128][20] = {0};
  int itrig_emcal_2x2sum[64][20] = {0};
  int jet_map[32][12][20] = {0};
  int jet_sum_result[32][9][20] = {0};
};


#endif /* __PACKET_IDLL1V1_H__ */
