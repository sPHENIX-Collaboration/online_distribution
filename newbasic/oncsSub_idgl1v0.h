#ifndef __ONCSSUB_IDGL1V0_H__
#define __ONCSSUB_IDGL1V0_H__

#include "oncsSubevent.h"

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idgl1v0 : public  oncsSubevent_w4 {
#else
class  oncsSub_idgl1v0 : public  oncsSubevent_w2 {
#endif

public:
  oncsSub_idgl1v0( subevtdata_ptr);

  int    iValue(const int dummy);

  long long  lValue(const int hit,const char * what);

  long long  lValue(const int channel, const int iy);

  
  void  dump ( OSTREAM& os = COUT);  

protected:
  int decode ();

  int is_decoded;
  
  unsigned long long BCO;
  unsigned long long BunchNumber;
  unsigned long long TriggerInput;
  unsigned long long TriggerVector;

  unsigned int GTM_BusyVector;
      
  unsigned int packet_nr;

  unsigned long long scalers[3*64];
  unsigned int gl1p_scalers[3*16];



};


#endif /* __ONCSSUB_IDGL1V0_H__ */
