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

// @sa <O2/DataFormats/common/src/InteractionRecord.cxx>
//     <03608ff89>

#include "mvtx_decoder/InteractionRecord.h"
#include <iostream>
#include <sstream>

namespace mvtx
{

//________________________________________________________________________________
std::string InteractionRecord::asString() const
{
  if ( isDummy() )
  {
    return std::string{"NotSet"};
  }

  std::stringstream ss;
  ss << std::hex << "Orbit: 0x" << orbit << std::dec << "  BCid: " << bc;
  return ss.str();
}

//________________________________________________________________________________
std::ostream& operator<<(std::ostream& stream, mvtx::InteractionRecord const& ir)
{
  stream << ir.asString();
  return stream;
}

//________________________________________________________________________________
void InteractionRecord::print() const
{
  std::cout << (*this) << std::endl;
}

} // namespace mvtx
