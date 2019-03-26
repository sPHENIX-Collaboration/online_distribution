#include "packet_iddigitizerv2.h"

#include <string.h>

using namespace std;

Packet_iddigitizerv2::Packet_iddigitizerv2(PACKET_ptr data)
  :Packet_w4 (data)
{

 _nsamples = 0;

 _evtnr = 0;
 _detid = 0;
 _module_address  = 0;
 _clock = 0;
 for ( int i = 0; i < 4; i++)
   {
     _fem_slot[i] = 0;
     _fem_evtnr[i] = 0;
     _fem_clock[i] = 0;
   }

 _even_checksum = 0;
 _odd_checksum = 0;

  _calculated_even_checksum = 0;
  _calculated_odd_checksum = 0;

  _even_checksum_ok = -1;  // -1 convention: "cannot be evaluated", typically for 0-supp. data 
  _odd_checksum_ok = -1;   // else ok =1, not ok 0.

 _nchannels = 0;
 _is_decoded = 0;


}


Packet_iddigitizerv2::~Packet_iddigitizerv2()
{
  //  if (array) delete [][] array;
}



const int offset=9;
const int samples=12;
const int channels=64;

  
int Packet_iddigitizerv2::decode ()
{

  if (_is_decoded ) return 0;
  _is_decoded = 1;


  int *k;


  // check later  int dlength = ( getLength()-4) - getPadding();

  int *SubeventData = (int *) findPacketDataStart(packet); 

  switch ( getHitFormat() )
    {
    case IDDIGITIZER_12S:
      _nsamples    = 12;
      break;

    case IDDIGITIZER_16S:
      _nsamples    = 16;
      break;

    default:
      _nsamples    = 31;
      break;
    }


  _evtnr           =  SubeventData[0]  & 0xffff;
  _detid           =  SubeventData[2]  & 0xffff;

  _module_address  = SubeventData[3] & 0xffff;
  _clock           = SubeventData[4] & 0xffff;

  _fem_slot[0]        = SubeventData[6] & 0xffff;
  _fem_evtnr[0]       = SubeventData[7] & 0xffff;
  _fem_clock[0]       = SubeventData[8] & 0xffff;

  //  _l1_delay    = (SubeventData[0] >> 16 ) & 0xff;
  _nr_modules =  0;


  int index = getDataLength() -2;
  _even_checksum = SubeventData[index] & 0xffff;
  // cout << __FILE__ << " " << __LINE__  
  //      << hex << SubeventData[index]
  //      << "  " << _even_checksum 
  //      << dec << endl;
  
  index = getDataLength() -1;
  _odd_checksum = SubeventData[index] & 0xffff;

  // cout << __FILE__ << " " << __LINE__  
  //      << hex << SubeventData[index]
  //      << "  " << _odd_checksum 
  //      << dec << endl;
  
  k = &SubeventData[offset];

  int dlength = getDataLength() - 9 -2; // 9 header words and 2 checksums + 1 more word at the end

  for ( int index  = 0; index  < dlength ; index++)
    {
      
      int rawtag = ((k[index] >> 16) & 0x3fff); // just so we can print it...
      int realtag = rawtag -9;

      // now we are getting into some kludginess...
      // if we have more than one card, he xmit injects 6 header words at the beginning.
      // so we are skipping that header here. 

      int module = 0;
      for ( module = 0; module < 4; module++)
	{
	  if ( realtag < (module+1) * 64 * _nsamples + 6*module ) break;
	}

      if ( module +1 > _nr_modules)
	{
	  _nr_modules = module +1;
	  _nchannels =  _nr_modules * 64;
	}

      // from now on, we now have to subtract those, too
      // so for module 0, we substract 0, module 1, 6 and module 2, 12, ...
      int tag = realtag - 6*module;
      
      int value = k[index] & 0x3fff;

      //no quite done... we now have to cut out the extra header words
      // so if we have one of those 6 words, we skip

      if ( module > 0 && realtag < ( module*64*_nsamples + 6*module )  )
	{

	  if ( realtag == module*64*_nsamples +2 + 6*(module -1)  ) //  leftover FEM header word
	    {
	      _fem_slot[module] =  k[index+1]  & 0xff;  // slot number
	      _fem_evtnr[module] = k[index+2]  & 0xffff;  // fem event nr
	      _fem_clock[module] = k[index+3]  & 0xffff;  // fem clock 

	    }
	  
	  //	  cout << __FILE__ << " " << __LINE__ << " skipping module " << module << " realtag: " << realtag << "  " << module*64*_nsamples + 6*module << "  " << hex << value << dec << endl;
	  
	  continue;
	}
      
      
      int channelpair = (tag / _nsamples) & 0xfffe;
      int ch = channelpair;  // even tag -> lower channel
      if ( tag & 1) ch++;    // odd tag -> 2nd channel
      
      int sample = (tag/2) % _nsamples;
      
      // cout << __FILE__ << " " << __LINE__  
      // 	   << " rawtag " << hex << rawtag  
      // 	   << " realtag " << dec << realtag 
      // 	   << " tagword " << dec << tag 
      // 	   << " module  " << dec << module  
      // 	   << " channel " << dec << " " << ch
      // 	   << " sample " << sample 
      // 	   << " value " << hex << "  " << value << dec << endl; 
      
      array[sample][ch]    =  value;
      
    }
  
  
  for ( index = 5; index < getDataLength()-3; index+=2)  // -3 to spare the CS fields out
    {
      _calculated_even_checksum ^= SubeventData[index] & 0xffff;
      _calculated_odd_checksum ^= SubeventData[index+1] & 0xffff;
    }
  
  return 0;
}


int Packet_iddigitizerv2::iValue(const int sample, const int ch)
{
  decode();

  if ( sample >= _nsamples || sample < 0 
       || ch >= _nchannels || ch < 0 ) return 0;

  return array[sample][ch];

}

int Packet_iddigitizerv2::iValue(const int n, const char *what)
{

  decode();

  if ( strcmp(what,"CLOCK") == 0 )
  {
    return _clock;
  }

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

  if ( strcmp(what,"DETID") == 0 )
  {
    return _detid;
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

  if ( strcmp(what,"EVENCHECKSUM") == 0 )
  {
    return _even_checksum;
  }

  if ( strcmp(what,"ODDCHECKSUM") == 0 )
  {
    return _odd_checksum;
  }

  if ( strcmp(what,"CALCEVENCHECKSUM") == 0 )
  {
    return _calculated_even_checksum;
  }

  if ( strcmp(what,"CALCODDCHECKSUM") == 0 )
  {
    return _calculated_odd_checksum;
  }

  if ( strcmp(what,"EVENCHECKSUMOK") == 0 )
  {
    if (  _calculated_even_checksum < 0 ) return -1; // cannot evaluate
    if (  _even_checksum == _calculated_even_checksum) return 1;
    return 0;
  }

  if ( strcmp(what,"ODDCHECKSUMOK") == 0 )
    {
      if (  _calculated_odd_checksum < 0 ) return -1; // cannot evaluate
      if (  _odd_checksum == _calculated_odd_checksum) return 1;
      return 0;
    }

  return 0;

}

void  Packet_iddigitizerv2::dump ( OSTREAM& os )
{
  identify(os);

  os << "Evt Nr:      " << iValue(0,"EVTNR") << std::endl;
  os << "Clock:       " << iValue(0,"CLOCK") << std::endl;
  os << "Nr Modules:  " << iValue(0,"NRMODULES") << std::endl;
  os << "Channels:    " << iValue(0,"CHANNELS") << std::endl;
  os << "Samples:     " << iValue(0,"SAMPLES") << std::endl;
  os << "Det. ID:     " << hex << "0x" << iValue(0,"DETID") << dec <<  std::endl;
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

  os << "Even chksum: 0x" << hex << iValue(0,"EVENCHECKSUM")  << "   calculated:  0x" <<  iValue(0,"CALCEVENCHECKSUM");
  if ( iValue(0,"EVENCHECKSUMOK") == 1) os << " ok" << endl;
  else if ( iValue(0,"EVENCHECKSUMOK") == 0) os << " **wrong" << endl;

  os << "Odd chksum:  0x" << hex << iValue(0,"ODDCHECKSUM")  << "   calculated:  0x" <<  iValue(0,"CALCODDCHECKSUM");
  if ( iValue(0,"ODDCHECKSUMOK") == 1) os << " ok" << endl;
  else if ( iValue(0,"ODDCHECKSUMOK") == 0) os << " **wrong" << endl;
  os << dec << endl;

  for ( int c = 0; c < _nchannels; c++)
    {
      os << setw(4) << c << " | ";

      os << hex;
      for ( int s = 0; s < _nsamples; s++)
	{
	  os << setw(6) << iValue(s,c);
	}
      os << dec << endl;
    }

}
