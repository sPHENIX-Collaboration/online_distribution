#include "oncsSub_idmvtxv3.h"

// for memset
#include <string>
#include <cassert>

using namespace std;

// define static references
size_t oncsSub_idmvtxv3::mEventId = 0;
std::unordered_map<uint16_t, oncsSub_idmvtxv3::dumpEntry> oncsSub_idmvtxv3::mFeeId2LinkID = {};
std::vector<mvtx::GBTLink> oncsSub_idmvtxv3::mGBTLinks = {};

oncsSub_idmvtxv3::oncsSub_idmvtxv3(subevtdata_ptr data)
  : oncsSubevent_w1(data)
{
  m_is_decoded = false;
  m_decoding_failed = false;
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
    link.clear(true, true); // clear data but not the statistics
    link.RDHErrors = 0;
    link.hbf_length = 0;
    link.prev_pck_cnt = 0;
  }

//  short pck_id = getIdentifier();

  payload_start = (uint8_t *) &SubeventHdr->data;  // here begins the payload
  payload_length = getDataLength() - getPadding(); //padding is supposed to be in units of dwords, this assumes dwords
  payload_length *= 4;

  if ( payload_length % mvtx_utils::FLXWordLength )
  {
    payload_length -= payload_length % mvtx_utils::FLXWordLength;
    COUT
      << ENDL
      << "!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!! \n"
      << "DMA packet has incomplete FLX words, only "
      << payload_length << " bytes(" << (payload_length / mvtx_utils::FLXWordLength)
      << " FLX words), will be decoded. \n"
      << "!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!! \n"
      << ENDL;
  }
  payload_position = 0;

  setupLinks();

  for (auto& link : mGBTLinks)
  {
    mvtx::GBTLink::CollectedDataStatus decoding_status = link.collectROFCableData();
    if(decoding_status == mvtx::GBTLink::CollectedDataStatus::AbortedOnError)
    {
      m_decoding_failed = true;
      link.clearCableData();
    }
  }

  ++mEventId;
  return 0;
}

void oncsSub_idmvtxv3::setupLinks()
{
  mvtx_utils::RdhExt_t rdh = {};
  do
  {
    // Skip FLX padding
    if ( *(reinterpret_cast<uint16_t*>(&payload_start[payload_position] + 30)) == 0xFFFF )
    {
      payload_position += mvtx_utils::FLXWordLength;
    }
    else if ( (payload_length - payload_position) >= 2 * mvtx_utils::FLXWordLength ) // at least FLX header and RDH
    {
      if ( *(reinterpret_cast<uint16_t*>(&payload_start[payload_position] + 30)) == 0xAB01 )
      {
        rdh.decode(&payload_start[payload_position]);
        const size_t pageSizeInBytes = (rdh.pageSize + 1) * mvtx_utils::FLXWordLength;
        if ( pageSizeInBytes > (payload_length - payload_position) )
        {
          std::cout << "Incomplete Felix packet, remaining data " << (payload_length - payload_position);
          std::cout << " bytes less than " << pageSizeInBytes << " bytes"<< std::endl;
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

          if ( ! rdh.checkRDH(true) )
          {
            // In case of corrupt RDH, skip felix word and continue to next
            payload_position += mvtx_utils::FLXWordLength;
            gbtLink.RDHErrors++;
            continue;
          }

          if ( (rdh.packetCounter) && (gbtLink.rawData.getNPieces()) && (rdh.packetCounter != gbtLink.prev_pck_cnt + 1) )
          {
            log_error << "Incorrect pages count " << rdh.packetCounter <<", previous page count was "
              << gbtLink.prev_pck_cnt << std::endl;
            payload_position += pageSizeInBytes;
            continue;
          }
          gbtLink.prev_pck_cnt = rdh.packetCounter;

          gbtLink.data.add((payload_start + payload_position), pageSizeInBytes);

          if ( ! rdh.packetCounter ) // start HB
          {
            if ( gbtLink.hbf_length )
            {
              log_error << "FLX: " << gbtLink.flxId << ", FeeId: " << gbtLink.feeId
                << ". Found new HBF before stop previous HBF. Previous HBF will be ignored." << std::endl;
                gbtLink.cacheData(gbtLink.hbf_length, true);
            }
            gbtLink.hbf_length = pageSizeInBytes;
            gbtLink.hbf_error = false;
          }
          else
          {
            if ( (! gbtLink.hbf_length) && (gbtLink.rawData.getNPieces()) )
            {
              log_error << "FLX: " << gbtLink.flxId << ", FeeId: " << gbtLink.feeId
              << ". Found continuous HBF before start new HBF. data will be ignored." << std::endl;
              gbtLink.hbf_error = true;
            }
            gbtLink.hbf_length += pageSizeInBytes;
            if ( rdh.stopBit ) // found HB end
            {
              gbtLink.cacheData(gbtLink.hbf_length, gbtLink.hbf_error);
              gbtLink.hbf_length = 0;
            }
          }
          payload_position += pageSizeInBytes;
        }
      }
      else
      {
        // skip raw data without a initial FLX header
        // (YCM)TODO: OK for OM but error otherwise
        if(0)
        {
          std::cout << "Felix header: " << std::hex << "0x" << std::setfill('0') << std::setw(4);
          std::cout << *(reinterpret_cast<uint16_t*>(&payload_start[payload_position] + 30)) << std::dec <<std::endl;
        }
        payload_position += mvtx_utils::FLXWordLength;
      }
    }
    else
    {
      std::cout << "Incomplete Felix header, remaining data " << (payload_length - payload_position);
      std::cout << " bytes less than " << (2 * mvtx_utils::FLXWordLength) << std::endl;
      break; // skip incomplete flx_header
    }
  } while (payload_position < payload_length);

  return;
}


int oncsSub_idmvtxv3::iValue(const int n, const char *what)
{
  decode();
  if(m_decoding_failed == true) return -1;
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
    else if ( strcmp(what, "RDH_ERRORS") == 0 )
    {
    return mGBTLinks[lnkId].RDHErrors;
    }
    else if ( strcmp(what, "NR_HITS") == 0 )  // the number of datasets
    {
      return mGBTLinks[lnkId].hit_vector.size();
    }
    else if ( strcmp(what, "tdt_lanestatus_error") == 0 )  // the number of datasets
    {
      int error = -1;
      if(!mGBTLinks[lnkId].tdt_lanestatus_error_vector.empty())
      {
        error = static_cast<int>(mGBTLinks[lnkId].tdt_lanestatus_error_vector.back());
        mGBTLinks[lnkId].tdt_lanestatus_error_vector.pop_back();
      }
      return error;
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
  if(m_decoding_failed == true) return -1;
  uint32_t feeId = i_feeid;
  uint32_t index = idx;

  if (mFeeId2LinkID.find(feeId) == mFeeId2LinkID.cend())
  {
    log_error << "FeeId " << feeId << " was not found in the feeId mapping for this packet" << std::endl;
    assert(false);
    return -1;
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
  if(m_decoding_failed == true) return -1;

  uint32_t feeId = i_feeid;
  uint32_t trg = i_trg;
  uint32_t hit = i_hit;

  if (mFeeId2LinkID.find(feeId) == mFeeId2LinkID.cend())
  {
    log_error << "FeeId " << feeId << "was not found in the feeId mapping for this packet" << std::endl;
    assert(false);
    return -1;
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

long long int oncsSub_idmvtxv3::lValue(const int i_feeid, const char *what)
{
  decode();
  //if(m_decoding_failed == true) return -1; should be commented - decoder errors are filled even when decoding failed

  uint32_t feeId = i_feeid;

  if (mFeeId2LinkID.find(feeId) == mFeeId2LinkID.cend())
  {
    log_error << "FeeId " << feeId << "was not found in the feeId mapping for this packet" << std::endl;
    assert(false);
    return -1;
  }
  uint32_t lnkId =  mFeeId2LinkID[feeId].entry;

  if ( strcmp(what, "decoder_error") == 0 )  // the number of datasets
    {
      long long int error = -1;
      if(!mGBTLinks[lnkId].decoder_error_vector.empty())
      {
        std::pair<int,int> pop = mGBTLinks[lnkId].decoder_error_vector.back();
        error = (long long int) pop.first << 32 | pop.second;
        mGBTLinks[lnkId].decoder_error_vector.pop_back();
      }
      return error;
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
  if(m_decoding_failed == true) return -1;

  uint32_t feeId = i_feeid;
  uint32_t index = idx;

  if (mFeeId2LinkID.find(feeId) == mFeeId2LinkID.cend())
  {
    log_error << "FeeId " << feeId << "was not found in the feeId mapping for this packet" << std::endl;
    assert(false);
    return -1;
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
      int layer = feeId>>12;
      int gbtx = (feeId>> 8) & 0x3;
      int stave = feeId & 0x3f;
      os << "FeeId " << hex << setw(4) << setfill('0') << feeId << dec << " (L" << layer <<
      "_" << setw(2) << stave << " GBTx" << setw(1) << gbtx << ") has " << hbfSize << " HBs, ";
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

std::string interpretGbtPacket(const unsigned char packetCode)
{
  std::string msg;
  switch (packetCode)
  {
    case 0x0:
      msg = " RDH     "; break;
    case 0xe0:
      msg = " IHW     "; break;
    case 0xe8:
      msg = " TDH     "; break;
    case 0xf0:
      msg = " TDT     "; break;
    case 0xe4:
      msg = " DDW     "; break;
    case 0xf8:
      msg = " CDW     "; break;
    case 0x20 ... 0x28:
      msg = " DTA     "; break;
    case 0xa0 ... 0xa8:
      msg = " Diag DTA"; break;
    default:
      msg = " ...     ";
  }
return msg;
}

//_________________________________________________
void oncsSub_idmvtxv3::gdump(const int i, OSTREAM& out) const
{
  uint8_t *SubeventData = reinterpret_cast<uint8_t *> (&SubeventHdr->data);


  if ( i == EVT_RAW)
  {
    fwrite(SubeventData, sizeof(int), getDataLength(), stdout);
    return;
  }

  if ( i == EVT_RAW_WH)
  {
    fwrite(SubeventHdr, sizeof(int), getLength(), stdout);
    return;
  }

  unsigned int j;
  int l;
  std::string msg;
  bool isFelixHeader;
  char cstring[20];
  char *c;

  identify(out);

  j = 0;
  switch (i)
  {
    case (EVT_HEXADECIMAL):
      while (1)
	    {
        msg = "";
        isFelixHeader = false;
	      out << std::hex <<  SETW(5) << j << " |  ";
        if ( *reinterpret_cast<uint16_t *>(&SubeventData[j+30])  == 0xab01)
        {
          msg = " | FELIX Header";
          isFelixHeader = true;
          msg += " GBT " + std::to_string( static_cast<int>(SubeventData[j+28]) );
          msg += " DMA Cnt " + std::to_string( (static_cast<int>(SubeventData[j+26])) * 256 + static_cast<int>(SubeventData[j+25]) );
        }
        else
        {
          msg += " |";
          msg += interpretGbtPacket(SubeventData[j+9]);
        }
	      for (l=0;l<10;l++)
	      {
	        if (j < 4*(SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) ) 
		      {
		        out << std::hex << SETW(2) << static_cast<u_int>(SubeventData[j]) << " ";
		      }
	        j++;
	      }
        out << " ";
        if (!isFelixHeader)
        {
          if ((SubeventData[j+20]%3 == 2) || (SubeventData[j+20]%3 == 0))
          {
            msg += " |";
            msg += interpretGbtPacket(SubeventData[j+9]);
          }
        }
	      for (l=10;l<20;l++)
	      {
	        if (j < 4*(SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) ) 
		      {
		        out << std::hex << SETW(2) << static_cast<u_int>(SubeventData[j]) << " ";
		      }
	        j++;
	      }
        out << " ";
        if (!isFelixHeader)
        {
          if (SubeventData[j+10]%3 == 0) 
          {
            msg += " |";
            msg += interpretGbtPacket(SubeventData[j+9]);
          }
        }
	      for (l=20;l<30;l++)
	      {
	        if (j < 4*(SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) ) 
		      {
		        out << std::hex << SETW(2) << static_cast<u_int>(SubeventData[j]) << " ";
		      }
	        j++;
	      }
        out << " ";
	      for (l=30;l<32;l++)
	      {
	        if (j < 4*(SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) ) 
		      {
		        out << std::hex << SETW(2) << static_cast<u_int>(SubeventData[j]) << " ";
		      }
	        j++;
	      }
	      out << msg << std::endl;
	      if (j >= 4*(SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) ) break;
	    }
      break;

    case (EVT_DECIMAL):
      while (1)
	    {
	      c = cstring;
	      out << std::dec <<  SETW(5) << j << " |  ";
	      for (l=0;l<16;l++)
	      {
	        if (j < 4*(SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) ) 
		      {
		        out  << SETW(3) << static_cast<u_int>(SubeventData[j]) << " ";
		        if (SubeventData[j] >=32 && SubeventData[j] <127) 
		        {
		          *c++ = SubeventData[j];
		        }
		        else
		        {
		          *c++ = 0x20;
		        }
		        out << " ";
		      }
	        j++;
	      }
	      *c = 0;
	      out << "  | " << cstring;
	      out << std::endl;
	      if (j >= 4*(SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) ) break;
	    }
      break;

    default: break;
  }
  out << std::endl;
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
