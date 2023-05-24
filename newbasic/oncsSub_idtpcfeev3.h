#ifndef __ONCSSUB_IDTPCFEEV3_H__
#define __ONCSSUB_IDTPCFEEV3_H__

#include "oncsSubevent.h"
#include <vector>
#include <set>
#include <algorithm>
#include <functional>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idtpcfeev3 : public  oncsSubevent_w2 {
#else
class  oncsSub_idtpcfeev3 : public  oncsSubevent_w2 {
#endif

public:
  oncsSub_idtpcfeev3( subevtdata_ptr);
  ~oncsSub_idtpcfeev3();

  int    iValue(const int ch, const int sample);
  int    iValue(const int ,const char * what);

  void  dump ( OSTREAM& os = COUT) ;


protected:
  int tpc_decode ();

  static const unsigned short  MAGIC_KEY_0 = 0xfe;
  static const unsigned short  MAGIC_KEY_1 = 0x00;

  static const uint16_t FEE_MAGIC_KEY = 0xba00;
  static const uint16_t GTM_MAGIC_KEY = 0xbb00;
  static const uint16_t GTM_LVL1_ACCEPT_MAGIC_KEY = 0xbbf0;
  static const uint16_t GTM_ENDAT_MAGIC_KEY = 0xbbf1;

  static const unsigned short  MAX_FEECOUNT = 26;   // that many FEEs
  static const unsigned short  MAX_CHANNELS   = 8*32; // that many channels per FEE
  static const unsigned short  HEADER_LENGTH  = 7;
  
  unsigned short reverseBits(const unsigned short x) const;
  unsigned short crc16(const unsigned int fee, const unsigned int index, const int  l) const;

  //  int find_header ( std::vector<unsigned short>::const_iterator &itr,  const std::vector<unsigned short> &orig);
  int find_header ( const unsigned int xx,  const std::vector<unsigned short> &orig);
  int decode_gtm_data(uint16_t gtm[16]);
  
  int _broken;
  
  int _is_decoded;

  struct sampa_waveform {
    uint16_t fee;
    uint16_t pkt_length;
    uint16_t channel;
    uint16_t sampa_channel;
    uint16_t sampa_address;
    uint32_t bx_timestamp;
    std::vector<uint16_t> waveform;
    uint16_t adc_length;
    uint16_t checksum;
    bool     valid;
  };

  struct gtm_payload {
      uint16_t pkt_type;
      bool is_endat;
      bool is_lvl1;
      uint64_t bco;
      uint32_t lvl1_count;
      uint32_t endat_count;
      uint64_t last_bco;
      uint8_t modebits;
  };
  
  // once vector per possible channel 16 cards * 256 channels
  //std::vector<sampa_waveform *> waveform_vector[MAX_FEECOUNT * MAX_CHANNELS];

  // our sort functional

struct bco_compare {
    bool operator() (const sampa_waveform *lhs, const sampa_waveform *rhs) const
    {
      return  ( lhs->bx_timestamp <= rhs->bx_timestamp );
    }
};


  
  typedef std::multiset< sampa_waveform* , bco_compare> waveform_set;
  //typedef waveform_set::iterator wf_iter;
  
  waveform_set waveforms;
  //  waveform_set waveforms[MAX_FEECOUNT * MAX_CHANNELS];

  int cacheIterator(const int n);
  
  waveform_set::iterator _cached_iter;
  int _last_requested_element;
  
  std::vector<unsigned short> fee_data[MAX_FEECOUNT];

  std::vector<gtm_payload *> gtm_data;

};

 
#endif /* __ONCSSUB_IDTPCFEEV3_H__ */
