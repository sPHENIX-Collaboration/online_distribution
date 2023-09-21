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

// @file GBTWord.h
// @brief Classes for creation/interpretation of MVTX GBT data
// @sa <O2/Detectors/ITSMFT/common/reconstruction/include/ITSMFTReconstruction/GBTWord.h>
//     <d03b22564>

#ifndef MVTXDECODER_GBTWORD_H
#define MVTXDECODER_GBTWORD_H

#include <cstdint>
#include <string>

namespace mvtx
{

constexpr uint64_t LANESMask = (0x1 << 9) - 1; // at most 9 lanes

/// GBT payload header flag
constexpr uint8_t GBTFlagIHW = 0xe0;
/// GBT trigger status word flag
constexpr uint8_t GBTFlagTDH = 0xe8;
/// GBT payload trailer flag
constexpr uint8_t GBTFlagTDT = 0xf0;
/// GBT diagnostic status word flag
constexpr uint8_t GBTFlagDiagnostic = 0xe4;
/// GBT calibration status word flag
constexpr uint8_t GBTFlagCalibration = 0xf8;

// GBT header flag in the RDH
constexpr uint8_t GBTFlagRDH = 0x00;

// GBT header flag for the ITS IB: 001 bbbbb with bbbbb -> Lane Number (0-8)
constexpr uint8_t GBTFlagDataIB = 0x20;

// GBT header flag for the ITS IB idagnostic : 101 bbbbb with bbbbb -> Lane Number (0-8)
constexpr uint8_t GBTFlagDiagnosticIB = 0xa0;

// GBT header flag for the ITS Status word : 111 bbbbb with bbbbb -> Lane Number
constexpr uint8_t GBTFlagStatus = 0xe0;

constexpr int GBTWordLength = 10;       // lentgh in bytes

struct GBTWord {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
  /// GBT word of 80 bits, bits 72:79 are reserver for GBT Header flag, the rest depends on specifications
  union {
    struct {
      uint64_t activeLanes : 28; /// 0:27   Bit map of lanes active and eligible for readout
      uint64_t na0hn : 36;       /// 28:71  reserved
      uint64_t na1hn : 8;        /// 28:71  reserved
      uint64_t id : 8;           /// 72:79  0xe0; Header Status Word (HSW) identifier
    };                           // ITS HEADER WWORD (IHW)

    // RS: packing will be needed only if some of the members cross 64 bit boundary
    struct __attribute__((packed)) {
      uint64_t triggerType : 12; /// 0:11   12 lowest bits of trigger type received from CTP
      uint64_t internal : 1;     /// 12     Used in Continuous Mode for internally generated trigger
      uint64_t noData : 1;       /// 13     No data expected (too close to previous trigger or error)
      uint64_t continuation : 1; /// 14     following data is continuation of the trigger from the previous CRU page
      uint64_t na1tr : 1;        /// 15     reserved
      uint64_t bc : 12;          /// 16:27  Trigger LHC BC (40MHz) from RU
      uint64_t na2tr : 4;        /// 28:31  reserved
      uint64_t bco : 40;         /// 32:71  sPHENIX GTM BCO
//      uint8_t  id : 8;           /// = 0xe8; Trigger Data Header (TDH) identifier
    }; // TRIGGER DATA HEADER (TDH)

    struct /*__attribute__((packed))*/ {
      uint64_t lanesStatus : 56;        /// 0:55  Bit map of “Valid Lane stops received”, 1 bit per lane, NOT USED
      uint64_t na0t : 5;                /// 56:60 reserved
      uint64_t timeout_in_idle : 1;     /// 61  = 1 if timeout waiting for a valid word from lanes
      uint64_t timeout_start_stop : 1;  /// 62  = 1 if timeout waiting for end-of-packet from all lanes
      uint64_t timeout_to_start : 1;    /// 63  = 1 if timeout waiting for first word from first lane
      uint64_t packetDone : 1;          /// 64  = 1 when current trigger packets transmission done
      uint64_t transmissionTimeout : 1; /// 65  = 1 if timeout while waiting for data on lanes
      uint64_t na1t : 1;                /// 66  reserved
      uint64_t laneStartsViolation : 1; /// 67  = 1 if at least 1 lane (eligible for readout) had a “start violation”
      uint64_t lane_timeouts : 1;       /// 68  = 1 if at least 1 lane (eligible for readout) timed out
      uint64_t na2t : 3;                /// 69:71  reserved
//        uint8_t  id : 8;                /// = 0xf0; Trigger Data Trailer (TDT) identifier
    }; // TRAILER


    struct __attribute__((packed)) {
      uint64_t calibUserField : 48; /// 0:47   user field
      uint64_t calibCounter : 24;   /// 48:71  elf-incrementing counter of
      //  uint64_t id : 8;          /// 72:79  0xf8; Calibration Status Word (HSW) identifier
    }; /// Calibration Data Word

    struct /*__attribute__((packed))*/ {
      uint64_t lane_status : 56;         /// 0:55  Readout status of the lanes for the current HBF.
                                         ///       Higher status level achieved for each lane taken from each TDT.
      uint64_t na3tr : 8;                /// 56:63 reserved
      uint64_t na4tr : 1;                /// 64    reserved
      uint64_t transmission_timeout : 1; /// 65    = 1 One or more lanes had a timeout in this HBF.
                                         ///       Cummulative logic OR of transmission_timeout field of the TDTs.
      uint64_t na5tr : 1;                /// 66    reserved
      uint64_t lane_start_violation : 1; /// 67    = 1 One or more lanes had a protocol violation in this HBF.
                                         ///       Cummulative logic OR of transmission_timeout field of the TDTs.
      uint64_t index : 4;                /// 68:71 Must be = 0 DDW0
      //      uint64_t id : 8;           /// 72:79  0xe4;  Status Word (HSW) identifier
    }; // HEADER Legacy

    uint8_t data8[GBTWordLength]; // 80 bits GBT word
  };
#pragma GCC diagnostic pop

  GBTWord() = default;

  /// check if the GBT Header corresponds to GBT payload header
  bool isIHW() const { return id == GBTFlagIHW; }

  /// check if the GBT Header corresponds to GBT payload trailer
  bool isTDH() const { return id == GBTFlagTDH; }

  /// check if the GBT Header corresponds to GBT trigger word
  bool isTDT() const { return id == GBTFlagTDT; }

  /// check if the GBT Header corresponds to Diagnostic data
  bool isDiagnosticWord() const { return id == GBTFlagDiagnostic; }

  /// check if the GBT Header corresponds to Calibration word
  bool isCalibrationWord() const { return id == GBTFlagCalibration; }

  /// check if the GBT Header corresponds to ITS IB data (header is combined with lanes info)
  bool isData() const { return (id & 0xe0) == GBTFlagDataIB; }

  /// check if the GBT Header corresponds to ITS IB diagnostics data (header is combined with lanes info)
  bool isCableDiagnostic() const { return (id & 0xe0) == GBTFlagDiagnosticIB; }

  bool isStatus() const { return (id & 0xe0) == GBTFlagStatus; }

  const uint8_t* getW8() const { return data8; }

  uint8_t getHeader() const { return id; }

  void printX() const;
  void printB() const;

  std::string asString() const;

//  ClassDefNV(GBTWord, 1);
};

} // namespace mvtx

#endif
