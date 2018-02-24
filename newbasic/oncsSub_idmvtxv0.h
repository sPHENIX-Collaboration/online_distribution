#ifndef __ONCSSUB_IDMVTXV0_H__
#define __ONCSSUB_IDMVTXV0_H__

#include "oncsSubevent.h"
#include <vector>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv0 : public  oncsSubevent_w4 {
#else
class  oncsSub_idmvtxv0 : public  oncsSubevent_w4 {
#endif

#define MAXRUCHN     9
public:
  oncsSub_idmvtxv0( subevtdata_ptr);
  ~oncsSub_idmvtxv0();

  int    iValue(const int ich, const char *what);
  //int    iValue(const int RU, const int chip, const char *);
  int    iValue(const int chip, const int region, const int row);
  
  void  dump ( OSTREAM& os = COUT) ;
  void  gdump ( const int how=EVT_HEXADECIMAL, OSTREAM& os = COUT) const; // add this to override the generic gdump

protected:
  int *decode ();

  int _highest_row_overall;
  int _is_decoded;
  int _highest_chip;
  int _highest_region[32];
  
  unsigned int bunchcounter;
  unsigned int chip_id;
  unsigned int region_id;
  unsigned int encoder_id;

  // this is one "row" of 32 pixels
  unsigned int chip_row[9][512][32];

};


#endif /* __ONCSSUB_IDMVTXV0_H__ */
