#ifndef __ONCSSUB_IDMVTXV3_H__
#define __ONCSSUB_IDMVTXV3_H__

#include "oncsSubevent.h"

#include "mvtx_decoder/PayLoadCont.h"
#include "mvtx_decoder/GBTLink.h"

#include "mvtx_decoder/mvtx_utils.h"

#include <map>
#include <set>
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

  int iValue(const int ,const char * what) final;
  int iValue(const int, const int, const char* what) final;
  int iValue(const int, const int, const int, const char* what) final;

  long long int lValue(const int, const int, const char* what) final;

  void * pValue(const int) final;

  void dump(OSTREAM &os = COUT) final;

 protected:

  int decode();
  bool m_is_decoded;


  struct dumpEntry
  {
    int entry = -1;
  };

  void loadInput(mvtx::PayLoadCont& buf);
  void setupLinks(mvtx::PayLoadCont&);

  static size_t mEventId;
  static std::unordered_map<uint32_t, dumpEntry> mSubId2Buffers; // link sub_id to data buffer in the pool mapping
  static std::vector< mvtx::PayLoadCont > mBuffers;
  static std::unordered_map<uint16_t, dumpEntry> mFeeId2LinkID; // link fee_id to GBTLinks
  static std::vector<mvtx::GBTLink> mGBTLinks;

  std::set<uint16_t> feeid_set;
  uint8_t *payload;
  unsigned int payload_position;

 };

#endif /* __ONCSSUB_IDMVTXV3_H__ */
