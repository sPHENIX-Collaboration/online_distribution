#ifndef __ONCSSUB_IDMVTXV3_H__
#define __ONCSSUB_IDMVTXV3_H__

#include "oncsSubevent.h"

#include "mvtx_decoder/PayLoadCont.h"
#include "mvtx_decoder/GBTLink.h"

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


  struct BufferEntry
  {
    int      entry = -1;
  };

  void loadInput(BufferEntry& bufEntry);
  void setupLinks(mvtx::PayLoadCont&);

  static size_t mEventId;
  static std::unordered_map<uint32_t, BufferEntry> mSubId2Buffers; // link sub_id to data buffer in the pool mapping
  static std::vector< mvtx::PayLoadCont > mBuffers;
  static std::unordered_map<uint16_t, BufferEntry> mFeeId2GBTLink; // link fee_id to GBTLinks
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

namespace mvtx_utils
{

#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_error(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)
#define ASSERT(A, D, M,...) if(!(A)) { log_error(M, ##__VA_ARGS__); assert(A); }

  enum WordTypeMarker
    {
     IHW = 0xe0,
     TDH = 0xe8,
     TDT = 0xf0,
     DDW = 0x40,
     CDW = 0xf8
    };

  struct RdhExt_t
  {
    // FLX header
    uint8_t flxId;    // [23]
    uint16_t pageSize; // [25]
    uint16_t gbtLink;
    uint16_t flxHdrSize;
    uint16_t flxHdrVersion;
    // RU header
    uint8_t rdhVersion;
    uint8_t rdhSize;
    uint16_t feeId;
    uint8_t sourceId;
    uint32_t detectorField;
    uint16_t bc;
    uint64_t orbit;
    uint32_t trgType;
    uint16_t packetCounter;
    uint8_t  stopBit;
    uint8_t  priority;
    uint16_t rdhGBTcounter; // 10 bits

    RdhExt_t() = default;
    ~RdhExt_t() = default;

    void decode(uint8_t* rdh_ptr)
    {
     // FELIX header
      flxId         = *(reinterpret_cast<uint8_t*>(rdh_ptr +  23) ) & 0xFF;
      pageSize      = *(reinterpret_cast<uint16_t*>(rdh_ptr + 25) ) & 0x7FF;
      gbtLink       = *(reinterpret_cast<uint16_t*>(rdh_ptr + 28) ) & 0x7FF;
      flxHdrSize    = *(reinterpret_cast<uint16_t*>(rdh_ptr + 29) ) & 0x7FF;
      flxHdrVersion = *(reinterpret_cast<uint16_t*>(rdh_ptr + 30) ) & 0xFFFF;
      // RU header
      rdhVersion    = *(reinterpret_cast<uint8_t*>(rdh_ptr + 32) ) & 0xFF;
      rdhSize       = *(reinterpret_cast<uint8_t*>(rdh_ptr + 33) ) & 0xFF;
      feeId         = *(reinterpret_cast<uint16_t*>(rdh_ptr + 34) ) & 0xFFFF;
      sourceId      = *(reinterpret_cast<uint8_t*>(rdh_ptr + 36) ) & 0xFF;
      detectorField = *(reinterpret_cast<uint32_t*>(rdh_ptr + 37) ) & 0xFFFFFFFF;
      bc            = *(reinterpret_cast<uint16_t*>(rdh_ptr + 42) ) & 0xFFF;
      orbit         = *(reinterpret_cast<uint64_t*>(rdh_ptr + 46) ) & 0xFFFFFFFFFF;
      trgType       = *(reinterpret_cast<uint32_t*>(rdh_ptr + 52) ) & 0xFFFFFFFF;
      packetCounter = *(reinterpret_cast<uint16_t*>(rdh_ptr + 56) ) & 0xFFFF;
      stopBit       = *(reinterpret_cast<uint8_t*>(rdh_ptr + 58) ) & 0xFF;
      priority      = *(reinterpret_cast<uint8_t*>(rdh_ptr + 59) ) & 0xFF;
      rdhGBTcounter = *(reinterpret_cast<uint16_t*>(rdh_ptr + 62) ) & 0xFFFF;
    }
  };
}
#endif /* __ONCSSUB_IDMVTXV3_H__ */
