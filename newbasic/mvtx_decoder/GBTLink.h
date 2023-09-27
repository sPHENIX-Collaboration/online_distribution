// @file GBTLink.h
// @brief Declarations of helper classes for the ITS/MFT raw data decoding
// @sa <O2/Detectors/ITSMFT/common/reconstruction/include/ITSMFTReconstruction/GBTLink.>
//     <760019308>

#ifndef MVTXDECODER_GBTLINK_H
#define MVTXDECODER_GBTLINK_H

#define _RAW_READER_ERROR_CHECKS_ // comment this to disable error checking

#include "mvtx_decoder/mvtx_utils.h"
#include "mvtx_decoder/PayLoadCont.h"
#include "mvtx_decoder/PayLoadSG.h"
#include "mvtx_decoder/DecodingStat.h"
#include "mvtx_decoder/GBTWord.h"
#include "mvtx_decoder/InteractionRecord.h"

//#include "MVTXDecoder/RUDecodeData.h"
//#include "MVTXDecoder/RUInfo.h"
//#include "MVTXDecoder/RAWDataHeader.h"
//#include "MVTXDecoder/RDHUtils.h"
//#include "MVTXDecoder/PhysTrigger.h"

#include <iostream>

#define GBTLINK_DECODE_ERRORCHECK(errRes, errEval)                            \
  errRes = errEval;                                                           \
  if ((errRes)&uint8_t(ErrorPrinted)) {                                       \
    ruPtr->linkHBFToDump[(uint64_t(subSpec) << 32) + hbfEntry] = irHBF.orbit; \
    errRes &= ~uint8_t(ErrorPrinted);                                         \
  }                                                                           \
  if ((errRes)&uint8_t(Abort)) {                                              \
    discardData();                                                            \
    return AbortedOnError;                                                    \
  }

namespace mvtx
{
  using namespace mvtx_utils;

  struct TRGData
  {
    InteractionRecord ir = {};
    bool hasCDW = false;
    GBTCalibDataWord calWord = {};

    TRGData(uint64_t orb, uint16_t b) : ir(orb, b) {};

    void clear()
    {
      ir.clear();
      hasCDW = false;
      calWord = {};
    }
  };

/// support for the GBT single link data
struct GBTLink
{
//  enum RawDataDumps : int { DUMP_NONE, // no raw data dumps on error
//                            DUMP_HBF,  // dump HBF for FEEID with error
//                            DUMP_TF,   // dump whole TF at error
//                            DUMP_NTYPES };

  enum CollectedDataStatus : int8_t { None,
                                      AbortedOnError,
                                      StoppedOnEndOfData,
                                      DataSeen }; // None is set before starting collectROFCableData

//  enum ErrorType : uint8_t { NoError = 0x0,
//                             Warning = 0x1,
//                             Skip = 0x2,
//                             Abort = 0x4,
//                             ErrorPrinted = 0x1 << 7 };
//
//  enum Verbosity : int8_t { Silent = -1,
//                            VerboseErrors,
//                            VerboseHeaders,
//                            VerboseData };
//
//  using RDH = mvtx::RDHAny;
//  using RDHUtils = mvtx::RDHUtils;

  CollectedDataStatus status = None;
//  Verbosity verbosity = VerboseErrors;

//  std::vector<PhysTrigger> trgVec;

//  uint8_t idInRU = 0;     // link ID within the RU
//  uint8_t idInCRU = 0;    // link ID within the CRU
//  bool gbtErrStatUpadated = false;
  uint16_t flxID = 0;     // FLX ID
  uint16_t feeID = 0;     // FEE ID
//  uint16_t channelID = 0; // channel ID in the reader input
//  uint32_t lanes = 0;     // lanes served by this link
//  uint32_t subSpec = 0;   // link subspec

  PayLoadCont data; // data buffer for single feeeid

  uint32_t hbfEntry = 0;      // entry of the current HBF page in the rawData SG list
  InteractionRecord ir = {};

  GBTLinkDecodingStat statistics; // link decoding statistics
  size_t hbf_count = 0;
//  ChipStat chipStat;              // chip decoding statistics
//  RUDecodeData* ruPtr = nullptr;  // pointer on the parent RU

  PayLoadSG rawData;         // scatter-gatter buffer for cached CRU pages, each starting with RDH
  size_t dataOffset = 0;     //
  std::vector<InteractionRecord> physTrgTime;
  std::vector<TRGData> mTrgData;

  static constexpr uint8_t MaxCablesPerLink = 3;
  std::array<PayLoadCont, MaxCablesPerLink> cableData;
  void clearCableData()
  {
    for ( auto&& data : cableData )
    {
      data.clear();
    }
  }

  static constexpr int RawBufferMargin = 5000000;                      // keep uploaded at least this amount
  static constexpr int RawBufferSize = 10000000 + 2 * RawBufferMargin; // size in MB


  //------------------------------------------------------------------------

  GBTLink() = default;
  GBTLink(uint16_t _flx, uint16_t _fee);
//  std::string describe() const;
  void clear(bool resetStat = true, bool resetTFRaw = false);

//  template <class Mapping>
  CollectedDataStatus collectROFCableData(/*const Mapping& chmap*/);

  void cacheData(size_t start, size_t sz)
  {
    rawData.add(start, sz);
  }

  int readFlxWord( GBTWord* gbtwords, uint16_t &w16 );

//  ClassDefNV(GBTLink, 1);
};

///_________________________________________________________________
/// collect cables data for single ROF, return number of real payload words seen,
/// -1 in case of critical error
//template <class Mapping>
inline GBTLink::CollectedDataStatus GBTLink::collectROFCableData(/*const Mapping& chmap*/)
{
  bool prev_evt_complete = false;
  bool header_found = false;
  bool trailer_found = false;

  status = None;

  auto currRawPiece = rawData.currentPiece();
  dataOffset = 0;
  while (currRawPiece)
  { // we may loop over multiple FLX page
    uint32_t n_no_continuation = 0;
    uint32_t n_packet_done = 0;

    if (dataOffset >= currRawPiece->size)
    {
      data.movePtr(dataOffset);
      dataOffset = 0;                              // start of the RDH
      if ( ! (currRawPiece = rawData.nextPiece()) )
      { // fetch next CRU page
        break;                                     // Data chunk (TF?) is done
      }
    }
    // here we always start with the RDH
    RdhExt_t rdh = {};
    rdh.decode(data.getPtr() + dataOffset);

    size_t pagesize = (rdh.pageSize + 1) * FLXWordLength;
    const size_t nFlxWords = (pagesize - (2 * FLXWordLength)) / FLXWordLength;
    //Fill statistics
    if ( !rdh.packetCounter )
    {
      ASSERT(!dataOffset, "Wrong dataOffset value %ld at the start of a HBF", dataOffset);
//      statistics.clear();
      //TODO: initialize/clear alpide data buffer
      for ( uint32_t trg = GBTLinkDecodingStat::BitMaps::ORBIT; trg < GBTLinkDecodingStat::nBitMap; ++trg )
      {
        if  ( (rdh.trgType >> trg) & 1 )
        {
          statistics.trgBitCounts[trg]++;
        }
      }
      hbfEntry = rawData.currentPieceID(); // in case of problems with RDH, dump full TF
      ++hbf_count;
    }
    else if ( !rdh.stopBit )
    {
      ASSERT( !prev_evt_complete, "Previous event was already completed");
    }

    dataOffset += 2 * FLXWordLength;
    int prev_gbt_cnt = 3;
    GBTWord gbtWords[3];
    uint16_t w16 = 0;
    for ( size_t iflx = 0; iflx < nFlxWords; ++iflx )
    {
	    readFlxWord(gbtWords, w16);
	    const uint16_t n_gbt_cnt = (w16 & 0x3FF) - prev_gbt_cnt;
      prev_gbt_cnt = (w16 & 0x3FF);
      ASSERT( (n_gbt_cnt <= 3), "Error. Bad gbt counter in the flx packet.");
      for ( int i = 0; i < n_gbt_cnt; ++i )
      {
        auto &gbtWord = gbtWords[i];
        if ( gbtWord.isIHW() ) // ITS HEADER WORD
        {
          //TODO assert first word after RDH and active lanes
          ASSERT( ( ((gbtWord.activeLanes >> 0) & 0x7) == 0x7 || \
                    ((gbtWord.activeLanes >> 3) & 0x7) == 0x7 || \
                    ((gbtWord.activeLanes >> 6) & 0x7) == 0x7),
              "Expected all active lanes for links, but %d found in HBF %d, %s", \
              gbtWord.activeLanes, hbfEntry, gbtWord.asString().data());
        }
        else if ( gbtWord.isTDH() ) // TRIGGER DATA HEADER (TDH)
        {
          header_found = true;
          ir.orbit = gbtWord.bco;
          ir.bc = gbtWord.bc;
          if ( gbtWord.bc ) //statistic trigger for first bc already filled on RDH
          {
            for ( uint32_t trg = GBTLinkDecodingStat::BitMaps::ORBIT; trg < GBTLinkDecodingStat::nBitMap; ++trg )
            {
              if ( trg == GBTLinkDecodingStat::BitMaps::FE_RST ) //  TDH save first 12 bits only
                break;
              if ( ((gbtWord.triggerType >> trg) & 1) )
              {
                statistics.trgBitCounts[trg]++;
              }
            }
          }

          if ( (gbtWord.triggerType >> GBTLinkDecodingStat::BitMaps::PHYSICS) & 0x1 )
          {
            physTrgTime.push_back(ir);
          }

          if ( !gbtWord.continuation && !gbtWord.noData)
          {
            n_no_continuation++;
            mTrgData.emplace_back(ir.orbit, ir.bc);
          } // end if not cont
        } // end TDH
        else if ( gbtWord.isCDW() ) // CALIBRATION DATA WORD
        {
          mTrgData.back().hasCDW = true;
          mTrgData.back().calWord = *(reinterpret_cast<GBTCalibDataWord*>(&gbtWord));
        }
        else if ( gbtWord.isTDT() )
        {
          trailer_found = true;
          if ( gbtWord.packet_done )
          {
            n_packet_done++;
            ASSERT(n_packet_done == n_packet_done,
               "TDT packet done before TDH no continuation %d != %d",
               n_packet_done, n_packet_done);
          }
          prev_evt_complete = gbtWord.packet_done;
          //TODO: YCM Add warning and counter for timeout and violation
        }
        else if ( gbtWord.isDDW() ) // DIAGNOSTIC DATA WORD (DDW)
        {
            ASSERT(rdh.stopBit, "");
            ASSERT(gbtWord.index == 0, "");
        }
        else if ( gbtWord.isDiagnosticIB() ) // IB DIAGNOSTIC DATA
        {
            std::cout << "WARNING: IB Diagnostic word found." << std::endl;
            std::cout << "diagnostic_lane_id: " << (gbtWord.id >> 5);
            std::cout << " lane_error_id: " << gbtWord.lane_error_id;
            std::cout << " diasnotic_data: 0x" << std::hex << gbtWord.diagnostic_data << std::endl;
        }
        else if ( gbtWord.isData() ) //IS IB DATA
        {
          ASSERT(header_found, "Trigger header not found before chip data");
          auto lane = ( gbtWord.data8[9] & 0x1F ) % 3;
          cableData[lane].add(gbtWord.getW8(), 9);
        }

        if ( prev_evt_complete )
        {
          /*
                                if not self.skip_data:
                                    previous_bc_count = bc_count
                                    bc_count = None
                                    for lane, data in lanes.items():
                                        try:
                                            chip_data = self.decode_alpide(data, self.accept_decreasing_address, thscan_current_charge, thscan_current_row)
                                            for i, d in enumerate(chip_data):
                                                if bc_count:
                                                    # assert d['bc']==bc_count, f"bc_count on chip {i}, lane {lane} not matching the one of the other chips: 0x{d['bc']:02x} != 0x{bc_count:02x}"
                                                    pass
                                                else:
                                                    bc_count = d['bc']
                                                    # if not self.thscan: # thscan does not need to check this. Check is done in line 377 and following
                                                    #     assert bc_count != previous_bc_count, f"Previous and current bc counts are the same! 0x{bc_count:2x} == 0x{previous_bc_count:2x}"
                                        except:
                                            self.logger.info(f"Error in block before byte {self.bytes_read} for lane {lane}")
                                            raise
                                        if (iblock_event-boffset)%self.print_interval==0:
                                            for d in chip_data:
                                                self.logger.info(f"iblock_event {iblock_event}, Lane {lane}, bytes_read {self.bytes_read}: {d}")
            */
          prev_evt_complete = false;
          header_found = false;
          trailer_found = false;
          clearCableData();
        }
      }
    }
  }
  return  (status = StoppedOnEndOfData);
}

} // namespace mvtx

#endif // _MVTX_DECODER_ITSMFT_GBTLINK_H_

//  unsigned long get_GBT_value( const std::bitset<80> gbtword, const int pos, const int size) const;
////_________________________________________________
//unsigned long  oncsSub_idmvtxv3::get_GBT_value( const std::bitset<80> gbtword, const int pos, const int size) const
//{
//  unsigned long value = 0;
//
//  for (int i = 0; i < size; i++)
//    {
//      //cout << __FILE__ << " " << __LINE__ << " pos " << pos+i << " " << gbtword.test(pos+i) << endl;
//      value |= ( gbtword.test(pos+i) << i) ;
//    }
//  return value;
//}


//  unsigned long long get_GBT_lvalue( const std::bitset<80> gbtword, const int pos, const int size) const;
////_________________________________________________
//// this one can do sizes > 32 bit (like the BCO)
//unsigned long  long oncsSub_idmvtxv3::get_GBT_lvalue( const std::bitset<80> gbtword, const int pos, const int size) const
//{
//  unsigned long long value = 0;
//
//  for (int i = 0; i < size; i++)
//    {
//      //cout << __FILE__ << " " << __LINE__ << " pos " << pos+i << " " << gbtword.test(pos+i) << endl;
//      value |= ( gbtword.test(pos+i) << i) ;
//    }
//  return value;
//}



//  int  decode_chipdata( const std::bitset<80> gbtword);
////_________________________________________________
//int  oncsSub_idmvtxv3::decode_chipdata( const std::bitset<80> gbtword)
//{
//  // unsigned int chipid;
//  // unsigned int timestamp;
//  // unsigned int region_id;
//  // unsigned int dl_bitmap;
//  // unsigned int readout_flags;
//  // unsigned int layer;
//  // unsigned int chipnr;
//  // unsigned int addr;
//
//  unsigned int marker =  get_GBT_value (gbtword, 72,8);
//  unsigned int laneidx = marker & 0x1F;
//
//  // add a check if that lane matches the embedded lane
//
//  std::map<unsigned int, std::vector<uint8_t> >::iterator itr;
//  itr = chipdata.find(laneidx);
//  // if ( itr == chipdata.end())
//  //   {
//  //     cout << __FILE__ << " " << __LINE__ << " new laneidx " << laneidx << endl;
//  //   }
//  // else
//  //   {
//  //     cout << __FILE__ << " " << __LINE__ << " adding to laneidx " << laneidx << endl;
//  //   }
//
//  //take the first 9 bytes off
//  for ( unsigned int index = 0; index < 72; index += 8)
//    {
//      chipdata[laneidx].push_back ( get_GBT_value (gbtword, index,8));
//    }
//
//  return 0;
//}


//  void  pretty_print( const std::bitset<80> gbtword) const;
////_________________________________________________
//void  oncsSub_idmvtxv3::pretty_print( const std::bitset<80> gbtword) const
//{
//
//  cout  << std::setfill('0');
//  for ( int i = 0; i < 10 ; i++)
//    {
//        cout << hex << setw(2) << get_GBT_value (gbtword, i * 8, 8) << " " << dec;
//    }
//  cout  << std::setfill(' ') << " ";
//}

//
//  int  decode_lane( const std::vector<uint8_t> v);
////_________________________________________________
//int  oncsSub_idmvtxv3::decode_lane( const std::vector<uint8_t> v)
//{
//
//  uint32_t pos = 0;
//  //std::vector<uint8_t>::const_iterator itr = v.begin();
//
//  int ret = 0; // currently we just print stuff, but we will add stuff to our
//               // structures and return a status later (that's why it's not a const function)
//
//  unsigned int chipid;
//  unsigned int region_id;
//  unsigned int readout_flags;
//  unsigned int addr;
//  unsigned int bunchcounter;
//  unsigned int encoder_id;
//
//  mvtx_hit *h = 0;
//
//
//
//  while ( pos < v.size() )
//    {
//      unsigned int marker = v[pos];
//      if  ((marker >> 4) ==  0xa)
//	{
//	  //	  cout  << " chip header found at position " << pos << endl;
//	  break;
//	}
//      pos++;
//    }
//
//  while ( pos < v.size() )
//    {
//      if ( pos >= v.size() ) break;
//      unsigned int marker = v[pos++];
//
//      if  ((marker >> 4) ==  0xa)  // chip header
//	{
//	  chipid    = marker & 0xf;
//	  bunchcounter = v[pos++];
//	  if ( pos >= v.size() ) break;
//	  // cout  << " chip header " << hex << marker << " chip id: " << chipid << " bunch " << bunchcounter << dec;
//	}
//
//      else if  ((marker ) ==  0xff)  // idle frame
//	{
//	  // cout  << " idle frame " << hex << marker <<  dec;
//	  if ( pos >= v.size() ) break;
//	}
//
//      else if  ((marker >> 4) ==  0xe)  // chip empty frame
//	{
//	  chipid    = marker & 0xf;
//	  bunchcounter = v[pos++];
//	  if ( pos >= v.size() ) break;
//	  // cout  << " chip empty frame " << hex << marker << " chip id: " << chipid << " bunch " << bunchcounter << dec;
//	}
//
//
//      else if  ((marker >> 4) ==  0xb)  // chip trailer
//	{
//	  readout_flags    = marker & 0xf;
//	  // cout  << " chip header " << hex << marker << " readout_flags " << readout_flags << dec;
//	}
//
//      else if  ((marker & 0xe0) ==  0xc0)  // region header 110x xxxx
//	{
//	  region_id    = marker & 0x1f;
//	  // cout  << " chip region hdr " << hex << marker << " region id " << region_id  << " next word is " << (unsigned short) v[pos] << dec;
//	}
//
//      else if  ((marker & 0xc0) ==  0x40) // we have a DATA short report
//	{
//	  unsigned short secondword = v[pos++];
//	  if ( pos >= v.size() ) break;
//	  encoder_id     = (marker >>2) & 0xf;
//	  addr           = (marker & 0x3) << 8 | secondword;
//	  // cout  << " data short hdr " << hex << setw(2) << marker << " " << setw(2) << secondword
//	  // 	<< " encoder_id " << setw(5) << encoder_id
//	  // 	<< " addr " << setw(5) << addr << dec ;
//
//	  h = new mvtx_hit;
//	  memset(h, 0, sizeof(*h));
//
//	  h->RHICBCO = last_BCO;
//	  h->LHCBC = last_LHCBC;
//	  h->source_id = last_source_id;
//	  h->fee_id = last_fee_id;
//	  h->lane = last_lane;
//	  h->addr  = addr;
//
//	  hit_vector.push_back(h);
//	}
//
//      else if  ((marker & 0xc0) ==  0x00) // we have a DATA long report
//	{
//	  unsigned short secondword = v[pos++];
//	  if ( pos >= v.size() ) break;
//	  unsigned short bits  = v[pos++];
//	  if ( pos >= v.size() ) break;
//	  encoder_id     = (marker >>2) & 0xf;
//	  addr           = (marker & 0x3) << 8 | secondword;
//	  // cout  << " data long hdr  " << hex << setw(2) << marker << " " << setw(2) << secondword << " " << setw(2) << bits
//	  // 	<< " encoder_id " << setw(5) << encoder_id
//	  // 	<< " addr " << setw(5) << addr;
//	  h = new mvtx_hit;
//	  memset(h, 0, sizeof(*h));
//
//	  h->RHICBCO = last_BCO;
//	  h->LHCBC = last_LHCBC;
//	  h->source_id = last_source_id;
//	  h->fee_id = last_fee_id;
//	  h->lane = last_lane;
//	  h->addr  = addr;
//	  hit_vector.push_back(h);
//
//	  while ( bits != 0)
//	    {
//	      addr++;
//	      if ( bits & 1)
//		{
//		  // another bit in a data long
//		  h = new mvtx_hit;
//		  memset(h, 0, sizeof(*h));
//
//		  h->RHICBCO = last_BCO;
//		  h->LHCBC = last_LHCBC;
//		  h->source_id = last_source_id;
//		  h->fee_id = last_fee_id;
//		  h->lane = last_lane;
//		  h->addr  = addr;
//		  hit_vector.push_back(h);
//
//
//		  // cout << setw(5) << addr;
//		}
//	      bits >>=1;
//	    }
//
//	}
//
//      else if  ((marker & 0xF0) == 0xB0) // we have a CHIP trailer
//	{
//	  readout_flags     = (marker >> 4) & 0xf;
//	  //  cout  << " chip trailer " << hex  << " readout flags " << setw(5) << readout_flags << dec;
//	}
//
//      //cout << endl;
//    }
//
//  return ret;
//
//}


 // the per-lane keeper of the chip data
  // lane, vector
//  std::map<unsigned int, std::vector<unsigned char> > chipdata;
//
//  std::vector< std::vector<std::bitset<80> > > gbtvector;
//
//  unsigned long long last_BCO;
//  unsigned long long last_LHCBC;
//  unsigned short last_source_id;
//  unsigned short last_fee_id;
//  unsigned short last_lane;
//
//
//  struct mvtx_hit
//  {
//    unsigned long long RHICBCO;
//    unsigned long long LHCBC;
//    // take out    unsigned int source_id;
//
//    unsigned int chip_id;
//
//    unsigned int fee_id;
//    unsigned int lane;
//    unsigned int encoder_id;
//    unsigned int addr;
//    unsigned int source_id;
//    // unsigned int row_pos;
//    // unsigned int col_pos;
//    // unsigned int L1Trigger;
//
//    // unsigned int chipid;
//    // unsigned int region_id;
//    // unsigned int readout_flags;
//    // unsigned int bunchcounter;
//  };
//
//  std::vector<mvtx_hit *> hit_vector;

