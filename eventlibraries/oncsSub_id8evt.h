#ifndef __ONCSSUB_ID8EVT_H__
#define __ONCSSUB_ID8EVT_H__

#include "oncsSubevent.h"

// A plain list of signed 64-bit values (long long), the 8-byte sibling
// of ID2EVT and ID4EVT. The values are read with lValue(i); 
// iValue(0,"CHANNELS") is the number of values.

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_id8evt : public  oncsSubevent_w8 {
#else
class  oncsSub_id8evt : public  oncsSubevent_w8 {
#endif

public:
  oncsSub_id8evt( subevtdata_ptr);

  using oncsSubevent::iValue;
  int    iValue(const int, const char *);
  long long  lValue(const int);
  int    getArraylength(const char *what="");

protected:
  int getNrValues() const;
};


#endif /* __ONCSSUB_ID8EVT_H__ */
