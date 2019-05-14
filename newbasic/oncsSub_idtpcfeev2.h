#ifndef __ONCSSUB_IDTPCFEEV2_H__
#define __ONCSSUB_IDTPCFEEV2_H__

#include "oncsSubevent.h"
#include "vector"

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idtpcfeev2 : public  oncsSubevent_w4 {
#else
class  oncsSub_idtpcfeev2 : public  oncsSubevent_w4 {
#endif

public:
  oncsSub_idtpcfeev2( subevtdata_ptr);
  ~oncsSub_idtpcfeev2();

  int    iValue(const int fee, const int ch, const int sample);
  int    iValue(const int ch, const int sample);
  int    iValue(const int ,const char * what);

  void  dump ( OSTREAM& os = COUT) ;


protected:
  int decode ();

#define MAX_FEECOUNT 8
#define MAX_FEECHANNELS 256

  
  unsigned short reverseBits(const unsigned short x) const;
  //  unsigned short crc16(unsigned short *mem, const int  len) const;
  unsigned short crc16(const unsigned int fee, const unsigned int index, const int  l) const;
  int prune_bx(); 

  
  int _broken;
  
  int _is_decoded;

  typedef struct {
    unsigned int index;
    unsigned int bx_count;
  } bx;

  unsigned int _max_samples[MAX_FEECOUNT];

  std::vector<unsigned short> *fee_data[MAX_FEECOUNT];
  
  std::vector<bx> fee_bx[MAX_FEECOUNT][MAX_FEECHANNELS]; 
  std::vector<unsigned short> fee_samples[MAX_FEECOUNT][MAX_FEECHANNELS];  

  };


#endif /* __ONCSSUB_IDTPCFEEV2_H__ */
