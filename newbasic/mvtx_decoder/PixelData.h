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

// @file PixelData.h
// @brief Transient data classes for single pixel and set of pixels from current chip
// @sa <O2/Detectors/ITSMFT/common/reconstruction/include/ITSMFTReconstruction/PixelData.h>
//     <0ec4d22ed>

#ifndef MVTXDECODER_PIXELDATA_H
#define MVTXDECODER_PIXELDATA_H

#include "mvtx_decoder/DecodingStat.h"
#include <string>
#include <vector>
#include <utility>
#include <cstdint>

namespace mvtx
{
///< single pixel datum, with possibility to set a flag of pixel being masked out
class PixelData
{

 public:
  PixelData(uint16_t r = 0, uint16_t c = 0) : mRow(r), mCol(c) {}
  uint16_t getRow() const { return mRow; }
  uint16_t getCol() const { return mCol; }

 private:
  void sanityCheck() const;
  uint16_t mRow = 0;                    ///< pixel row
  uint16_t mCol = 0;                    ///< pixel column

  // ClassDefNV(PixelData, 1);
};

///< Transient data for single chip fired pixeld
///< Assumes that the digits data is sorted in chip/col/row
class ChipPixelData
{

 public:
  // total number of raw data bytes to save in case of error and number of bytes (if any) after problematic one
  static constexpr size_t MAXDATAERRBYTES = 16, MAXDATAERRBYTES_AFTER = 2;
  ChipPixelData() = default;
  ~ChipPixelData() = default;
//  uint8_t getROFlags() const { return mROFlags; }
  uint16_t getChipID() const { return mChipID; }
//  uint32_t getROFrame() const { return mROFrame; }
  const std::vector<PixelData>& getData() const { return mPixels; }
  std::vector<PixelData>& getData() { return (std::vector<PixelData>&)mPixels; }

//  void setROFlags(uint8_t f = 0) { mROFlags = f; }
  void setChipID(uint16_t id) { mChipID = id; }
//  void setROFrame(uint32_t r) { mROFrame = r; }

  void setError(ChipStat::DecErrors i) { mErrors |= 0x1 << i; }
  void addErrorInfo(uint64_t b) { mErrorInfo |= b; }
  void setErrorFlags(uint32_t f) { mErrors |= f; }
  bool isErrorSet(ChipStat::DecErrors i) const { return mErrors & (0x1 << i); }
  bool isErrorSet() const { return mErrors != 0; }
  auto getErrorFlags() const { return mErrors; }
  auto getErrorInfo() const { return mErrorInfo; }
  auto getNBytesInRawBuff() const { return int(mErrorInfo >> 32) & 0xff; }
  void setNBytesInRawBuff(int n) { mErrorInfo |= (uint64_t(n & 0xff)) << 32; }
  auto& getRawErrBuff() { return mRawBuff; }
  auto& getRawErrBuff() const { return mRawBuff; }
  std::string getErrorDetails(int pos) const;

  void resetChipID()
  {
    mChipID = -1;
  }

  void clear()
  {
    resetChipID();
    mPixels.clear();
//    mROFlags = 0;
    mErrors = 0;
    mErrorInfo = 0;
  }

  void swap(ChipPixelData& other)
  {
    // swap content of two objects
//    std::swap(mROFlags, other.mROFlags);
    std::swap(mChipID, other.mChipID);
//    std::swap(mROFrame, other.mROFrame);
    std::swap(mErrors, other.mErrors);
    mPixels.swap(other.mPixels);
  }

  void print() const;

 private:
//  uint8_t mROFlags = 0;                            // readout flags from the chip trailer
  uint16_t mChipID = 0;                            // chip id within the detector
//  uint32_t mROFrame = 0;                           // readout frame ID
  uint32_t mErrors = 0;                            // errors set during decoding
  uint64_t mErrorInfo = 0;                         // optional extra info on the error
  std::array<uint8_t, MAXDATAERRBYTES> mRawBuff{}; // buffer for raw data showing an error
  std::vector<PixelData> mPixels;                  // vector of pixels

  // ClassDefNV(ChipPixelData, 1);
};
} // namespace mvtx

#endif // MVTXDECODER_PIXELDATA_H
