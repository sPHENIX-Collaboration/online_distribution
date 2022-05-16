#ifndef __ONCSSUB_IDMVTXV3_H__
#define __ONCSSUB_IDMVTXV3_H__

#include "oncsSubevent.h"

#include <utility> // std::pair, std::make_pair

using packet_range_t = std::pair<uint8_t *, uint8_t *>;

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv3 : public  oncsSubevent_w4 {
#else
    class  oncsSub_idmvtxv3 : public  oncsSubevent_w4 {
#endif

 public:
  explicit oncsSub_idmvtxv3(subevtdata_ptr);
  ~oncsSub_idmvtxv3() = default;

#ifndef _MVTX_DECODER_ // define public dummy inline methods
  void dump(OSTREAM &os = COUT) override {};
  void gdump(const int how=EVT_HEXADECIMAL, OSTREAM &os = COUT) const override {};
#else
  void dump(OSTREAM &os = COUT) override;
  void gdump(const int how=EVT_HEXADECIMAL, OSTREAM &os = COUT) const override;
#endif

 protected:
  void print_stuff(
    OSTREAM &out, uint32_t data, uint8_t width /*bytes*/, uint8_t shift = 0) const;

#ifndef _MVTX_DECODER_  // define private dummy inlide methods
  packet_range_t get_packet_range() const { return std::make_pair(nullptr, nullptr); }
  int decode() { return 0; }
#else
  packet_range_t get_packet_range() const;
  int decode();
#endif

  bool m_is_decoded;
};

#endif /* __ONCSSUB_IDMVTXV3_H__ */
