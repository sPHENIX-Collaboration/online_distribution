#ifndef __ONCSSUB_ID4EVT_H__
#define __ONCSSUB_ID4EVT_H__

#include "oncsSubevent.h"

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_id4evt : public  oncsSubevent_w4 {
#else
class  oncsSub_id4evt : public  oncsSubevent_w4 {
#endif

public:
  oncsSub_id4evt( subevtdata_ptr);

  // iValue(0,"CHANNELS") is the number of values ("channels" of an ADC);
  // any other "what" gives value ch, as before
  using oncsSubevent::iValue;
  int iValue(const int ch, const char *what);

protected:
  int *decode (int *);
};


#endif /* __ONCSSUB_ID4EVT_H__ */
