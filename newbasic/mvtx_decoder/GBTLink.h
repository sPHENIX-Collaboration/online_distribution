// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

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

//#include "MVTXDecoder/GBTWord.h"
//#include "MVTXDecoder/RUDecodeData.h"
//#include "MVTXDecoder/RUInfo.h"
//#include "MVTXDecoder/RAWDataHeader.h"
//#include "MVTXDecoder/RDHUtils.h"
//#include "MVTXDecoder/InteractionRecord.h"
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

//struct RUDecodeData; // forward declaration to allow its linking in the GBTlink
//struct GBTTrigger;
//struct GBTTrailer;

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


  uint32_t prev_pck_cnt = 0;
  uint8_t* hb_start = nullptr;
  size_t   hb_length = 0;

  PayLoadCont data; // data buffer for single feeeid

  uint32_t hbfEntry = 0;      // entry of the current HBF page in the rawData SG list
  const mvtx_utils::RdhExt_t* lastRDH = nullptr;

  GBTLinkDecodingStat statistics; // link decoding statistics
//  ChipStat chipStat;              // chip decoding statistics
//  RUDecodeData* ruPtr = nullptr;  // pointer on the parent RU

  PayLoadSG rawData;         // scatter-gatter buffer for cached CRU pages, each starting with RDH
  size_t dataOffset = 0;     //

  //------------------------------------------------------------------------

  GBTLink() = default;
  GBTLink(uint16_t _flx, uint16_t _fee);
//  std::string describe() const;
  void clear(bool resetStat = true, bool resetTFRaw = false);

//  template <class Mapping>
  CollectedDataStatus collectROFCableData(/*const Mapping& chmap*/);

  void cacheData(const void* ptr, size_t sz)
  {
    rawData.add(reinterpret_cast<const PayLoadSG::DataType*>(ptr), sz);
  }


//  ClassDefNV(GBTLink, 1);
};

///_________________________________________________________________
/// collect cables data for single ROF, return number of real payload words seen,
/// -1 in case of critical error
//template <class Mapping>
inline GBTLink::CollectedDataStatus GBTLink::collectROFCableData(/*const Mapping& chmap*/)
{
  status = None;
//  //verbosity = VerboseHeaders;
//  //LOG(info)<<"entering collectROFCableData";
  auto* currRawPiece = rawData.currentPiece();
  while (currRawPiece)
  { // we may loop over multiple CRU page
    if (dataOffset >= currRawPiece->size)
    {
      dataOffset = 0;                              // start of the RDH
      if ( ! (currRawPiece = rawData.nextPiece()) )
      { // fetch next CRU page
        break;                                     // Data chunk (TF?) is done
      }
    }
    if (!dataOffset)
    { // here we always start with the RDH
      auto hbfEntrySav = hbfEntry;
      hbfEntry = 0xffffffff; // in case of problems with RDH, dump full TF
      mvtx_utils::RdhExt_t rdh = {};
      rdh.decode(&currRawPiece->data[dataOffset]);
      hbfEntry = hbfEntrySav; // critical check of RDH passed
      lastRDH = &rdh;
      dataOffset += 2 * mvtx_utils::FLXWordLength;
    }
    return (status = DataSeen);
  }
  return  (status = StoppedOnEndOfData);
}

} // namespace mvtx

#endif // _MVTX_DECODER_ITSMFT_GBTLINK_H_
