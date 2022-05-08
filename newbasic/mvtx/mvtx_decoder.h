#ifndef __MVTX_DECODER__
#define __MVTX_DECODER__

#include <cstdint>  // for uint8_t...

namespace mvtx
{
  static const uint16_t DUMMY16 = 0xFFFF;

  static const uint8_t kBytesFLX = 32;
  static const uint8_t kBytesGBT = 10;
  static const uint8_t kGBTinFLX =  3;
  static const uint8_t kBitShiftRow = 16;

  typedef struct
  {
    uint8_t res3[23];
    uint8_t flxid;
    uint8_t res2;
    uint16_t count : 12, : 4;
    uint8_t res1;
    uint8_t gbtid : 5, : 3;
    uint8_t res0;
    uint16_t version;
  } flx_header_t;

  typedef struct
  {
    uint8_t gbt_word[kGBTinFLX][kBytesGBT];
    uint16_t count;
  } flx_word_t;

  uint32_t encode_hit(uint16_t row, uint16_t col);
  uint16_t get_row(uint32_t endoded_hit);
  uint16_t get_col(uint32_t endoded_hit);

  void mvtx_decoder(uint8_t *pos, uint8_t *the_end);

} /* namespace mvtx */

#endif /* __MVTX_DECODER__ */
