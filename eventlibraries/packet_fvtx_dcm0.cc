#include <packet_fvtx_dcm0.h>

Packet_fvtx_dcm0::Packet_fvtx_dcm0()
{

  calculated_parity=0;
  parity_comparison=0; 
  number_of_hits = 0;
  error_code = 0;
}

Packet_fvtx_dcm0::Packet_fvtx_dcm0(PACKET_ptr data)
  : Packet_w4 (data)
{
  calculated_parity=0;
  parity_comparison=0; 
  number_of_hits = 0;
  error_code = 0;

}

Packet_fvtx_dcm0::~Packet_fvtx_dcm0()
{
}

// ------------------------------------------------------

int  Packet_fvtx_dcm0::iValue(const int ich, const int what)
{
  // we check that "row" is within range
  if ( what < 0 || what >3 ) return 0;

  // now we must make sure we have decoded first so that the number_of_hits used
  // in a moment is valid

  if (decoded_data1 == NULL )
    {
      if ( (decoded_data1 = decode(&data1_length))==NULL)
	return 0;
    }

  if ( ich < 0 || ich >= number_of_hits) return 0;
  if ( error_code ) return 0;

  switch (what)
    {
    case 0:     // FEMID
      return decoded_data2[ich];
      break;
      
    case 1:     // CHIPID
      return decoded_data3[ich];
      break;

    case 2:    // CHANNNEL
      return decoded_data4[ich];
      break;

    case 3:    // ADC
      return decoded_data1[ich];
      break;

    default:
      break;

    }

  return 0;
}

int  Packet_fvtx_dcm0::iValue(const int ich, const char *what)
{
  // now let's derefence the proxy array. If we didn't decode
  // the data until now, we do it now


  if ( strcmp(what,"HITS")==0)
    {
      //we need to decode the main data structures now
      if (decoded_data1 == NULL )
	{
	  if ( (decoded_data1 = decode(&data1_length))==NULL)
	    return 0;
	}
      if ( error_code ) return 0;
      return number_of_hits;
    }

  else if ( strcmp(what,"FLAG")==0)
    {
      if (decoded_data5 == NULL )
	{
	  if ( (decoded_data5 = decode_misc(&data5_length))==NULL)
	    return 0;
	}
      
      return decoded_data5[1];
    }

  else if ( strcmp(what,"EVTNR")==0)
    {
      if (decoded_data5 == NULL )
	{
	  if ( (decoded_data5 = decode_misc(&data5_length))==NULL)
	    return 0;
	}
      
      return decoded_data5[0];
    }

  else if ( strcmp(what,"DETID")==0)
    {
      if (decoded_data5 == NULL )
	{
	  if ( (decoded_data5 = decode_misc(&data5_length))==NULL)
	    return 0;
	}
      
      return decoded_data5[2];
    }

  else if ( strcmp(what,"ADC")==0)
    {
      if (decoded_data1 == NULL )
	{
	  if ( (decoded_data1 = decode(&data1_length))==NULL)
	    return 0;
	}
      if ( ich < 0 || ich >= number_of_hits) return 0;      
      return decoded_data1[ich];
    }


  else if ( strcmp(what,"FEMID")==0)
    {
      if (decoded_data1 == NULL )
	{
	  if ( (decoded_data1 = decode(&data1_length))==NULL)
	    return 0;
	}
      
      if ( ich < 0 || ich >= number_of_hits) return 0;      
      return decoded_data2[ich];
    }

  else if ( strcmp(what,"CHIPID")==0)
    {
      if (decoded_data1 == NULL )
	{
	  if ( (decoded_data1 = decode(&data1_length))==NULL)
	    return 0;
	}
      
      if ( ich < 0 || ich >= number_of_hits) return 0;      
      return decoded_data3[ich];
    }

  else if ( strcmp(what,"CHANNEL")==0)
    {
      if (decoded_data1 == NULL )
	{
	  if ( (decoded_data1 = decode(&data1_length))==NULL)
	    return 0;
	}
      
      if ( ich < 0 || ich >= number_of_hits) return 0;      
      return decoded_data4[ich];
    }


  else if ( strcmp(what,"MODADDR")==0)
    {
      if (decoded_data5 == NULL )
	{
	  if ( (decoded_data5 = decode_misc(&data5_length))==NULL)
	    return 0;
	}
      
      return decoded_data5[3];
    }


  else if ( strcmp(what,"BCLCK")==0)
    {
      if (decoded_data5 == NULL )
	{
	  if ( (decoded_data5 = decode_misc(&data5_length))==NULL)
	    return 0;
	}
      
      return decoded_data5[4];
    }

  else if ( strcmp(what,"PARITY")==0)
    {
      if (decoded_data5 == NULL )
	{
	  if ( (decoded_data5 = decode_misc(&data5_length))==NULL)
	    return 0;
	}
      
      return decoded_data5[5];
    }

  else if ( strcmp(what,"FEMERROR")==0)
    {
      if (decoded_data5 == NULL )
	{
	  if ( (decoded_data5 = decode_misc(&data5_length))==NULL)
	    return 0;
	}
      
      return decoded_data5[6];
    }

  else if ( strcmp(what,"CHECKPARITY")==0)
    {
      if (! parity_comparison  )
 	{
 	  calculate_parity();
 	}
      return parity_comparison;
    }
  
  else if ( strcmp(what,"ERRORCODE")==0)
    {
      return error_code;
    }
  
  
  return 0;
}



// ------------------------------------------------------

int *Packet_fvtx_dcm0::decode (int *nwout)
{
  int *p,*k;

    
  int dlength = getDataLength();

  p = (int *) findPacketDataStart(packet);
  if (p == 0) 
    {
      *nwout = 0;
      return 0;
    }
  k = &p[5]; // skip the header info
  int payloadlength = dlength -5; 

  // this routine is designed to return the "main" array of values which gets 
  // hooked up to decoded_data1 internally. 
  // in addition, we fill decoded_data{2,3,4} here as well. In this way, we don't
  // need to bother with deleting the allocated memory in the end. 

  // we use 
  // decoded_data1 -> adc values
  // decoded_data2 -> FEM ID
  // decoded_data3 -> CHIP ID
  // decoded_data4 -> CHANNEL NUMBER

  p =             new int [ 4*512];  // 4 FEM ids (wedges), 512 max hits per wedge
  decoded_data2 = new int [ 4*512];  // 4 FEM ids (wedges), 512 max hits per wedge
  decoded_data3 = new int [ 4*512];  // 4 FEM ids (wedges), 512 max hits per wedge
  decoded_data4 = new int [ 4*512];  // 4 FEM ids (wedges), 512 max hits per wedge

  data2_length = 4*512;
  data3_length = 4*512;
  data4_length = 4*512;
  
  memset (p, 0, 4*512*sizeof(int));
  memset (decoded_data2, 0, 4*512*sizeof(int));
  memset (decoded_data3, 0, 4*512*sizeof(int));
  memset (decoded_data4, 0, 4*512*sizeof(int));

  // let's confirm that k[payloadlength-1] has the MSB set (this holds the CRS)
  if ( (k[payloadlength-1] & 0x80000000) == 0)
    {
      std::cout << __FILE__ << "  " << __LINE__ << " Error in trailer word " << std::hex << k[payloadlength-1] << std::dec << "  ";
      this->identify();
      // for now we let this pass
    }

  // and let's confirm that k[0] is an actual FEMID field ( must have all 12 LSBs 0) 
  if ( (k[0] & 0xFFF) != 0)
    {
      std::cout << __FILE__ << "  " << __LINE__ << " Error: 1st word is not a FEM ID " << std::hex << k[0] << std::dec << "  ";
      this->identify();
      // for now we let this pass
    }

  int i;
  int last_femid = -1;
  int number_of_datawords = 0;
  int adc, channelnumber, chipid;
  int array_pos = 0;

  for ( i = 0; i < payloadlength -2; i++)
    {

      if ( (k[i] & 0xFFF) == 0) // found a FEM ID
	{
	  // the simulated data have thee femid encoded differently
	  if ( 	getHitFormat() == IDFVTX_SIM)
	    {
	      last_femid = ((k[i] >> 12) & 0xf) -1;
	    }

	  else
	    {
	      last_femid = -1;  // we reset to "invalid"
	      int f = k[i] >> 12;
	      if ( f & 1 ) last_femid = 0;
	      if ( f & 2 ) last_femid = 1;
	      if ( f & 4 ) last_femid = 2;
	      if ( f & 8 ) last_femid = 3;
	    }
	  number_of_datawords = 0;     // we reset the dataword count, just a sanity check if we get more than 512
	}

      else  // now this should be a data word
	{
	  
	  // raise hell if either
	  //  - we still don't have a valid FEM ID
	  //  - the number_of_datawords has reached 512 
	  //  - the array_pos has reached 2048 

	  
	  if ( last_femid < 0)
	    {
	      error_code = 1;
	      //	      std::cout << __FILE__ << "  " << __LINE__ << " Error: got data words without FEM ID at position " << i+5 << "   ";
	      //this->identify();
	    }
 	  else if ( number_of_datawords >= 512) 
	    {
	      error_code = 2;
	      //      std::cout << __FILE__ << "  " << __LINE__ << " Error: we aleady have reached the max hit count for FEMID " << last_femid << "    ";
	      //this->identify();
	    }
 	  else if ( array_pos >= 4*512) 
	    {
	      error_code = 3;
	      //	      std::cout << __FILE__ << "  " << __LINE__ << " Error: we aleady have reached the max total hit count    " ;
	      //this->identify();
	    }
	  else
	    {

	      adc           =         k[i] & 0x07;
	      chipid        = (k[i]>>3) & 0x3f;
	      channelnumber = (k[i]>>9) & 0x7f;

	      p[array_pos]             = adc;
	      decoded_data2[array_pos] = last_femid;
	      decoded_data3[array_pos] = chipid;
	      decoded_data4[array_pos] = channelnumber;
	      array_pos++;
	      number_of_datawords++;
	    }
	}
    }
  number_of_hits = array_pos;  // we remember how many hits we actually got
  *nwout = 4*512;
  return p;
}

// ------------------------------------------------------

int Packet_fvtx_dcm0::calculate_parity ()
{
  int *k;

  k = (int *) findPacketDataStart(packet);
  if (k == 0) 
    {
      return 0;
    }

  int dlength = getDataLength();

  int i;
  int p = 0;

  // the simulated data calculate the hitformat
  // from word 0; real data from word 5
  if ( 	getHitFormat() == IDFVTX_SIM)
    {
      i = 0;
    }
  else
    {
      i = 5;
    }
  for (; i< dlength-1; i++)
    {
      p = p ^ (k[i] & 0xffff);
    }

  calculated_parity = p;

  if( p == iValue(0, "PARITY") )
    {
      parity_comparison = 1;
    }
  else
    {
      parity_comparison = -1;
    }

  return 0;
}



// ------------------------------------------------------

int *Packet_fvtx_dcm0::decode_misc (int *nwout)
{
  int *p,*k;


  k = (int *) findPacketDataStart(packet);
  if (k == 0) 
    {
      *nwout = 0;
      return 0;
    }

  int dlength = getDataLength();
  
  p = new int[7];

  p[0] = k[0] & 0xffff;  // event number
  p[1] = k[1] & 0xffff;  // Flag word
  p[2] = k[2] & 0xffff;  // DET ID
  p[3] = k[3] & 0xffff;  // module address
  p[4] = k[4] & 0xffff;  // beam clock counter
  p[5] = k[dlength-1] & 0xffff;  // PARITY
  p[6] = k[dlength-2] & 0xffff;  // FEM_ERROR_WORD


  *nwout = 7;
  return p;

}

// ------------------------------------------------------

void Packet_fvtx_dcm0::dump ( OSTREAM &os) 
{

  this->identify(os);
  
  os << " Number of hits                   " << iValue(0,"HITS")  << std::endl;
  os << " Event number:                    " << iValue(0,"EVTNR") << std::endl; 
  os << " Det id                           " << iValue(0,"DETID") << std::endl;
  if ( iValue(0,"ERRORCODE"))
    {
      os << " Error Code  *********            " << iValue(0,"ERRORCODE");
      switch ( iValue(0,"ERRORCODE") )
	{
	case 1:
	  os << "  (no FEM ID found)";
	  break;

	case 2:
	  os << "  (Max hit count per FEM ID reached)";
	  break;

	case 3:
	  os << "  (Max hit count per packet reached)";
	  break;

	default :
	  break;
	}
      os<< std::endl;
    }
  else 
    {
      os << " Error Code                       " << iValue(0,"ERRORCODE") << std::endl;
    }
  os << " module address                   " << iValue(0,"MODADDR") << std::endl;
  os << " Flag word                      0x" << std::hex << iValue(0,"FLAG") <<std::dec << std::endl;
  os << " beam clock counter               " << iValue(0,"BCLCK") << std::endl;
  os << " FEM Error word                   " << iValue(0,"FEMERROR") << std::endl;
  os << " Parity                           " << std::hex << iValue(0,"PARITY") << std::dec <<std::endl;
  os << " Parity Ok                        ";

  if ( iValue(0,"CHECKPARITY") ==1 )
    {
      os << "Yes" <<std::endl;
    }
  else
    {
      os << "No    (0x" << std::hex << calculated_parity << ")" << std::dec << std::endl;
    }

  int hit;

  if  ( iValue(0,"HITS") )  os << " hit # |  FemId  Chip   Chan    ADC" << std::endl;

  for ( hit = 0; hit  < iValue(0,"HITS"); hit++)
    {
      os << std::setw(6) << hit << " |" 
	 << std::setw(5) << iValue(hit, 0)  
	 << std::setw(7) << iValue(hit, 1)  
	 << std::setw(7) << iValue(hit, 2)  
	 << std::setw(7) << iValue(hit, 3)  
	 << std::endl;



    }
  std::cout << std::endl;
  
  //   dumpErrorBlock(os);
  //  dumpDebugBlock(os);

}








