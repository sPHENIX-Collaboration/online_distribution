#include "oncsSub_idmvtxv3.h"

// for memset
#include <string.h>
#include <cassert>

using namespace std;

// define static references
size_t oncsSub_idmvtxv3::mEventId = 0;
std::unordered_map<uint32_t, oncsSub_idmvtxv3::dumpEntry> oncsSub_idmvtxv3::mSubId2Buffers = {};
std::vector< mvtx::PayLoadCont> oncsSub_idmvtxv3::mBuffers = {};
std::unordered_map<uint16_t, oncsSub_idmvtxv3::dumpEntry> oncsSub_idmvtxv3::mFeeId2LinkID = {};
std::vector< mvtx::GBTLink> oncsSub_idmvtxv3::mGBTLinks = {};

oncsSub_idmvtxv3::oncsSub_idmvtxv3(subevtdata_ptr data)
  : oncsSubevent_w4(data)
{
  m_is_decoded = false;
  payload = nullptr;
  payload_position = 0;
}

//_________________________________________________
int oncsSub_idmvtxv3::decode()
{
  if ( m_is_decoded )
  {
    return 0;
  }
  m_is_decoded = true;

  feeid_set.clear();

  for (auto& link : mGBTLinks)
  {
    link.clear(false, true); // clear data but not the statistics
  }

  short pck_id = getIdentifier();

  auto& bufref = mSubId2Buffers[pck_id];
  if ( bufref.entry == -1 )
  {
    bufref.entry = mBuffers.size();
    mBuffers.push_back( {} );
  }
  auto& buffer = mBuffers[bufref.entry];
  loadInput(buffer);
  setupLinks(buffer);

  buffer.movePtr(payload_position);

  if ( buffer.isEmpty() )
  {
    buffer.clear();
  }
  else
  {
    buffer.moveUnusedToHead();
  }

  for ( auto& link : mGBTLinks )
  {
    link.collectROFCableData();
  }

  ++mEventId;
  return 0;
}


void oncsSub_idmvtxv3::loadInput(mvtx::PayLoadCont& buffer)
{
  uint8_t* payload_start = (uint8_t *) &SubeventHdr->data;  // here begins the payload
  unsigned int dlength = getDataLength() - getPadding(); //padding is supposed to be in units of dwords, this assumes dwords
  dlength *= 4;

  if ( dlength % mvtx_utils::FLXWordLength )
  {
    dlength -= dlength % mvtx_utils::FLXWordLength;
    COUT
      << ENDL
      << "!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!! \n"
      << "DMA packet has incomplete FLX words, only "
      << dlength << " bytes(" << (dlength / mvtx_utils::FLXWordLength)
      << " FLX words), will be decoded. \n"
      << "!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!! \n"
      << ENDL;
  }

  buffer.add(payload_start, dlength);
  payload = buffer.getPtr();
  payload_position = 0;

  return;
}


void oncsSub_idmvtxv3::setupLinks(mvtx::PayLoadCont& buf)
{
  mvtx_utils::RdhExt_t rdh = {};
  size_t dlength = buf.getUnusedSize();
  do
  {
    // Skip FLX padding
    if ( *(reinterpret_cast<uint16_t*>(&payload[payload_position] + 30)) == 0xFFFF )
    {
      while ( (*(reinterpret_cast<uint16_t*>(&payload[payload_position] + 30)) == 0xFFFF) &&\
             payload_position < dlength)
      {
        payload_position += mvtx_utils::FLXWordLength;
      }
    }
    else if ( (dlength - payload_position) >= 2 * mvtx_utils::FLXWordLength ) // at least FLX header and RDH
    {
      if ( *(reinterpret_cast<uint16_t*>(&payload[payload_position] + 30)) == 0xAB01 )
      {
        rdh.decode(&payload[payload_position]);
        const size_t pageSizeInBytes = (rdh.pageSize + 1) * mvtx_utils::FLXWordLength;
        if ( pageSizeInBytes > (dlength - payload_position) )
        {
          break; // skip incomplete felix packet
        }
        else
        {
          feeid_set.insert(rdh.feeId);
          auto& lnkref = mFeeId2LinkID[rdh.feeId];
          if ( lnkref.entry == -1 )
          {
            lnkref.entry = mGBTLinks.size();
            mGBTLinks.emplace_back(rdh.flxId, rdh.feeId);
          }
          auto& gbtLink = mGBTLinks[lnkref.entry];

          if ( (rdh.packetCounter) && (rdh.packetCounter != gbtLink.prev_pck_cnt + 1) )
          {
            log_error << "Incorrect pages count " << rdh.packetCounter <<", previous page count was " \
              << gbtLink.prev_pck_cnt << std::endl;
            payload_position += pageSizeInBytes;
            continue;
          }
          gbtLink.prev_pck_cnt = rdh.packetCounter;

          gbtLink.data.add((payload + payload_position), pageSizeInBytes);

          if ( ! rdh.packetCounter ) // start HB
          {
            if ( gbtLink.hbf_found )
            {
              log_error << "FLX: " << gbtLink.flxId << ", FeeId: " << gbtLink.feeId \
                << ". Found new HBF before stop previous HBF. Previous HBF will be ignored." << std::endl;
              gbtLink.cacheData(gbtLink.hbf_length, true);
            }
            gbtLink.hbf_found = true;
            gbtLink.hbf_length = pageSizeInBytes;
          }
          else
          {
            gbtLink.hbf_length += pageSizeInBytes;
          }

          if ( rdh.stopBit ) // found HB end
          {
            if ( ! gbtLink.hbf_found )
            {
              log_error << "FLX: " << gbtLink.flxId << ", FeeId: " << gbtLink.feeId \
                << ". Stopping HBF without start. This block will be ignored." << std::endl;
              gbtLink.cacheData(gbtLink.hbf_length, true);
            }
            gbtLink.hbf_found = false;
            gbtLink.cacheData(gbtLink.hbf_length, false);
          }
          payload_position += pageSizeInBytes;
        }
      }
      else
      {
        // skip raw data without a initial FLX header
        // (YCM)TODO: OK for OM but error otherwise
        if ( 0 )
        {
          std::cout << "Felix header: " << std::hex << "0x" << std::setfill('0') << std::setw(4);
          std::cout << *(reinterpret_cast<uint16_t*>(&payload[payload_position] + 30)) << std::dec <<std::endl;
        }
        payload_position += mvtx_utils::FLXWordLength;
      }
    }
    else
    {
      break; // skip incomplete flx_header
    }
  } while (payload_position < dlength);

  return;
}


int oncsSub_idmvtxv3::iValue(const int n, const char *what)
{
  decode();
  if ( n == -1 ) // Global Information.
  {
    if ( strcmp(what, "NR_LINKS") == 0)
    {
      return feeid_set.size();
    }
    else
    {
      std::cout << "Unknow option " << what << std::endl;
      return -1;
    }
  }

  unsigned int i = n;
  if ( strcmp(what, "FEEID") == 0 )
  {
    return (i < feeid_set.size()) ? *(next(feeid_set.begin(), i)) : -1;
  }
  else
  {
    if (mFeeId2LinkID.find(i) == mFeeId2LinkID.cend())
    {
      log_error << "FeeId " << i << " was not found in the feeId mapping for this packet" << std::endl;
      assert(false);
    }
    uint32_t lnkId =  mFeeId2LinkID[i].entry;
    if ( strcmp(what, "NR_HBF") == 0 )
    {
      if ( mGBTLinks[lnkId].rawData.getNPieces() != mGBTLinks[lnkId].hbf_count)
      {
        log_error << "Mismatch size for HBF from hbfData: " << mGBTLinks[lnkId].hbf_count << " and link rawData Pieces: " \
          << mGBTLinks[lnkId].rawData.getNPieces() << std::endl;
        assert(false);
      }
      return mGBTLinks[lnkId].hbf_count;
    }
    else if ( strcmp(what, "NR_PHYS_TRG") == 0 )
    {
      return mGBTLinks[lnkId].mL1TrgTime.size();
    }
    else if ( strcmp(what, "NR_STROBES") == 0 )
    {
    return mGBTLinks[lnkId].mTrgData.size();
    }
    else if ( strcmp(what, "NR_HITS") == 0 )  // the number of datasets
    {
      return mGBTLinks[lnkId].hit_vector.size();
    }
    else
    {
      std::cout << "Unknow option " << what << std::endl;
      return -1;
    }
  }
  return 0;
}

int oncsSub_idmvtxv3::iValue(const int i_feeid, const int idx, const char *what)
{
  decode();
  uint32_t feeId = i_feeid;
  uint32_t index = idx;

  if (mFeeId2LinkID.find(feeId) == mFeeId2LinkID.cend())
  {
    log_error << "FeeId " << feeId << " was not found in the feeId mapping for this packet" << std::endl;
    assert(false);
  }
  uint32_t lnkId =  mFeeId2LinkID[feeId].entry;

  if ( strcmp(what, "L1_IR_BC") == 0 )
  {
    return (index < mGBTLinks[lnkId].mL1TrgTime.size()) ? mGBTLinks[lnkId].mL1TrgTime[index].bc : -1;
  }
  else if ( strcmp(what, "TRG_IR_BC") == 0 )
  {
    return (index < mGBTLinks[lnkId].mTrgData.size()) ? mGBTLinks[lnkId].mTrgData[index].ir.bc : -1;
  }
  else if ( strcmp(what, "TRG_NR_HITS") == 0)
  {
    return (index < mGBTLinks[lnkId].mTrgData.size()) ? mGBTLinks[lnkId].mTrgData[index].n_hits : -1;
  }
  else
  {
    std::cout << "Unknow option " << what << std::endl;
    return -1;
  }
  return 0;
}

int oncsSub_idmvtxv3::iValue(const int i_feeid, const int i_trg, const int i_hit, const char *what)
{
  decode();

  uint32_t feeId = i_feeid;
  uint32_t trg = i_trg;
  uint32_t hit = i_hit;

  if (mFeeId2LinkID.find(feeId) == mFeeId2LinkID.cend())
  {
    log_error << "FeeId " << feeId << "was not found in the feeId mapping for this packet" << std::endl;
    assert(false);
  }
  uint32_t lnkId =  mFeeId2LinkID[feeId].entry;

  uint32_t hit_global_id = mGBTLinks[lnkId].mTrgData[trg].first_hit_pos + hit;

  if ( strcmp(what, "HIT_CHIP_ID") == 0 )
  {
    return ( (hit >= 0) && (hit < mGBTLinks[lnkId].mTrgData[trg].n_hits) ) ? \
                     mGBTLinks[lnkId].hit_vector[hit_global_id]->chip_id : -1;
  }
  else if ( strcmp(what, "HIT_BC") == 0 )
  {
    return ( (hit >= 0) && (hit < mGBTLinks[lnkId].mTrgData[trg].n_hits) ) ? \
                     mGBTLinks[lnkId].hit_vector[hit_global_id]->bunchcounter : -1;
  }
  else if ( strcmp(what, "HIT_ROW") == 0 )
  {
    return ( (hit >= 0) && (hit < mGBTLinks[lnkId].mTrgData[trg].n_hits) ) ? \
                     mGBTLinks[lnkId].hit_vector[hit_global_id]->row_pos : -1;
  }
  else if ( strcmp(what, "HIT_COL") == 0 )
  {
    return ( (hit >= 0) && (hit < mGBTLinks[lnkId].mTrgData[trg].n_hits) ) ? \
                     mGBTLinks[lnkId].hit_vector[hit_global_id]->col_pos : -1;
  }
  else
  {
    std::cout << "Unknow option " << what << std::endl;
    return -1;
  }
  return 0;
}


long long int oncsSub_idmvtxv3::lValue(const int i_feeid, const int idx, const char *what)
{
  decode();

  uint32_t feeId = i_feeid;
  uint32_t index = idx;

  if (mFeeId2LinkID.find(feeId) == mFeeId2LinkID.cend())
  {
    log_error << "FeeId " << feeId << "was not found in the feeId mapping for this packet" << std::endl;
    assert(false);
  }
  uint32_t lnkId =  mFeeId2LinkID[feeId].entry;

  if ( strcmp(what, "L1_IR_BCO") == 0 )
  {
    return (index < mGBTLinks[lnkId].mL1TrgTime.size()) ? mGBTLinks[lnkId].mL1TrgTime[index].orbit : -1;
  }
  else if ( strcmp(what, "TRG_IR_BCO") == 0 )
  {
    return (index < mGBTLinks[lnkId].mTrgData.size()) ? mGBTLinks[lnkId].mTrgData[index].ir.orbit : -1;
  }
  else
  {
    std::cout << "Unknow option " << what << std::endl;
    return -1;
  }

  return 0;
}


//_________________________________________________
void * oncsSub_idmvtxv3::pValue(const int channel)
{
  if (channel == getIdentifier())
  {
    return &SubeventHdr->data;
  }

  return nullptr;
}


//_________________________________________________
void oncsSub_idmvtxv3::dump(OSTREAM &os)
{
  identify(os);
  decode();

  // Debug HB pooling
  int num_feeids = iValue(-1, "NR_LINKS");
  os << "Event: " << mEventId << " Number of feeid: " << num_feeids << std::endl;
  if (num_feeids > 0)
  {
    for ( int i = 0; i < num_feeids; ++i )
    {
      auto feeId = iValue(i, "FEEID");
      auto hbfSize = iValue(feeId, "NR_HBF");
      os << "Link " << setw(4) << feeId << " has " << hbfSize << " HBs, ";
      os << iValue(feeId, "NR_STROBES") << " strobes and ";
      os << iValue(feeId, "NR_PHYS_TRG") << " L1 triggers" << std::endl;

      for ( int iL1 = 0; iL1 < iValue(feeId, "NR_PHYS_TRG"); ++iL1 )
      {
        os << "L1: " << iL1  << std::hex << " BCO: 0x" << lValue(feeId, iL1, "L1_IR_BCO");
        os << std::dec << " BC: " << iValue(feeId, iL1, "L1_IR_BC") << endl;
      }

      os << "Total number of hits: " << iValue(feeId, "NR_HITS") << endl;
      for ( int i_trg = 0; i_trg < iValue(feeId, "NR_STROBES"); ++i_trg )
      {
        os << "-- Strobe: " << i_trg;
        os << ", BCO: 0x" << std::hex << lValue(feeId, i_trg, "TRG_IR_BCO") << std::dec;
        os << " BC: " << iValue(feeId, i_trg, "TRG_IR_BC");
        os << ", has " << iValue(feeId, i_trg, "TRG_NR_HITS") << " hits." << std::endl;

        if ( iValue(feeId, i_trg, "TRG_NR_HITS") )
        {
          os << "   hit number chip_id  bc   row   col  "  << endl;
        }
        for ( int i_hit = 0; i_hit < iValue(feeId, i_trg, "TRG_NR_HITS"); ++i_hit )
        {
          os << setw(4) << i_hit;
          os << "  " << setw(9) << iValue(feeId, i_trg, i_hit, "HIT_CHIP_ID");
          os << "  " << setw(8) << std::hex << iValue(feeId, i_trg, i_hit, "HIT_BC") << std::dec;
          os << "  " << setw(4) << iValue(feeId, i_trg, i_hit, "HIT_ROW");
          os << "  " << setw(4) << iValue(feeId, i_trg, i_hit, "HIT_COL");
          os << endl;
        }
      }
    }
  }

  return;
}

//_________________________________________________
oncsSub_idmvtxv3::~oncsSub_idmvtxv3()
{
  for ( auto&& link : mGBTLinks )
  {
    link.clear(true, true);
  }
  feeid_set.clear();
}
