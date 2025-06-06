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
 _ntrigger_words = 0; 
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
      _nfibers = 4;
      _nsums = 13;
      _ntrigger_words = 8;
      _monitor = 0;
      break;
      
      // EMCAL decoder :
      // 0 is lower cross-stich 4x4 overlapping sum result
      // 1 is upper cross-stich 4x4 overlapping sum result
      // 2 is 2d array map of 2x2 sums (in 1024 bit sections os 4224 bit array) must know what section
      // 3 is the peak sorting result
      // 4 is the 8x8 non-overlapping sum result
    case IDLL1_EMCAL_MON0:
      _trigger_type = TRIGGERTYPE::EMCAL;
      _detector_type = DETECTORTYPE::dEMCAL;
      _monitor = 0;
      _ntrigger_words = 32;
      break;

    case IDLL1_EMCAL_MON1:
      _trigger_type = TRIGGERTYPE::EMCAL;
      _detector_type = DETECTORTYPE::dEMCAL;
      _monitor = 1;
      _ntrigger_words = 32;
      break;

    case IDLL1_EMCAL_MON2:
      _trigger_type = TRIGGERTYPE::EMCAL;
      _detector_type = DETECTORTYPE::dEMCAL;
      _monitor = 2;
      _ntrigger_words = 24;
      break;

    case IDLL1_EMCAL_MON3:
      _trigger_type = TRIGGERTYPE::EMCAL;
      _detector_type = DETECTORTYPE::dEMCAL;
      _monitor = 3;
      _ntrigger_words = 32;
      break;


      // JET decoder :
      // EMCAL v HCAL fiber input readout.
      // 0 - 4 are the sections in eta of the 4x4 overlapping 8x8 non-overllaping sum results

    case IDLL1_JET_EMCAL_MON0:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 32*9;
      _monitor = 0;
      break;

    case IDLL1_JET_EMCAL_MON1:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 32*9;
      _monitor = 1;
      break;

    case IDLL1_JET_EMCAL_MON2:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 32*9;
      _monitor = 2;
      break;

    case IDLL1_JET_EMCAL_MON3:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 32*9;
      _monitor = 3;
      break;

    case IDLL1_JET_EMCAL_MON4:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 32*9;
      _monitor = 4;
      break;
    case IDLL1_JET_HCAL_MON0:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 32*9;
      _monitor = 0;
      break;

    case IDLL1_JET_HCAL_MON1:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 32*9;
      _monitor = 1;
      break;

    case IDLL1_JET_HCAL_MON2:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 32*9;
      _monitor = 2;
      break;

    case IDLL1_JET_HCAL_MON3:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 32*9;
      _monitor = 3;
      break;

    case IDLL1_JET_HCAL_MON4:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 32*9;
      _monitor = 4;
      break;

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

  _nsamples = (dlength - 4)/256;
  if (_trigger_type == TRIGGERTYPE::MBD)
    {
      _nsamples = 5;
    }
  int ijk, count, ib, iturn, iw;

  for (int is=0; is< _nsamples; is++ ) {
    
    for (int ch=0; ch< 256/2; ch++) {
      if (ch == 95 && is >= _nsamples - 2){
	array[(ch*2)][is] = k[2*(127*_nsamples)+2*is+4] & 0xffff;
	array[((ch*2+1))][is] = k[2*(127*_nsamples)+2*is + 1 + 4] & 0xffff;
      }
      else if(ch == 127 && is >= _nsamples - 2){
	array[(ch*2)][is] = k[2*(95*_nsamples)+2*is+4] & 0xffff;
	array[((ch*2+1))][is] = k[2*(95*_nsamples)+2*is + 1 + 4] & 0xffff;
      }
      else{
	array[(ch*2)][is] = k[2*(ch*_nsamples)+2*is+4] & 0xffff;
	array[((ch*2+1))][is] = k[2*(ch*_nsamples)+2*is + 1 + 4] & 0xffff;
      }
    }
  }

  
  
  if (_trigger_type == TRIGGERTYPE::EMCAL)
    { 
      for (int is=0; is<_nsamples; is++) {
	for (int ch = 0; ch < 24; ch++)
	  {
	    for (int ia = 0; ia < 8; ia++)
	      {
		itrig_sums[ch][ia*2][is] = ((array[8*ch + ia][is] & 0xff));
		itrig_sums[ch][ia*2+1][is] = ((array[8*ch + ia][is] >> 8 ) & 0xff);
	      }		
	  }
	if (_monitor == 0)
	  {
	    for (int ix = 0 ; ix < 64; ix ++)                                                                                                                                                           
	      {                                                                                                                                                                                         
		int ic1 = 192 + ix;                                                                                                                                                                     
		itrig_emcal_2x2sum[ix][is] = array[ic1][is] & 0x3ff;                                                                                                                                   
	      }     
	  }
	else if (_monitor == 1)
	  {
	    for (int ix = 0 ; ix < 64; ix ++)                                                                                                                                                           
	      {                                                                                                                                                                                         
		int ic1 = 192 + ix;    
		itrig_emcal_2x2map[ix*2][is]   = array[ic1][is] & 0xff;
		itrig_emcal_2x2map[ix*2+1][is] = (array[ic1][is] >> 8)& 0xff;
	      }     
	  }
	else if (_monitor == 3)
	  {
	    for (int ix=0; ix<2; ix++) {                                                                                                                                                                
	      for (int iy=0; iy<12; iy++) {                                                                                                                                                             
		int ic1 = 192 + iy;                                                                                                                                                                     
		if((ix%2) == 0) 
		  {
		    itrig_emcal_8x8map[ix][iy][is] = array[ic1][is] & 0xff;                            
		  }                                                                                     
		else 
		  {
		    itrig_emcal_8x8map[ix][iy][is] = (array[ic1][is] >> 8) & 0xff;                                                                                                                    
		  }   
	      }                 
	    }
	  }
      }
    }
  else if (_trigger_type == TRIGGERTYPE::JET)
    { 
      for (int iss = 0; iss < _nsamples; iss++)
	{
	  for (int is=0; is<192; is++) {
	    int ieta = is/16; 
	    int iphi = is%16; 
	    jet_map[(iphi*2)][ieta][iss] = array[is][iss] & 0xff;
	    jet_map[(iphi*2)+1][ieta][iss] = (array[is][iss]>>8) & 0xff;
	  }
	  for (int i = 0; i < 32; i++)
	    {
	      for (int j = 0 ; j < 9; j++)
		{
		  jet_sum_result[i][j][iss] = 0;
		}
	    }

	  int ic2 = 192;
	  for (int is=0; is<_ntrigger_words; is++) {

	    int ieta = is/32; 
	    int iphi = is%32; 
	    if ((is/64) == _monitor)
	      {
		jet_sum_result[iphi][ieta][iss] = array[ic2++][iss] & 0xfff;
	      }
	    else
	      {
		jet_sum_result[iphi][ieta][iss] = 0;
	      } 
	  }	  
	} 
    } 
  else if (_trigger_type == TRIGGERTYPE::MBD)
    {
      for (int is=0; is< _nsamples; is++ ) 
	{   

	  for (int ia=0; ia<4; ia++) {
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
	  for (int ia = 0; ia < 8; ia++)
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

    }  
  return 0;
}


int Packet_idll1v2::iValue(const int sample, const int isum)
{
  decode();

  if (_trigger_type == TRIGGERTYPE::EMCAL)
    {
      if ( sample >= _nsamples || sample < 0 
	   || isum >= _nfibers*_nsums + _ntrigger_words || isum < 0) return 0;
      
      if (isum < _nfibers*_nsums)
	{
	  return itrig_sums[isum/_nsums][isum%_nsums][sample];
	}
            
      int iisum = isum - _nfibers*_nsums;
      
      switch (_monitor)
	{
	case 0:
	  return itrig_emcal_2x2sum[iisum][sample];
	  break;
	case 1:
	  return itrig_emcal_2x2map[iisum][sample];
	  break;
	case 3:
	  return itrig_emcal_8x8map[iisum/12][iisum%12][sample];
	  break;
	default:
	  return 0;
	  break;
	}
    }
  else if (_trigger_type == TRIGGERTYPE::JET)
    {
      if ( sample >= _nsamples || sample < 0 
	   || isum >= _nfibers*_nsums + _ntrigger_words || isum < 0) return 0;
      if (isum < _nfibers*_nsums)
	{
	  return jet_map[isum%32][isum/32][sample];
	}
      int iisum = isum - _nfibers*_nsums;
      return jet_sum_result[iisum%32][iisum/32][sample];
    }
  else if (_trigger_type == TRIGGERTYPE::MBD)
    {
      if ( sample >= _nsamples || sample < 0 
	   || isum >= _nfibers*_nsums + _ntrigger_words || isum < 0) return 0;

      if (isum < _nfibers*_nsums)
	{

	  if (isum%13 < 8)
	    {
	      return itrig_charge[isum/13][isum%13][sample];
	    }
	  else if (isum%13 == 8)
	    {
	      return itrig_nhit[isum/13][sample];
	    }
	  else
	    {
	      return itrig_time[isum/13][(isum - 9)%13][sample];
	    }
	}

      return triggerwords[isum - 52][sample];
    }

  return 0;
}

int Packet_idll1v2::iValue(const int n, const char *what)
{

  decode();

  if ( strcmp(what,"MONITOR") == 0 )
  {
    return _monitor;
  }

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
    return _nfibers*_nsums + _ntrigger_words;
  }

  if ( strcmp(what,"TRIGGERWORDS") == 0 )
  {
    return _ntrigger_words;
  }

  if ( strcmp(what,"FEMWORDS") == 0 )
  {
    return _nfibers*_nsums;
  }
  if ( strcmp(what,"FIBERS") == 0 )
  {
    return _nfibers;
  }
  if ( strcmp(what,"SUMS") == 0 )
  {
    return _nsums;
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


  os <<" "<<std::endl;
  
  os << std::dec << std::setprecision(2) << "Trigger Module = " << iValue(0, "SLOTNR") * 2 + iValue(0, "CARDNR") << std::endl;
  os << std::dec << std::setprecision(4) << "Evt Nr = " << iValue(0, "EVTNR") <<std::endl;
  os << std::dec << std::setprecision(4) << "Clock = " << iValue(0, "CLOCK") <<std::endl;
  os << std::dec << std::setprecision(4) << "Monitor = " << iValue(0, "MONITOR") <<std::endl;

  if (_trigger_type == TRIGGERTYPE::EMCAL)
    {  
      os <<"-------------------------------------------------------------- "<<std::endl;
      for (int ch = 0; ch < iValue(0, "FIBERS"); ch++)
	{      
	  os << std::dec<<"Fiber: "<<ch<<std::endl;
	  for (int ic=0; ic<iValue(0, "SUMS"); ic++) {
	    os<<std::dec<<" Sum " << ic<<" |";
	    for (int is=0; is< iValue(0, "SAMPLES"); is++) {
	      os << std::hex<< " "<<iValue(is, ch*iValue(0, "SUMS") + ic);
	    }
	
	    os <<" |"<<endl;
	    os <<"-------------------------------------------------------------- "<<std::endl;
	
	  }
	}
      for (int ic = 0; ic < iValue(0, "TRIGGERWORDS"); ic++)
	{      
	  os << std::dec<<"SUM "<<ic<<std::endl;
	  for (int is=0; is<iValue(0,"SAMPLES"); is++) {
	    os << std::hex<< " "<<iValue(is, iValue(0,"FEMWORDS") + ic);
	  }
	  os <<" |"<<endl;
	  os <<"-------------------------------------------------------------- "<<std::endl;
	}

    }
  else if (_trigger_type == TRIGGERTYPE::JET)
    {
      os << " -------------- " << (iValue(0, "MONITOR")? "HCAL Data Map" : "EMCAL Data Map") <<  " -------------- " <<std::endl;
      for (int sample = 0; sample < iValue(0, "SAMPLES") ; sample++)
	{      
	  os << std::dec<<"BC : "<<sample<<std::endl;
	  os<<std::dec<<"phibin --> ";
	  for (int ic=0; ic<32; ic++) os << std::dec << "\t" << ic ;
	  os << " " << std::endl;
	  os<<std::dec<<"etabin\t||  \t"<<std::endl;

	  for (int ic=0; ic<12; ic++)
	    {
	      os<<std::dec<< ic << "\t||";
	      for (int is=0; is<32; is++) {
		os << std::hex<< "\t"<<iValue(sample, ic*32 + is);
	    }
	    os <<" |"<<endl;
	  }
	  os << " "<<std::endl;
	}

      for (int is = 0; is < _nsamples; is++)
	{      
	  os << std::dec<<"Sample: "<<is<<std::endl;
	  for (int ic=0; ic<9; ic++) {
	    for (int ie=0; ie<32; ie++) {
	      os << std::hex<< " "<<iValue(is, iValue(0,"FEMWORDS") + ic*32 + ie);
	    }
	
	    os <<" |"<<endl;
	    os <<"-------------------------------------------------------------- "<<std::endl;
	
	  }
	}

    }
  else if (_trigger_type == TRIGGERTYPE::MBD)
    {
      for (int ifem = 0; ifem < iValue(0, "FIBERS"); ifem++)
	{      
	  os << std::dec<<"FEM : "<<ifem<<std::endl;
	  for (int iq = 0 ; iq < 8; iq++)
	    {
	      os<<std::dec<<"Q" << iq << "\t||  \t";
	      for (int is=0; is<iValue(0, "SAMPLES"); is++)
		{
		  os << std::hex << iValue(is, ifem*iValue(0,"SUMS") + iq)<<"\t";
		}
	      os <<" |"<<endl;
	    }
	  os<<std::dec<<"NH \t||  \t";
	  for (int is=0; is<iValue(0, "SAMPLES"); is++)
	    {
	      os << std::hex << iValue(is, ifem*iValue(0,"SUMS") + 8)<<"\t";
	    }
	  os <<" |"<<endl;

	  for (int iq = 0 ; iq < iValue(0,"FIBERS"); iq++)
	    {
	      os<<std::dec<<"T" << iq << "\t||  \t";
	      for (int is=0; is<iValue(0, "SAMPLES"); is++)
		{
		  os << std::hex <<iValue(is, ifem*iValue(0,"SUMS") + 9 + iq)<<"\t";
		}
	      os <<" |"<<endl;
	    }
	  os << " "<<std::endl;
	}

	for (int iw = 0; iw < iValue(0,"TRIGGERWORDS"); iw++)
	{      
	  os<<std::dec<< "W "<<iw << "\t||  \t";
	  for (int is=0; is<iValue(0,"SAMPLES"); is++) 
	    {
	      os << std::hex<<iValue(is, iValue(0,"FEMWORDS") + iw) << "\t";
	    }
	
	  os <<" |"<<endl;
	}
  
    }

  return;
}
