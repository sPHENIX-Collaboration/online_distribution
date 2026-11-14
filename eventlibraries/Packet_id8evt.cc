#include "Packet_id8evt.h"

#include <string.h>

Packet_id8evt::Packet_id8evt(subevtdata_ptr data)
  :Packet_w8 (data){}

// the padding is counted in 8-byte words
int Packet_id8evt::getNrValues() const
{
  return ( getLength() - SEVTHEADERLENGTH ) / 2 - getPadding();
}

long long Packet_id8evt::lValue(const int i)
{
  if ( i < 0 || i >= getNrValues() ) return 0;
  long long *SubeventData = (long long *) &SubeventHdr->data;
  return SubeventData[i];
}

int Packet_id8evt::iValue(const int i, const char *what)
{
  if ( strcmp(what, "CHANNELS") == 0) return getNrValues();
  return 0;
}

int Packet_id8evt::getArraylength(const char *what)
{
  return getNrValues();
}
