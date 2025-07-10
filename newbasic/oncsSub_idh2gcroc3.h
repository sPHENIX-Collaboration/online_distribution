#ifndef __ONCSSUB_IDH2GCROC3_H__
#define __ONCSSUB_IDH2GCROC3_H__

#include "oncsSubevent.h"

#include<vector>


#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idh2gcroc3 : public  oncsSubevent_w4 {
#else
class  oncsSub_idh2gcroc3 : public  oncsSubevent_w2 {
#endif

public:
  oncsSub_idh2gcroc3( subevtdata_ptr);
  ~oncsSub_idh2gcroc3();

  int    iValue(const int ch, const int sample);
  int    iValue(const int ch, const int sample, const char *what);
  int    iValue(const int ,const char * what);

  void  dump ( OSTREAM& os = COUT);  

protected:
  int decode ();
  int decode_line (unsigned int *);
  unsigned int u4swap(const unsigned int v);

  int is_decoded;
  
  int _broken;


  unsigned int old_timestamp;
  
  struct sample {
    unsigned int ADC[144];
    unsigned int tot[144];
    unsigned int toa[144];
    unsigned int timestamp;
    unsigned int cmn;
    unsigned int calib;
    unsigned int crc32;
  } ;

  sample *_sample;
  
  std::vector<sample *> waveform;

  int nr_samples;
};


#endif /* __ONCSSUB_IDH2GCROC3_H__ */
