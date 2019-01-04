#ifndef __ONCSSUB_IDDREAMV0_H__
#define __ONCSSUB_IDDREAMV0_H__

#include "oncsSubevent.h"

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_iddreamv0 : public  oncsSubevent_w4 {
#else
class  oncsSub_iddreamv0 : public  oncsSubevent_w2 {
#endif

public:
  oncsSub_iddreamv0( subevtdata_ptr);
  ~oncsSub_iddreamv0();

  int    iValue(const int channel, const char *);
  int    iValue(const int channel, const int sample);   //treating the 8 dreams as 512 contiguous channels 
  int    iValue(const int dreamchip, const int sample, const char *);
  int    iValue(const int dreamchip, const int channel, const int sample);


  void  dump ( OSTREAM& os = COUT) ;

protected:
  
  int *decode (int *);
  int decode_payload ( unsigned short *d, const int size);
  int decode_dream( unsigned short *d, const int size, const unsigned int sample_nr);

  
  int samples [8][64][255];
  unsigned long long  cellids [8][255];

  int _nr_dreams;
  int _nr_samples;
  int _dream_enabled[8];

  int _is_decoded;

  unsigned short *swapped_array;
  
};


#endif /* __ONCSSUB_IDDREAMV0_H__ */
