#include "packet_idll1v1.h"

#include <string.h>

using namespace std;

Packet_idll1v1::Packet_idll1v1(PACKET_ptr data)
  :Packet_w4 (data)
{

 _slot_nr = 0;
 _card_nr = 0;
 _nsamples = 0;
 _evt_nr = 0;
 _clock = 0;

 _nchannels = 13*4 + 8;
 _even_checksum = 0;
 _odd_checksum = 0;
 
 _calculated_even_checksum = 0;
 _calculated_odd_checksum = 0;
 
 _even_checksum_ok = -1;  // -1 convention: "cannot be evaluated", typically for 0-supp. data 
 _odd_checksum_ok = -1;   // else ok =1, not ok 0.
 
 _is_decoded = 0;


}


Packet_idll1v1::~Packet_idll1v1()
{
  //  if (array) delete [][] array;
}



const int offset=2;
// const int samples=12;
// const int channels=64;

  
int Packet_idll1v1::decode ()
{


  if (_is_decoded ) return 0;
  _is_decoded = 1;


  int *k;


  // check later  int dlength = ( getLength()-4) - getPadding();

  int *SubeventData = (int *) findPacketDataStart(packet); 

  switch ( getHitFormat() )
    {
    case IDLL1_20S:
      _nsamples   = 20;
      _hitbits    = 6;
      break;
    case IDLL1v2_20S:
      _nsamples   = 20;
      _hitbits    = 7;
    default:
      _nsamples   = 20;
      _hitbits    = 7;
      break;
    }

  int upperbits[10] = { 15, 
			15,
			_hitbits,
			_hitbits,
			_hitbits, 
			_hitbits,
			9,
			10,
			16,
			25};

  _slot_nr           =  (SubeventData[0]  & 0xffff) / 2;
  _card_nr           =  (SubeventData[0]  & 0xffff) % 2;

  _clock             =  (SubeventData[1] & 0xffff) ;
  _evt_nr             = ((SubeventData[1] >> 16)  & 0xffff)  ;


  int index = getDataLength() - 1;
  _even_checksum = (SubeventData[index] >> 16) & 0xffff;
  _odd_checksum = SubeventData[index] & 0xffff;
  
  k = &SubeventData[offset];

  int is;

  int ia, ib, iw, ijk, iturn;
  int count;  
  for (is=0; is< _nsamples; is++ ) {

    for (ia = 0; ia < 20; ia++)
      {

	array[2*ia][is]        = (k[(ia*_nsamples) + is] & 0xffff) ;
	array[2*ia + 1][is]    =  ((k[(ia*_nsamples) + is] >> 16 ) & 0xffff) ;

      }

    for (ia=0; ia<4; ia++) {
      ijk = ia*8;
      itrig_charge[ia][0][is] = array[ijk][is] & 0x1ff;          //8-0
      itrig_charge[ia][1][is] = ((array[ijk][is] & 0xfe00) >>9) + ((array[ijk+1][is] & 0x3)<<7) ;  //17-9
      itrig_charge[ia][2][is] = ((array[ijk+1][is] & 0x7fc) >>2) ;                                  //26-18
      itrig_charge[ia][3][is] = ((array[ijk+1][is] & 0xf800) >>11) + ((array[ijk+2][is] & 0xf)<<5) ;  //35-27
      itrig_charge[ia][4][is] = ((array[ijk+2][is] & 0x1ff0) >>4) ;                                 //44-36
      itrig_charge[ia][5][is] = ((array[ijk+2][is] & 0xe000) >>13) + ((array[ijk+3][is] & 0x3f)<<3) ;  //53-45
      itrig_charge[ia][6][is] = ((array[ijk+3][is] & 0x7fc0) >>6) ;                                   //62-54
      itrig_charge[ia][7][is] = ((array[ijk+3][is] & 0x8000) >>15) + ((array[ijk+4][is] & 0xff)<<1) ;  //71-63
      itrig_nhit[ia][is] = (array[ijk+4][is] & 0xff00) >> 8;
      itrig_time[ia][0][is] = array[ijk+5][is] & 0xfff;          //91-80
      itrig_time[ia][1][is] = ((array[ijk+5][is] & 0xf000) >>12) + ((array[ijk+6][is] & 0xff)<<4) ;  //103-92
      itrig_time[ia][2][is] = ((array[ijk+6][is] & 0xff00) >>8) + ((array[ijk+7][is] & 0xf)<<8) ;  //115- 104
      itrig_time[ia][3][is] = (array[ijk+7][is] & 0xfff0)>>4;                                      //127-116
    }

    ijk = 32;
    ib = 0;
    iturn = 0;
    count = 0;
    for (ia = 0; ia < 8; ia++)
      {
	triggerwords[ia][is] = 0;
	for (iw = 0 ; iw < upperbits[ia] ; iw++)
	  {
	    if (iw+ib == 16)
	      {
		ib = iw;
		ijk++;
		iturn = 1 - iturn;
		
	      }

	    if (!iturn) triggerwords[ia][is] += ((array[ijk][is] & (0x1 << (ib+iw))) >> ib);
	    else triggerwords[ia][is] += ((array[ijk][is] & (0x1 << (iw - ib))) << ib);

	    //	    if (is == 8 || is == 9 ) cout << dec<< count << "= ia: "<<ia<<" , sample "<<is<<", iw/ib/turn: "<<iw<<"/"<<ib<<"/"<<iturn<<" --> "<< hex<<triggerwords[ia][is]<<endl;

	    count++;
	  }
	ib = (iturn? iw-ib: iw+ib);
	iturn = 0;
      }
    
    // triggerwords[0][is] = array[32][is] & 0x7fff;
    // triggerwords[1][is] = ((array[32][is] & 0x8000)>>15)+((array[33][is] & 0x3fff) <<1);
    // triggerwords[2][is] = ((array[33][is] & 0xc000)>>14)+((array[34][is] & 0xf) <<2);
    // triggerwords[3][is] = ((array[34][is] & 0x03f0)>>4);
    // triggerwords[4][is] = ((array[34][is] & 0xfc00)>>10);
    // triggerwords[5][is] = (array[35][is] & 0x3f);
    // triggerwords[6][is] = ((array[35][is] & 0x7fc0)>>6);
    // triggerwords[7][is] = ((array[35][is] & 0x8000)>>15)+((array[36][is] & 0x1ff) <<1);

  }

  for ( index = 5; index < getDataLength()-3; index+=2)  // -3 to spare the CS fields out
    {
      _calculated_even_checksum ^= (SubeventData[index]) & 0xffff;
      _calculated_odd_checksum ^= (SubeventData[index+1] >> 16 ) & 0xffff;
    }
  
  return 0;
}


int Packet_idll1v1::iValue(const int sample, const int ch)
{
  decode();

  if ( sample >= _nsamples || sample < 0 
       || ch >= _nchannels || ch < 0 ) return 0;

  if (ch < 52)
    {
      if (ch%13 < 8)
	{
	  return itrig_charge[ch/13][ch%13][sample];
	}
      else if (ch%13 == 8)
	{
	  return itrig_nhit[ch/13][sample];
	}
      else
	{
	  return itrig_time[ch/13][(ch - 9)%13][sample];
	}
    }
  return triggerwords[ch - 52][sample];
  
}

int Packet_idll1v1::iValue(const int n, const char *what)
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
    return _nchannels;
  }

  if ( strcmp(what,"SLOTNR") == 0 )
  {
    return _slot_nr;
  }

  if ( strcmp(what,"CARDNR") == 0 )
  {
    return _card_nr;
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

void  Packet_idll1v1::dump ( OSTREAM& os )
{
  identify(os);

  os << "Evt Nr:      " << iValue(0,"EVTNR") << std::endl;
  os << "Clock:       " << iValue(0,"CLOCK") << std::endl;
  os << "Channels:    " << iValue(0,"CHANNELS") << std::endl;
  os << "Samples:     " << iValue(0,"SAMPLES") << std::endl;

  os << "Slot:    " << iValue(0,"SLOTNR") << std::endl;
  os << "Card:    " << iValue(0,"CARDNR") << std::endl;

  os << "Even chksum: 0x" << hex << iValue(0,"EVENCHECKSUM")  << "   calculated:  0x" <<  iValue(0,"CALCEVENCHECKSUM");
  if ( iValue(0,"EVENCHECKSUMOK") == 1) os << " ok" << endl;
  else if ( iValue(0,"EVENCHECKSUMOK") == 0) os << " **wrong" << endl;

  os << "Odd chksum:  0x" << hex << iValue(0,"ODDCHECKSUM")  << "   calculated:  0x" <<  iValue(0,"CALCODDCHECKSUM");
  if ( iValue(0,"ODDCHECKSUMOK") == 1) os << " ok" << endl;
  else if ( iValue(0,"ODDCHECKSUMOK") == 0) os << " **wrong" << endl;
  os << dec << endl;

  for ( int c = 0; c < 4; c++)
    {
      for (int j = 0; j < 13; j++)
	{
	  os << setw(4) << c << " " << j << " | ";

	  os << hex;
	  for ( int s = 0; s < _nsamples; s++)
	    {
	      os << setw(6) << iValue(s,c*13 + j);
	    }
	  os << dec << endl;
	}
    }

      for (int j = 0; j < 8; j++)
	{
	  os << setw(4) << 25 << " " << j << " | ";

	  os << hex;
	  for ( int s = 0; s < _nsamples; s++)
	    {
	      os << setw(6) << iValue(s, 52 + j);
	    }
	  os << dec << endl;
	}

}
