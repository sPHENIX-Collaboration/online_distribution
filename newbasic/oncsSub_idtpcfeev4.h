#ifndef __ONCSSUB_IDTPCFEEV4_H__
#define __ONCSSUB_IDTPCFEEV4_H__

#include "oncsSubevent.h"
#include <vector>
#include <set>
#include <bitset>
#include <algorithm>
#include <functional>
#include <limits>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idtpcfeev4 : public  oncsSubevent_w2 {
#else
class  oncsSub_idtpcfeev4 : public  oncsSubevent_w2 {
#endif

public:
  oncsSub_idtpcfeev4( subevtdata_ptr);
  ~oncsSub_idtpcfeev4();

  //! SAMPA waveform interfaces
  int    iValue(const int ch, const int sample);
  int    iValue(const int ,const char * what);

  //! Expose the Level 1 trigger and endat taggers
  long long  lValue(const int channel, const char *what) ;

  void  dump ( OSTREAM& os = COUT) ;


protected:
  int tpc_decode();
  int tpc_gtm_decode();

  static const unsigned short  MAGIC_KEY_0 {0xfe};
//  static const unsigned short  MAGIC_KEY_1 = 0x00;
  static const unsigned short  MAGIC_KEY_1 {0xed};

  static const unsigned short FEE_MAGIC_KEY {0xba00};
  static const unsigned short GTM_MAGIC_KEY {0xbb00};
  static const unsigned short GTM_LVL1_ACCEPT_MAGIC_KEY {0xbbf0};
  static const unsigned short GTM_ENDAT_MAGIC_KEY {0xbbf1};
  static const unsigned short GTM_MODEBIT_MAGIC_KEY {0xbbf2};

  static const unsigned short  MAX_FEECOUNT {26};   // that many FEEs
  static const unsigned short  MAX_CHANNELS   {8*32}; // that many channels per FEE
//  static const unsigned short  HEADER_LENGTH  = 5;
  static const unsigned short  HEADER_LENGTH  {7};
  
  unsigned short reverseBits(const unsigned short x) const;
  unsigned short crc16(const unsigned int fee, const unsigned int index, const int  l) const;
  unsigned short check_data_parity(const unsigned int fee, const unsigned int index, const int  l) const;

  //  int find_header ( std::vector<unsigned short>::const_iterator &itr,  const std::vector<unsigned short> &orig);
  int find_header ( const unsigned int xx,  const std::vector<unsigned short> &orig);
  int decode_gtm_data(unsigned short gtm[16]);
  
//  int _broken;
  unsigned long NR_VALIDFEE = 0;
  
  int _is_decoded{0};
  int _is_gtm_decoded{0};

  struct sampa_waveform {
    unsigned short fee {std::numeric_limits<unsigned short>::max()};
    unsigned short pkt_length {std::numeric_limits<unsigned short>::max()};
    unsigned short channel {std::numeric_limits<unsigned short>::max()};
    unsigned short sampa_channel {std::numeric_limits<unsigned short>::max()};
    unsigned short sampa_address {std::numeric_limits<unsigned short>::max()};
    unsigned int bx_timestamp {0};
    std::vector<unsigned short> waveform;
    unsigned short adc_length {std::numeric_limits<unsigned short>::max()};
    unsigned short checksum {std::numeric_limits<unsigned short>::max()};
    unsigned short user_word {std::numeric_limits<unsigned short>::max()};
    unsigned short type {std::numeric_limits<unsigned short>::max()};
    unsigned short data_parity {std::numeric_limits<unsigned short>::max()};
    bool     valid {false};
    bool     parity_valid {false};
  };

  struct gtm_payload {
      unsigned short pkt_type;
      bool is_endat;
      bool is_lvl1;
      bool is_modebit;
      unsigned long long bco;
      unsigned int lvl1_count;
      unsigned int endat_count;
      unsigned long long last_bco;
      unsigned char modebits;
      unsigned char userbits;
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

  std::vector<sampa_waveform*> waveform_vector;
  
  int cacheIterator(const int n);
  
  int _last_requested_element {-1}; // impossible value to mark "unused"
  sampa_waveform* _last_requested_waveform {nullptr};
  
  std::vector<unsigned short> fee_data[MAX_FEECOUNT];

  std::vector<gtm_payload *> gtm_data;

};

 
#endif /* __ONCSSUB_IDTPCFEEV4_H__ */
