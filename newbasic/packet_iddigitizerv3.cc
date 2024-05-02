#include "packet_iddigitizerv3.h"

#include <string.h>

using namespace std;


#define coutfl std::cout << __FILE__<< "  " << __LINE__ << " "
#define cerrfl std::cerr << __FILE__<< "  " << __LINE__ << " "


Packet_iddigitizerv3::Packet_iddigitizerv3(PACKET_ptr data)
  :Packet_w4 (data)
{

  _broken = 0;
  
  if ( getHitFormat() > 160 && getHitFormat() <= 190)
    {
      _nsamples = getHitFormat() - 160;
    }
  _nchannels = 0;
  _nr_modules = 0;
  
  _evtnr = 0;
  _module_address  = 0;
  _xmit_clock = 0;
  _AnyChannelisSuppressed = false;  // we use this later to see if the checksum can be calculated
  
  for ( int i = 0; i < NR_FEMS; i++)
    {
      _fem_slot[i] = 0;
      _fem_evtnr[i] = 0;
      _fem_clock[i] = 0;
      _fem_checksum_MSB[i] = 0;
      _fem_checksum_LSB[i] = 0;
      _fem_calculated_checksum_MSB[i] = 0;
      _fem_calculated_checksum_LSB[i] = 0;

    }
  
  _is_decoded = 0;
  
}


Packet_iddigitizerv3::~Packet_iddigitizerv3()
{

}

int Packet_iddigitizerv3::decode ()
{

  if (_is_decoded ) return 0;
  _is_decoded = 1;
  
  unsigned int *k;


  // check later  int dlength = ( getLength()-4) - getPadding();

  unsigned int *SubeventData = (unsigned int *) findPacketDataStart(packet); 

  int dlength = getDataLength();
  
  k = SubeventData;  // we make a shorter variale for less typing..

  
  for (int i = 0; i < 32; i++)
    {
      for (int j = 0; j < 4*64; j++)
	{
	  adc[i][j] = 0;
	}
    }

  if ( dlength < 6 )
    {
      _broken = 1;
      return 0;  //safety belt
    }
  
  // we have here - 
  //  [0]  0x8000xxxx  with xxxx = evt nr
  //  [1]  0x9000xxxx  with xxxx = flag word
  //  [2]  0x9000xxxx  with xxxx = det id
  //  [3]  0x9000xxxx  with xxxx = module address
  //  [4]  0x9000xxxx  with xxxx = xmit clock nr
  
  if ( (k[0] >> 28) != 0x8)  return 1;
  if ( (k[1] >> 28) != 0x9)  return 1;
  if ( (k[2] >> 28) != 0x9)  return 1;
  if ( (k[3] >> 28) != 0x9)  return 1;
  if ( (k[4] >> 28) != 0x9)  return 1;

  _evtnr          = (k[0] & 0xffff);
  _clock_rollover = (k[1] & 0xffff);
  _evtrollover    = (k[2] & 0xffff);
  _module_address = (k[3] & 0xffff);
  _xmit_clock     = (k[4] & 0xffff);

  _evtnr = (_evtrollover << 16) + _evtnr;
  _xmit_clock = (_clock_rollover << 16) + _xmit_clock;

  int fem_nr = 0;
  
  for (int fem_index = 5; fem_index < dlength ; )  // we hop from FEM to FEM here
    {

      if ( k[fem_index] == 0xa000ffff)
	{
	  // coutfl << " FEM_ start at  " << fem_index << "   " << hex << k[fem_index] << dec  << endl;
	  
	  unsigned int i = decode_FEM (  &k[fem_index], fem_nr, dlength - fem_index); // we make "i" so we can debug
	  if ( i == 0)
	    {
	      //coutfl << " strange return " << i << endl;
	      _broken = 2;
	      break;  // no endless loops...
	    }
	  fem_nr++;
	  if ( fem_nr >= NR_FEMS)
	    {
	      _broken = 3;
	      return 1;
	    }
	  fem_index += i;
	  //coutfl << " fem_index now  " << fem_index << "  " << hex <<  k[fem_index] << dec << endl;
	}
    }

  
  return 0;
}


unsigned int Packet_iddigitizerv3::decode_FEM ( unsigned int *k, const int fem_nr, const int len)
{
  int NFEM = fem_nr;
  int CHNL = 0;
  int index_channel = NFEM*64;
  int index_sample = 0;
  int index_parity = 0;

  //  coutfl << " calling decode_FEM for fem " << fem_nr << endl << endl;

  
  // should have been checked but let's be sure
  if ( k[0] != 0xa000ffff) return 0;
  
  _fem_slot[fem_nr]  = k[1] & 0xffff;
  _fem_evtnr[fem_nr] = k[2] & 0xffff;
  _fem_clock[fem_nr] = k[3] & 0xffff;

  _nr_modules++;
  _nchannels += 64;  

  int index;
  int corrected_index_channel;
  // for ( index = 1; index < 4; index++)
  //   {
  //     _fem_calculated_checksum_MSB[NFEM] ^= ((k[index] >> 16 ) & 0xffff);
  //     _fem_calculated_checksum_LSB[NFEM] ^= (k[index] & 0xffff);
  //   }



  for ( index = 4; index < len;)
    {
      int word_classifier = k[index] >> 28;
      
      if ( (k[index] >> 28) == 0xe) // ah! another "0xe" channel
	{
	  corrected_index_channel = index_channel^1;
	  isZeroSuppressed[corrected_index_channel] = true;
	  _AnyChannelisSuppressed = true;
	  pre_post[0][corrected_index_channel] = (k[index] & 0x3fff);
	  pre_post[1][corrected_index_channel] = ((k[index]>>14) & 0x3fff);
	  // coutfl << " found a zp-word  " << hex << "0x" << k[index]  << dec << " at index " << index
	  // 	 << " for channel " << index_channel << "  " << isZeroSuppressed[index_channel] << " **** zero-suppressed " <<  endl;
	  index++;
	  index_channel++;
	}

      else if ( word_classifier == 0x1) // FEM/Ch address
	{
	  // _fem_calculated_checksum_MSB[NFEM] ^= ((k[index] >> 16 ) & 0xffff);
	  // _fem_calculated_checksum_LSB[NFEM] ^= (k[index] & 0xffff);

	  NFEM = (k[index] >> 6) & 0x7;
	  CHNL   = k[index] & 0x3f;
	  //	  coutfl << "new NFEM and channel - " << NFEM << " " << CHNL << "   "  << hex << k[index] << dec << endl;
	  if (NFEM != fem_nr || NFEM >= NR_FEMS)
	    {
	      coutfl << "NFEM and fem_nr differ - " << NFEM << " " << fem_nr << "  " << hex << k[index] << dec << endl;
	    }
	  // we have a new channel. reset the sample and parity index, and set the channel index
	  index_sample = 0;
	  index_parity = 0;  
	  index_channel = NFEM*64+CHNL;
	  corrected_index_channel = index_channel^1;
	  // we assume we are zero-suppressed and reset this if we find later that we are not
	  //	  isZeroSuppressed[index_channel] = true;

	  if ( (k[index+1] >> 28) == 0x3) // we have a full waveform
	    {
	      index++;  //move on to that word
	      for ( int c = 0; c < ( _nsamples/2 + 1) ; c++)  // like for 12 samples we expect 7 words
		{
		  if ( (k[index] >> 28) == 0x3)  // normal word
		    {
		      // coutfl << " found a normal word   " << hex << "0x" << k[index]  << dec << " at index " << index
		      // 	     << " for channel " << index_channel << " and " << index_sample  << endl;
		      adc[index_sample++][corrected_index_channel] = k[index] & 0x3fff;
		      adc[index_sample++][corrected_index_channel] = (k[index] >> 14) & 0x3fff;
		      isZeroSuppressed[corrected_index_channel] = false; // we are ot zero-sppressed
		      // coutfl << " setting not zp  for channel " << index_channel  << " " << isZeroSuppressed[index_channel] << endl;

		    }
		  else if ( (k[index] >> 28) == 0xe) // here is our regular pre-post word
		    {
		      pre_post[0][corrected_index_channel] = (k[index] & 0x3fff);
		      pre_post[1][corrected_index_channel] = ((k[index]>>14) & 0x3fff);
		      // coutfl << " found a regular zp-word  " << hex << "0x" << k[index]  << dec << " at index " << index
		      // 	     << " for channel " << index_channel << endl;
		    }
		  else
		    {
		      // scream if that's not 0x3 nor 0xe
		      coutfl << "wrong tag! " << hex << (k[index] >> 28) << dec << " at index " << index << endl;
		    }
		  index++;
		}
	    }

	  // just in case we get a zero-suppressed word, step the index_channel
	  index_channel++;
	}
      
      else if ( word_classifier == 0xb)   // parity info
	{
	  if ( index_parity == 0)
	    {
	      _fem_checksum_MSB[NFEM] = (k[index] & 0xffff);
	      index_parity++;
	      index++;
	    }
	  else
	    {
	      _fem_checksum_LSB[NFEM] = (k[index] & 0xffff);
	      index++;
	      break;
	    }
	  if ( index >= len) break;
	}
    }
  return index;
}


int Packet_iddigitizerv3::iValue(const int sample, const int ch)
{
  decode();

  
  if ( sample >= _nsamples || sample < 0 
       || ch >= _nchannels || ch < 0 ) return 0;
 
  return adc[sample][ch];
  return 0;
}

long long Packet_iddigitizerv3::lValue(const int n, const char *what)
{

  decode();

  if ( strcmp(what,"CLOCK") == 0 )
  {
    return _xmit_clock;
  }
  return 0;
}


int Packet_iddigitizerv3::iValue(const int n, const char *what)
{

  decode();

  // if ( strcmp(what,"CLOCK") == 0 )
  // {
  //   return _xmit_clock;
  // }

  if ( strcmp(what,"EVTNR") == 0 )
  {
    return _evtnr;
  }

  if ( strcmp(what,"SAMPLES") == 0 )
  {
    return _nsamples;
  }

  if ( strcmp(what,"NRMODULES") == 0 )
  {
    return _nr_modules;
  }


  if ( strcmp(what,"CHANNELS") == 0 )
  {
    return _nchannels;
  }

  if ( strcmp(what,"MODULEADDRESS") == 0 )
  {
    return _module_address;
  }


  if ( strcmp(what,"FEMSLOT") == 0 )
  {
    if ( n < 0 || n >= _nr_modules) return 0;
    return _fem_slot[n];
  }

  if ( strcmp(what,"FEMEVTNR") == 0 )
  {
    if ( n < 0 || n >= _nr_modules) return 0;
    return _fem_evtnr[n];
  }

  if ( strcmp(what,"FEMCLOCK") == 0 )
  {
    if ( n < 0 || n >= _nr_modules) return 0;
    return _fem_clock[n];
  }

  if ( strcmp(what,"CHECKSUMMSB") == 0 )
  {
    if ( n < 0 || n >= _nr_modules) return 0;
    return _fem_checksum_MSB[n];
  }

  if ( strcmp(what,"CHECKSUMLSB") == 0 )
  {
    if ( n < 0 || n >= _nr_modules) return 0;
    return _fem_checksum_LSB[n];
  }

  if ( strcmp(what,"CALCCHECKSUMMSB") == 0 )
  {
    if ( n < 0 || n >= _nr_modules) return 0;
    return _fem_calculated_checksum_MSB[n];
  }

  if ( strcmp(what,"CALCCHECKSUMLSB") == 0 )
  {
    if ( n < 0 || n >= _nr_modules) return 0;
    return _fem_calculated_checksum_LSB[n];
  }

  // to maintain compatibility with v2, we provide those interfaces
  // but we return -1 for "cannot be determined"
  if ( strcmp(what,"EVENCHECKSUMOK") == 0 )
  {
    return -1;
  }

  if ( strcmp(what,"ODDCHECKSUMOK") == 0 )
  {
    return -1;
  }
  
  if ( strcmp(what,"CHECKSUMOK") == 0 )
  {
    return -1;
  }
  
  if ( strcmp(what,"PRE") == 0 )
  {
    if ( n < 0 || n >= _nchannels ) return 0;
    return pre_post[0][n];
  }

  if ( strcmp(what,"POST") == 0 )
  {
    if ( n < 0 || n >= _nchannels ) return 0;
    return pre_post[1][n];
  }

  if ( strcmp(what,"SUPPRESSED") == 0 )
  {
    if ( n < 0 || n >= _nchannels ) return 0;
    if ( isZeroSuppressed[n] ) return 1;
    return 0;
  }


  return 0;

}

void  Packet_iddigitizerv3::dump ( OSTREAM& os )
{
  identify(os);

  if ( _broken)
    {
      os << " *** Corrupt packet "  << std::endl;
      return;
    }
  
  os << "Evt Nr:      " << iValue(0,"EVTNR") << std::endl;
  os << "Clock:       " << lValue(0,"CLOCK") << std::endl;
  os << "Nr Modules:  " << iValue(0,"NRMODULES") << std::endl;
  os << "Channels:    " << iValue(0,"CHANNELS") << std::endl;
  os << "Samples:     " << iValue(0,"SAMPLES") << std::endl;
  //os << "Det. ID:     " << hex << "0x" << iValue(0,"DETID") << dec <<  std::endl;
  os << "Mod. Addr:   " << hex << "0x" << iValue(0,"MODULEADDRESS") << dec << std::endl;

  os << "FEM Slot:    ";
  for ( int i = 0; i < iValue(0,"NRMODULES"); i++) os << setw(8) << iValue(i,"FEMSLOT");
  os <<  std::endl;

  os << "FEM Evt nr:  ";
  for ( int i = 0; i < iValue(0,"NRMODULES"); i++)  os << setw(8) << iValue(i,"FEMEVTNR");
  os << std::endl;

  os << "FEM Clock:   ";
  for ( int i = 0; i < iValue(0,"NRMODULES"); i++)  os << setw(8) << iValue(i,"FEMCLOCK");
  os << std::endl;

  char oldFill=os.fill('0');
  
  os << "FEM Checksum LSB:   ";
  for ( int i = 0; i < iValue(0,"NRMODULES"); i++)
    {
      os <<  "0x" << hex <<  setw(4) << iValue(i,"CHECKSUMLSB") << "  "  << dec;
    }
  os << std::endl;

  // os << "Calculated Checksum LSB:   ";
  // for ( int i = 0; i < iValue(0,"NRMODULES"); i++)
  //   {
  //     os <<  "0x" << hex <<  setw(4) << iValue(i,"CALCCHECKSUMLSB") << "  "  << dec;
  //   }
  // os << std::endl;

  os << "FEM Checksum MSB:   ";
  for ( int i = 0; i < iValue(0,"NRMODULES"); i++)
    {
      os <<  "0x" << hex << setw(4) << iValue(i,"CHECKSUMMSB")  << "  "<< dec;
    }
  os << std::endl;

  // os << "Calculated Checksum MSB:   ";
  // for ( int i = 0; i < iValue(0,"NRMODULES"); i++)
  //   {
  //     os <<  "0x" << hex <<  setw(4) << iValue(i,"CALCCHECKSUMMSB") << "  "  << dec;
  //   }
  // os << std::endl;

  os.fill(oldFill);
  os << endl;

  for ( int c = 0; c < _nchannels; c++)
    {
      if (  iValue(c,"SUPPRESSED") )
	{
	  os << setw(4) << c << " |-";
	}
      else
	{
	  os << setw(4) << c << " | ";
	}

	  
      os << hex;

      os << setw(6) << iValue(c, "PRE");
      os << setw(6) << iValue(c, "POST") << " | " ;

      if ( ! iValue(c,"SUPPRESSED") )
	{
	  for ( int s = 0; s < _nsamples; s++)
	    {
	      os << setw(6) << iValue(s,c);
	    }
	}
      os << dec << endl;
    }

}
