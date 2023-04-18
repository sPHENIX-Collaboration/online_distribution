#ifndef __ONCSSUB_IDMVTXV3_H__
#define __ONCSSUB_IDMVTXV3_H__

#include "oncsSubevent.h"

#include <map>
#include <vector>
#include <bitset>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv3 : public  oncsSubevent_w4 {
#else
class  oncsSub_idmvtxv3 : public  oncsSubevent_w4 {
#endif

 public:
  explicit oncsSub_idmvtxv3(subevtdata_ptr);
  ~oncsSub_idmvtxv3();

  int    iValue(const int ,const char * what);


  
  void dump(OSTREAM &os = COUT) override;


 protected:

  int decode();
  int read_32( std::bitset<80> &gbtword0
	       , std::bitset<80> &gbtword1
	       , std::bitset<80> &gbtword2
	       , unsigned short &w0
	       , unsigned short &w1 );

  
  bool m_is_decoded;

  unsigned long get_GBT_value( const std::bitset<80> gbtword, const int pos, const int size) const;
  unsigned long long get_GBT_lvalue( const std::bitset<80> gbtword, const int pos, const int size) const;
  int  decode_chipdata( const std::bitset<80> gbtword);

  void  pretty_print( const std::bitset<80> gbtword) const;

  int  decode_lane( const std::vector<uint8_t> v);

  uint8_t *payload;
  unsigned int payload_position;

  // the per-lane keeper of the chip data
  // lane, vector 
  std::map<unsigned int, std::vector<unsigned char>> chipdata;
  
  std::vector<std::bitset<80>> gbtvector;

  
  uint64_t last_BCO;
  unsigned short last_source_id;
  unsigned short last_fee_id;
  unsigned short last_lane;
  
  
  struct mvtx_hit
  {
    unsigned long long BCO;
    unsigned int source_id;
    unsigned int fee_id;
    unsigned int lane;
    unsigned int encoder_id;
    unsigned int addr;

    // unsigned int chipid;
    // unsigned int region_id;
    // unsigned int readout_flags;
    // unsigned int bunchcounter;
  };
    
  std::vector<mvtx_hit *> hit_vector;

  
  enum WordTypeMarker
    {
     IHW = 0xe0,
     TDH = 0xe8,
     TDT = 0xf0,
     DDW = 0x40,
     CDW = 0xf8
    };
     
  
  enum Rdh8ByteMapW0
    {
     HDRVERSION     = 0 *8,
     SIZE           = 1 *8,
     FEEID_LSB      = 2 *8,
     FEEID_MSB      = 3 *8,
     SOURCE_ID      = 4 *8,
     DET_FIELD_SB0  = 5 *8,
     DET_FIELD_SB1  = 6 *8,
     DET_FIELD_SB2  = 7 *8,
     DET_FIELD_SB3  = 8 *8,
     RESERVED_0_LSB = 9 *8
    };
  
  enum Rdh8ByteMapW1
    {
     BC_LSB         = 0 *8,
     BC_MSB         = 1 *8,
     RESERVED_1_LSB = 2 *8,
     RESERVED_1_MSB = 3 *8,
     BCO_SB0        = 4 *8,
     BCO_SB1        = 5 *8,
     BCO_SB2        = 6 *8,
     BCO_SB3        = 7 *8,
     BCO_SB4        = 8 *8,
     RESERVED_2_SB0 = 9 *8
    };

  enum Rdh8ByteMapW2
    {
     TRG_TYPE_SB0    = 0 * 8,
     TRG_TYPE_SB1    = 1 * 8,
     TRG_TYPE_SB2    = 2 * 8,
     TRG_TYPE_SB3    = 3 * 8,
     PAGE_CNT_LSB    = 4 * 8,
     PAGE_CNT_MSB    = 5 * 8,
     STOP_BIT        = 6 * 8,
     PRIORITY        = 7 * 8,
     RESERVED_3_SB0  = 8 * 8,
     RESERVED_3_SB1  = 9 * 8
    };
  
     
  enum flxhdr_word2
    {
     RESERVED_0_SB20  = 0 * 8,
     RESERVED_0_SB21  = 1 * 8,
     RESERVED_0_SB22  = 2 * 8,
     FLXID            = 3 * 8,
     RESERVED_1       = 4 * 8,
     DMA_WRD_CTR_LSB  = 5 * 8,
     DMA_WRD_CTR_MSB  = 6 * 8,
     RESERVED_2       = 7 * 8,
     GBT_ID           = 8 * 8,
     RESERVED_3       = 9 * 8
    };
    


  
};

#endif /* __ONCSSUB_IDMVTXV3_H__ */
