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
      _ntrigger_words = 24;
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
      _ntrigger_words = 128;
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
      _ntrigger_words = 64;
      _monitor = 0;
      break;

    case IDLL1_JET_EMCAL_MON1:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 64;
      _monitor = 1;
      break;

    case IDLL1_JET_EMCAL_MON2:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 64;
      _monitor = 2;
      break;

    case IDLL1_JET_EMCAL_MON3:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 64;
      _monitor = 3;
      break;

    case IDLL1_JET_EMCAL_MON4:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dEMCAL;
      _ntrigger_words = 64;
      _monitor = 4;
      break;
    case IDLL1_JET_HCAL_MON0:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 64;
      _monitor = 0;
      break;

    case IDLL1_JET_HCAL_MON1:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 64;
      _monitor = 1;
      break;

    case IDLL1_JET_HCAL_MON2:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 64;
      _monitor = 2;
      break;

    case IDLL1_JET_HCAL_MON3:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 64;
      _monitor = 3;
      break;

    case IDLL1_JET_HCAL_MON4:
      _nfibers = 16;
      _nsums = 24;
      _trigger_type = TRIGGERTYPE::JET;
      _detector_type = DETECTORTYPE::dHCAL;
      _ntrigger_words = 64;
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

  _nsamples =  (dlength - 4)/256;

  for (int is=0; is< _nsamples; is++ ) {
    
    for (int ch=0; ch< 256/2; ch++) {
      array[(ch*2)][is] = k[2*(ch*_nsamples)+2*is+4] & 0xffff;
      array[((ch*2+1))][is] = k[2*(ch*_nsamples)+2*is + 1 + 4] & 0xffff;
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

	  for (int is=0; is<_ntrigger_words; is++) {
	    int ic1 = 192 + is;
	    int ieta = is/32; 
	    int iphi = is%32; 
	    jet_sum_result[iphi][ieta][iss] = array[ic1][iss] & 0xfff;
	  }	  
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
    return _nfibers*_nsums;
  }

  if ( strcmp(what,"TRIGGERWORDS") == 0 )
  {
    return _ntrigger_words;
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

  if (_trigger_type == TRIGGERTYPE::EMCAL)
    {  
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
      for (int is = 0; is < _nsamples; is++)
	{      
	  std::cout << std::dec<<"Sample: "<<is<<std::endl;
	  for (int ic=0; ic<2; ic++) {
	    for (int ie=0; ie<12; ie++) {
	      std::cout << std::hex<< " "<<iValue(is, 24*16 + ic * 12 + ie);
	    }
	
	    std::cout <<" |"<<endl;
	    std::cout <<"-------------------------------------------------------------- "<<std::endl;
	
	  }
	}

    }
  else if (_trigger_type == TRIGGERTYPE::JET)
    {
      std::cout << " -------------- " << (iValue(0, "MONITOR")? "HCAL Data Map" : "EMCAL Data Map") <<  " -------------- " <<std::endl;
      for (int sample = 0; sample < iValue(0, "SAMPLES") ; sample++)
	{      
	  std::cout << std::dec<<"BC : "<<sample<<std::endl;
	  std::cout<<std::dec<<"\t phibin --> "<<std::endl;
	  std::cout<<std::dec<<"etabin\t||  \t"<<std::endl;
	  for (int ic=0; ic<32; ic++) std::cout << std::dec << "\t" << ic ;
	  std::cout << " " << std::endl;
	  for (int ic=0; ic<12; ic++)
	    {
	      std::cout<<std::dec<< ic << "\t||";    
	      for (int is=0; is<32; is++) {
		std::cout << std::hex<< "\t"<<iValue(sample, ic*32 + is);
	    }
	    std::cout <<" |"<<endl;
	  }
	  std::cout << " "<<std::endl;
	}

      for (int is = 0; is < _nsamples; is++)
	{      
	  std::cout << std::dec<<"Sample: "<<is<<std::endl;
	  for (int ic=0; ic<2; ic++) {
	    for (int ie=0; ie<32; ie++) {
	      std::cout << std::hex<< " "<<iValue(is, 12*32 + ic*32 + ie);
	    }
	
	    std::cout <<" |"<<endl;
	    std::cout <<"-------------------------------------------------------------- "<<std::endl;
	
	  }
	}

    }
  return;
}
