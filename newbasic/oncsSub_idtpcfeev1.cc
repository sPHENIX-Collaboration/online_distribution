#include "oncsSub_idtpcfeev1.h"

#include <string.h>
#include <stdint.h>
#include <stdint.h>

using namespace std;

oncsSub_idtpcfeev1::oncsSub_idtpcfeev1(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{

 _nsamples = 0;
 _nchannels = 0;
 _nchips = 0;
 _is_decoded = 0;

 memset(array, 0, 64*1002*sizeof(uint32_t) );

 
}


oncsSub_idtpcfeev1::~oncsSub_idtpcfeev1()
{
  //  if (array) delete [][] array;
}


#define HEADER_LENGTH 7U

int oncsSub_idtpcfeev1::decode ()
{

  if (_is_decoded ) return 0;
  _is_decoded = 1;

  int recv_length = getLength() - SEVTHEADERLENGTH - getPadding();

  _nsamples    = 0;
  _nchannels   = 32;

  uint32_t *buffer = ( uint32_t *)  &SubeventHdr->data;
  unsigned int payload_len = buffer[1] & 0xffff;

  _bx_count = ((buffer[4] & 0xffff) << 4) | (buffer[5] & 0xf);  
  
  for (int i = 0; i < recv_length; i += (payload_len+1))
    {
      payload_len = (buffer[i+1] & 0xffff);
      unsigned int channel     = buffer[i+3] & 0x1f;
      unsigned int  sampa_addr  = (buffer[i+3] >> 5) & 0xf;
      
      if ( sampa_addr > 1)
	{
	  _broken = 1;
	  return 0;
	}
      if ( channel >= 32 )
	{
	  _broken = 2;
	  return 0;
	}
      
      if ( sampa_addr > _nchips)
	{
	  _nchips = sampa_addr;
	}
      for  ( unsigned int s = 0; s < payload_len - HEADER_LENGTH; s++)
	{
	  array[sampa_addr*32 + channel][s] =  buffer[i+s+HEADER_LENGTH] & 0xffff;
	}
      _nsamples = payload_len - HEADER_LENGTH;
    }
  
  _nchips++;
  
  return 0;
}


int oncsSub_idtpcfeev1::iValue(const int ch, const int sample)
{
     decode();

     if ( sample >= (int) _nsamples || sample < 0 
	   || ch >= (int) (_nchips * _nchannels) || ch < 0 ) return 0;
     
  return array[ch][sample];

}
int oncsSub_idtpcfeev1::iValue(const int chip, const int ch, const int sample)
{
  if ( sample >= (int)_nsamples || sample < 0 
       || chip >= (int) _nchips || chip < 0  
       || ch >= (int) _nchannels || ch < 0 ) return 0;
     
     return iValue(chip*32 + ch, sample);

}

int oncsSub_idtpcfeev1::iValue(const int n, const char *what)
{

  decode();

  if ( strcmp(what,"SAMPLES") == 0 )
  {
    return _nsamples;
  }

  if ( strcmp(what,"CHANNELS") == 0 )
  {
    return _nchannels;
  }

  if ( strcmp(what,"CHIPS") == 0 )
  {
    return _nchips;
  }

  if ( strcmp(what,"BROKEN") == 0 )
  {
    return _broken;
  }

  if ( strcmp(what,"BCTR") == 0 )
  {
    return _bx_count;
  }

  return 0;

}

void  oncsSub_idtpcfeev1::dump ( OSTREAM& os )
{
  identify(os);

  os << "Chips:         " << iValue(0,"CHIPS") << std::endl;
  os << "Channels:      " << iValue(0,"CHANNELS") << std::endl;
  os << "Samples:       " << iValue(0,"SAMPLES") << std::endl;
  os << "Beam Crossing: " << iValue(0,"BCTR") << std::endl;
  os << endl;

  for ( int chip = 0; chip < iValue(0,"CHIPS"); chip++)
    {
      cout << " chip sample        +++++++++++++++ Chip " << chip << " ++++++++" << endl;
      for ( int s = 0; s < iValue(0,"SAMPLES"); s++)
	{

	  os << setw(3) << chip << setw(5) << s << " | ";

	  for ( int c = 0; c < iValue(0,"CHANNELS"); c++)
	    {
	      os << setw(4) << iValue(chip, c, s) << " ";
	    }
	  os << endl;
	}
      os << endl;
    }
  
}
