// @file PixelData.cxx
// @brief Implementation for transient data of single pixel and set of pixels
// from current chip
// @sa <O2/Detectors/ITSMFT/common/reconstruction/src/PixelData.cxx>
//     <d44292025>

#include "PixelData.h"

#include <sstream>

using namespace mvtx;

//________________________________________________________________________________
void ChipPixelData::print() const
{
  // print chip data
  printf("Chip %d | %4lu hits\n", mChipID, mPixels.size());
  for (std::size_t i = 0; i < mPixels.size(); i++)
  {
    printf("#%4ld C:%4d R: %3d\n", i, mPixels[i].getCol(), mPixels[i].getRow());
  }
}

//________________________________________________________________________________
std::string ChipPixelData::getErrorDetails(int pos) const
{
  // if possible, extract more detailed info about the error
  if (pos == int(ChipStat::RepeatingPixel))
  {
    std::stringstream ss;
    ss << ": " << (mErrorInfo & 0xffff) << "/" << ((mErrorInfo >> 16) & 0xffff);
    return ss.str();
  }
  if (pos == int(ChipStat::UnknownWord))
  {
    std::string rbuf = ": 0x<";
    int nc = getNBytesInRawBuff();
    for (int i = 0; i < nc; i++)
    {
      std::stringstream ss;
      ss << (i ? " " : "") << std::hex << (int) getRawErrBuff()[i];
    }
    rbuf += '>';
    return rbuf;
  }
  return {};
}
