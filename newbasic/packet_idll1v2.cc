#include "packet_idll1v2.h"

#include <string.h>

using namespace std;

Packet_idll1v2::Packet_idll1v2(PACKET_ptr data)
  :Packet_w4 (data)
{

 _slot_nr = 0;
 _card_nr = 0;
 _nsamples = 0;
 _evt_nr = 0;
 _clock = 0;
 _nfibers = 24;
 _nsums = 16;
 _nchannels = 256;
 
 _is_decoded = 0;

}


Packet_idll1v2::~Packet_idll1v2()
{
  //  if (array) delete [][] array;
}



const int offset=0;
// const int samples=12;
// const int channels=64;

  
int Packet_idll1v2::decode ()
{


  if (_is_decoded ) return 0;
  _is_decoded = 1;


  int *k;

  // check later  int dlength = ( getLength()-4) - getPadding();
  int *SubeventData = (int *) findPacketDataStart(packet); 

  switch ( getHitFormat() )
    {
    case IDLL1_MBD:
      _trigger_type = TRIGGERTYPE::MBD;
      break;
    case IDLL1_EMCAL_MON0:
      _trigger_type = TRIGGERTYPE::EMCAL;
      _monitor = 0;
      break;
    case IDLL1_EMCAL_MON1:
      _trigger_type = TRIGGERTYPE::EMCAL;
      _monitor = 1;
    default:
      _trigger_type = TRIGGERTYPE::NONE;
      break;
    }


  k = &SubeventData[offset];

  _slot_nr = (k[0] & 0xffff) / 2;
  _card_nr = (k[0] & 0xffff) % 2;
 
  _evt_nr = ( k[3] & 0xffff );
  _clock = ( k[2] & 0xffff);

  int dlength = getDataLength() - 2; // 9 header words and 2 checksums + 1 more word at the end

  _nsamples =  (dlength - 4)/256;
 
  for (int is=0; is< _nsamples; is++ ) {

    for (int ch=0; ch< 256/2; ch++) {
      array[(ch*2)][is] = k[2*(ch*_nsamples)+2*is+4] & 0xffff;
      array[((ch*2+1))][is] = k[2*(ch*_nsamples)+2*is + 1 + 4] & 0xffff;
    }
    
  }
  
  for (int is=0; is<_nsamples; is++) {
    for (int ch = 0; ch < 24; ch++)
      {
	for (int ia = 0; ia < 8; ia++)
	  {
	    itrig_sums[ch][ia*2][is] = ((array[8*ch + ia][is] & 0xff));
	    itrig_sums[ch][ia*2+1][is] = ((array[8*ch + ia][is] >> 8 ) & 0xff);
	  }		
      }
  }
  
  return 0;
}


int Packet_idll1v2::iValue(const int sample, const int isum)
{
  decode();

  if ( sample >= _nsamples || sample < 0 
       || isum >= _nfibers*_nsums || isum < 0) return 0;

  return itrig_sums[isum/_nsums][isum%_nsums][sample];
  
}

int Packet_idll1v2::iValue(const int n, const char *what)
{

  decode();

  if ( strcmp(what,"CLOCK") == 0 )
  {
    return _clock;
  }

  if ( strcmp(what,"EVTNR") == 0 )
  {
    return _evt_nr;
  }

  if ( strcmp(what,"SAMPLES") == 0 )
  {
    return _nsamples;
  }

  if ( strcmp(what,"CHANNELS") == 0 )
  {
    return _nfibers*_nsums;
  }

  if ( strcmp(what,"SLOTNR") == 0 )
  {
    return _slot_nr;
  }

  if ( strcmp(what,"CARDNR") == 0 )
  {
    return _card_nr;
  }


  return 0;

}

void  Packet_idll1v2::dump ( OSTREAM& os )
{
  identify(os);

  std::cout <<" "<<std::endl;
  
  std::cout << std::dec << std::setprecision(2) << "Trigger Module = " << iValue(0, "SLOTNR") * 2 + iValue(0, "CARDNR") << std::endl;
  std::cout << std::dec << std::setprecision(4) << "Trigger Number = " << iValue(0, "EVTNR") <<std::endl; 
  std::cout << std::dec << std::setprecision(4) << "Beam Crossing Number = " << iValue(0, "CLOCK") <<std::endl; 
  
  std::cout <<"-------------------------------------------------------------- "<<std::endl;
  for (int ch = 0; ch < 24; ch++)
    {      
      std::cout << std::dec<<"Fiber: "<<ch<<std::endl;
      for (int ic=0; ic<16; ic++) {
	std::cout<<std::dec<<" Sum " << ic<<" |";
	for (int is=0; is<_nsamples; is++) {
	  std::cout << std::hex<< " "<<iValue(is, ch*16 + ic);
	}
	
	std::cout <<" |"<<endl;
	std::cout <<"-------------------------------------------------------------- "<<std::endl;
	
      }
    }

}
