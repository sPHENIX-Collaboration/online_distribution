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

// @file GBTWord.cxx
// @brief Classes for creation/interpretation of MVTX GBT data
// @sa <O2/Detectors/ITSMFT/common/reconstruction/src/GBTWord.cxx>
//     <1ecb2b0a2>

#include "mvtx_decoder/GBTWord.h"
#include <sstream>
#include <iostream>

using namespace mvtx;

//________________________________________________________________________________
void GBTWord::printX() const
{
  /// print in right aligned hex format
  std::cout << std::hex;
  //%02x %02x %02x %02x %02x %02x %02x %02x %02x",
  //     data8[9], data8[8], data8[7], data8[6], data8[5],
  //     data8[4], data8[3], data8[2], data8[1], data8[0]);
  std::cout << std::dec << std::endl;
}

//________________________________________________________________________________
std::string GBTWord::asString() const
{
  std::stringstream ss;
  for (int i = GBTWordLength; i--;)
  {
    uint8_t v = data8[i];
    ss << ' ' << std::hex << v;
  }
  return ss.str();
}

//________________________________________________________________________________
void GBTWord::printB() const
{
  /// print in bitset format
  std::stringstream ss;
  for (int i = GBTWordLength; i--;)
  {
    uint8_t v = data8[i];
    ss << ' ';
    for (int j = 8; j--;) {
      ss << ((v & (0x1 << j)) ? '1' : '0');
    }
  }
  std::cout << "0b: " << ss.str() << std::endl;
}
