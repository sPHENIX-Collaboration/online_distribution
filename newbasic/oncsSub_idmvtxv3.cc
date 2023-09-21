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
  payload = 0;
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

  buffer.setPtr(payload + payload_position);

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

//  std::bitset<80> report[3] = {};
//  uint16_t w16;

  mvtx_utils::RdhExt_t rdh = {};

  size_t dlength = buf.getEnd() - payload;
  do
  {
    // Skip FLX padding
    if ( (*(reinterpret_cast<uint16_t*>(&payload[payload_position] + 30)) == 0xFFFF) )
    {
      while ( (*(reinterpret_cast<uint16_t*>(&payload[payload_position] + 30)) == 0xFFFF) &&\
             payload_position < dlength)
      {
        payload_position += mvtx_utils::FLXWordLength;
      }
//        ASSERT(! ((decoder->nbytesread + decoder->ptr_pos()) & 0xFF), decoder,
//               "FLX header is not properly aligned in byte %lu of current chunk, previous packet %ld",
//                decoder->ptr_pos(), decoder->ptr_pos(decoder->prev_packet_ptr));
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
          auto& lnkref = mFeeId2LinkID[rdh.feeId];
          if ( lnkref.entry == -1 )
          {
            lnkref.entry = mGBTLinks.size();
            mGBTLinks.emplace_back(rdh.flxId, rdh.feeId);
          }
          auto& gbtLink = mGBTLinks[lnkref.entry];
          auto lnkEndOffser= gbtLink.data.getEnd() - gbtLink.data.getPtr();

          gbtLink.data.add((payload + payload_position), pageSizeInBytes);

          if ( ! rdh.packetCounter ) // start HB
          {
            gbtLink.hb_start = lnkEndOffser;
            gbtLink.hb_length = pageSizeInBytes;
          }
          else
          {
            gbtLink.hb_length += pageSizeInBytes;
          }

          if ( rdh.stopBit ) // found HB end
          {
            gbtLink.cacheData(gbtLink.hb_start, gbtLink.hb_length);
          }
          ASSERT(((! rdh.packetCounter) || (rdh.packetCounter == gbtLink.prev_pck_cnt + 1)),
           "Incorrect pages count %d", rdh.packetCounter);
          gbtLink.prev_pck_cnt = rdh.packetCounter;
          payload_position += pageSizeInBytes;
        }
      }
      else
      {
        // skip raw data without a initial FLX header
        // (YCM)TODO: OK for OM but error otherwise
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
      ASSERT(mGBTLinks.size() == mFeeId2LinkID.size(), "");
      return mGBTLinks.size();
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
    return (i < mGBTLinks.size()) ? mGBTLinks[i].feeID : -1;
  }
  else if ( strcmp(what, "NR_HBF") == 0 )
  {
    ASSERT(mFeeId2LinkID.find(i) != mFeeId2LinkID.cend(),
        "FeeId %d was not found in the feeId mapping for this packet", i);
    uint32_t lnkId =  mFeeId2LinkID[i].entry;
    ASSERT( mGBTLinks[lnkId].hbfData.size() == mGBTLinks[lnkId].rawData.getNPieces(),
              "");
    return mGBTLinks[lnkId].hbfData.size();
  }
  else
  {
    return -1;
  }
/*
  if ( strcmp(what,"NR_HITS") == 0 )  // the number of datasets
  {
    return hit_vector.size();
  }

  if ( i >= hit_vector.size())
    {
      return 0;
    }

  else if ( strcmp(what,"SOURCE_ID") == 0 )
    {
      return hit_vector[i]->source_id;
    }

  else if ( strcmp(what,"LANE") == 0 )
    {
      return hit_vector[i]->lane;
    }

  else if ( strcmp(what,"ENCODER_ID") == 0 )
    {
      return hit_vector[i]->encoder_id;
    }

  else if ( strcmp(what,"ADDR") == 0 )
    {
      return hit_vector[i]->addr;
    }
*/
  return 0;
}


long long oncsSub_idmvtxv3::lValue(const int n, const char *what)
{

  decode();
//  unsigned int i = n;
/*
  if ( i >= hit_vector.size())
    {
      return 0;
    }

  if ( strcmp(what,"RHICBCO") == 0 )
    {
      return hit_vector[i]->RHICBCO;
    }

  else if ( strcmp(what,"LHCBC") == 0 )
    {
      return hit_vector[i]->LHCBC;
    }
*/
  return 0;
}

//_________________________________________________
void oncsSub_idmvtxv3::dump(OSTREAM &os)
{
  decode();
  identify(os);

  // Debug HB pooling
  os << "Number of feeid: " << iValue(-1, "NR_LINKS") << endl;
  for ( uint32_t i = 0; i < mFeeId2LinkID.size(); ++i )
  {
    auto feeId = iValue(i, "FEEID");
    os << "Link " << setw(4) << feeId << " has " << iValue(feeId, "NR_HBF") << " HBs." << endl;
  }

//  unsigned int n;
/*
  unsigned int nr_hits = iValue(0, "NR_HITS");

  os << "Number of hits: " << nr_hits << endl;

  os << " hit number   addr   enc.id   lane   src.id      BCO         LHC BC  "  << endl;

  for ( n = 0; n < nr_hits; n++)
    {
      os << setw(6) << n
      << "  " << setw(6)  << iValue(n, "ENCOODER_ID")
	 << "     " << setw(6) << iValue(n, "ADDR")
	 << "  " << setw(6)  << iValue(n, "LANE")
	 << "  " << setw(6)  << iValue(n, "SOURCE_ID")
	 << "  " << setw(6)  << "0x" << lValue(n, "RHICBCO")
	 << "  " << setw(6)  << "0x" << lValue(n, "LHCBC")
	 << endl;
    }
*/
// 0 0 0 0 0 0 1 0 0 0 0 0

//   iValue(x, "L1TriggerBCO")
  return;
}

//_________________________________________________
oncsSub_idmvtxv3::~oncsSub_idmvtxv3()
{

  for ( auto& buffer : mBuffers )
  {
    buffer.clear();
  }

  for ( auto& lnk : mGBTLinks )
  {
    lnk.clear(true, true);
    lnk.data.clear();
  }
/*
  chipdata.clear();

  std::vector<mvtx_hit *>::iterator itr = hit_vector.begin();
  for ( ; itr != hit_vector.end(); ++itr)
    {
      delete (*itr);
    }
  hit_vector.clear();
*/
}
