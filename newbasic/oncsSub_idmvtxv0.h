#ifndef __ONCSSUB_IDMVTXV0_H__
#define __ONCSSUB_IDMVTXV0_H__

#include "oncsSubevent.h"
#include <vector>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv0 : public  oncsSubevent_w4 {
#else
class  oncsSub_idmvtxv0 : public  oncsSubevent_w4 {
#endif

#define MAXRUID       0
#define MAXRUCHN      4
#define MAXCHIPID     9
public:
  oncsSub_idmvtxv0( subevtdata_ptr);
  ~oncsSub_idmvtxv0();

  int    iValue(const int ich, const char *what);
  //int    iValue(const int RU, const int chip, const char *);
  int    iValue(const int chip, const int region, const int row);
  int    iValue(const int chip, const int row);
  
  void  dump ( OSTREAM& os = COUT) ;
  void  gdump ( const int how=EVT_HEXADECIMAL, OSTREAM& os = COUT) const; // add this to override the generic gdump

protected:
  int *decode ();

  int _is_decoded;
  int _unexpected_felix_counter;
  int _bad_ruid;
  int _bad_ruchns;
  int _bad_chipids;
  int _highest_row_overall;
  int _highest_chip;
  int _highest_region[32];
  int _excess_data_bytes;
  int _unexpected_bytes[32];
  int _bunchcounter[32];
  bool _header_found[32];
  bool _trailer_found[32];
  int _readout_flags[32];

  
  unsigned int bunchcounter;
  int chip_id;
  unsigned int region_id;
  unsigned int encoder_id;

  // this is one "row" of 32 pixels
  unsigned int chip_row[MAXCHIPID][512][32];
  unsigned int chip_rowmap[MAXCHIPID][512];

};

int decode_thebit (int the_row, int encoder_id, int address); //helper function to decode the column number


#endif /* __ONCSSUB_IDMVTXV0_H__ */
