#ifndef __ONCSSUB_IDDREAMV0_H__
#define __ONCSSUB_IDDREAMV0_H__

#include "oncsSubevent.h"
#include "vector"
#include "map"

#define MAX_FEU 8


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
  
  struct FEU_decoded_data
  {
    int _feu_id;
    int _feu_P;  // pedestal subtraction on 
    int _feu_C;  // common noise suppression
    int _feu_Z;  // zero-suppressed format

    int _nr_dreams;
    int _nr_samples;
    int _dream_enabled[8];
    int samples [8][64][255];
    unsigned long long  cellids [8][255];
    //  std::vector<short> feu_rawdata;
  };
  



  int *decode ( int *);
  //  int decode_payload (FEU_decoded_data *fd);
  int decode_payload ( unsigned short *d, const int size);
  int decode_dream( FEU_decoded_data *fd, unsigned short *d, const int size, const unsigned int sample_nr);
  int decode_dream( FEU_decoded_data *fd, const int offset, const int size, const unsigned int sample_nr);

  std::map<int,struct FEU_decoded_data *> feu_map; 


    
  int _nr_feus;

  int _is_decoded;


  
  unsigned short *swapped_array;
  
};


#endif /* __ONCSSUB_IDDREAMV0_H__ */
