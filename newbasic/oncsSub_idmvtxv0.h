#ifndef __ONCSSUB_IDMVTXV0_H__
#define __ONCSSUB_IDMVTXV0_H__

#include "oncsSubevent.h"
#include <vector>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv0 : public  oncsSubevent_w4 {
#else
class  oncsSub_idmvtxv0 : public  oncsSubevent_w4 {
#endif

#define MAXRUID       1
#define RUHEADER      0xE0
#define RUTRAILER     0xF0
#define MAXRUCHN      28
#define MAXCHIPID     9
public:
  oncsSub_idmvtxv0( subevtdata_ptr);
  ~oncsSub_idmvtxv0();

  //ichip, what -> event or chip info
  //ichip, region, row -> hit mask (indicates pixels with hits in given 32-pix block)
  //ichip, row -> row map (indicates 32-pix blocks with hits in given row)
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
  int _excess_data_bytes;
  int _chip_id[MAXRUCHN+1];
  int _highest_region[MAXRUCHN+1];
  int _unexpected_bytes[MAXRUCHN+1];
  int _bunchcounter[MAXRUCHN+1];
  bool _header_found[MAXRUCHN+1];
  bool _trailer_found[MAXRUCHN+1];
  int _readout_flags[MAXRUCHN+1];

  // this is one "row" of 32 pixels
  unsigned int chip_row[MAXRUCHN+1][512][32];
  unsigned int chip_rowmap[MAXRUCHN+1][512];

};

int decode_thebit (int the_row, int encoder_id, int address); //helper function to decode the column number
void print_stuff(OSTREAM& out, unsigned int data, int width, int shift, bool blank = false);


#endif /* __ONCSSUB_IDMVTXV0_H__ */
