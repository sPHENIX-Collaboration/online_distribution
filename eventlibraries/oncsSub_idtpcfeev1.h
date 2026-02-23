#ifndef __ONCSSUB_IDTPCFEEV1_H__
#define __ONCSSUB_IDTPCFEEV1_H__

#include "oncsSubevent.h"

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idtpcfeev1 : public  oncsSubevent_w4 {
#else
class  oncsSub_idtpcfeev1 : public  oncsSubevent_w4 {
#endif

public:
  oncsSub_idtpcfeev1( subevtdata_ptr);
  ~oncsSub_idtpcfeev1();

  int    iValue(const int chip, const int ch, const int sample);
  int    iValue(const int ch, const int sample);
  int    iValue(const int ,const char * what);

  void  dump ( OSTREAM& os = COUT) ;


protected:
  int decode ();

  int _broken;
  
  unsigned int _is_decoded;
  unsigned int _nchips;
  unsigned int _nchannels;
  unsigned int _nsamples;
  unsigned int _bx_count;
  
  unsigned int array[64][1002]; 

};


#endif /* __ONCSSUB_IDTPCFEEV1_H__ */
