#ifndef __ONCSSUB_IDMVTXV3_H__
#define __ONCSSUB_IDMVTXV3_H__

#include "oncsSubevent.h"

#include "mvtx_decoder/PayLoadCont.h"
#include "mvtx_decoder/GBTLink.h"

#include "mvtx_decoder/mvtx_utils.h"

#include <map>
#include <vector>
#include <bitset>
#include <stdint.h>
#include <unordered_map>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv3 : public  oncsSubevent_w4 {
#else
class  oncsSub_idmvtxv3 : public  oncsSubevent_w4 {
#endif

 public:
  explicit oncsSub_idmvtxv3(subevtdata_ptr);
  ~oncsSub_idmvtxv3();

  int    iValue(const int ,const char * what);
  long long    lValue(const int ,const char * what);


  void dump(OSTREAM &os = COUT);

 protected:

  int decode();
  int read_32( std::bitset<80>* gbtwords
               , uint16_t &w16 );

  bool m_is_decoded;


  struct dumpEntry
  {
    int      entry = -1;
  };

  void loadInput(mvtx::PayLoadCont& buf);
  void setupLinks(mvtx::PayLoadCont&);

  static size_t mEventId;
  static std::unordered_map<uint32_t, dumpEntry> mSubId2Buffers; // link sub_id to data buffer in the pool mapping
  static std::vector< mvtx::PayLoadCont > mBuffers;
  static std::unordered_map<uint16_t, dumpEntry> mFeeId2LinkID; // link fee_id to GBTLinks
  static std::vector<mvtx::GBTLink> mGBTLinks;

  unsigned long get_GBT_value( const std::bitset<80> gbtword, const int pos, const int size) const;
  unsigned long long get_GBT_lvalue( const std::bitset<80> gbtword, const int pos, const int size) const;
  int  decode_chipdata( const std::bitset<80> gbtword);

  void  pretty_print( const std::bitset<80> gbtword) const;

  int  decode_lane( const std::vector<uint8_t> v);

  uint8_t *payload;
  unsigned int payload_position;

  // the per-lane keeper of the chip data
  // lane, vector
  std::map<unsigned int, std::vector<unsigned char> > chipdata;

  std::vector< std::vector<std::bitset<80> > > gbtvector;

  unsigned long long last_BCO;
  unsigned long long last_LHCBC;
  unsigned short last_source_id;
  unsigned short last_fee_id;
  unsigned short last_lane;


  struct mvtx_hit
  {
    unsigned long long RHICBCO;
    unsigned long long LHCBC;
    // take out    unsigned int source_id;

    unsigned int chip_id;

    unsigned int fee_id;
    unsigned int lane;
    unsigned int encoder_id;
    unsigned int addr;
    unsigned int source_id;
    // unsigned int row_pos;
    // unsigned int col_pos;
    // unsigned int L1Trigger;

    // unsigned int chipid;
    // unsigned int region_id;
    // unsigned int readout_flags;
    // unsigned int bunchcounter;
  };

  std::vector<mvtx_hit *> hit_vector;
};

#endif /* __ONCSSUB_IDMVTXV3_H__ */
