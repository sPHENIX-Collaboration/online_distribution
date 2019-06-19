#include "oncsSub_idtpcfeev2.h"

#include <string.h>
#include <stdint.h>

#define coutfl cout << __FILE__<< "  " << __LINE__ << " "
#define cerrfl cerr << __FILE__<< "  " << __LINE__ << " "

using namespace std;

oncsSub_idtpcfeev2::oncsSub_idtpcfeev2(subevtdata_ptr data)
  :oncsSubevent_w2 (data)
{

 _is_decoded = 0;
 
 for ( int i = 0; i < MAX_FEECOUNT; i++)
   {
     _max_samples[i] = 0;
     _lowest_bx[i] = 0;
     _highest_bx[i] = 0;
   }
}


oncsSub_idtpcfeev2::~oncsSub_idtpcfeev2()
{
  // for (int fee = 0; fee < MAX_FEECOUNT; fee++)
  //   {
  //     delete fee_data[fee];
  //   }

}


#define HEADER_LENGTH 9

int oncsSub_idtpcfeev2::decode ()
{

  if (_is_decoded ) return 0;
  _is_decoded = 1;

  unsigned int payload_length = 2 * (getLength() - SEVTHEADERLENGTH - getPadding() );
  
  unsigned int index = 0;
  

  unsigned short *buffer = ( unsigned short *)  &SubeventHdr->data;


//   for (int fee = 0; fee < MAX_FEECOUNT; fee++)
//     {
//       fee_data[fee] = new std::vector<unsigned short>();
//     }
  
  while (buffer[index] != 0xfeee && buffer[index+1] != 0xba5e)
    {
      index += 2;
    }
  
  unsigned short word_count;
  unsigned short link_count;
  unsigned short link_enabled;
    
    
  int locked[16];
  for (int a = 0; a < 16; a++) locked[a] = 0;


  while ( index < payload_length)
    {

      if (buffer[index] == 0xfeee && buffer[index+1] == 0xba5e)
	{
	  
	  word_count = (buffer[index+2] >> 4) + 1;
	  link_count = buffer[index+2] & 0xf;
	  link_enabled = buffer[index+3];
	  
	    // cout << __FILE__<< "  " << __LINE__ << " feeeba5e index at " << index
	    // 	 << " word2 " << hex << buffer[index+2] << dec
	    // 	 << " wordcount " << word_count
	    // 	 << " link_count " << link_count
	    // 	 << " link_enabled 0x" << hex << link_enabled << dec
	    // 	 << endl;
	    
	  unsigned short offset_table[16] = {0};
	    
	  int offset_idx = 0;
	  for  ( int bitcnt = 0 ; bitcnt < 16; bitcnt++)
	    {
	      if ( (link_enabled & (1 << bitcnt)) != 0)
		{
		  offset_table[offset_idx] = bitcnt;
		  // coutfl << "link_enabled " << hex << link_enabled << dec
		  // 	 << " offset table " << offset_idx << " value " << bitcnt << endl;
		  offset_idx++;
		}
	    }
	  
	  int idx;
	  int link;
	    
	  //	    for (idx = index + 4; idx < index + 4 +  word_count * link_count ; idx++)
	  for (index += 4, idx=0; idx < word_count * link_count ; idx++, index++)
	    {
	      link = offset_table[idx % link_count];
	      if (locked[link])
		{
		  //coutfl << "pushing idx " << idx << " value " << hex << buffer[index] << dec << " into " << link << endl;
		  fee_data[link].push_back(buffer[index]);
		}
	      else if (buffer[index] == 0xfeed)
		{
		  locked[link] = 1;
		  //coutfl << "pushing idx " << idx << " value " << hex << buffer[index] << dec << " into " << link << " ** locking" << endl;
		  fee_data[link].push_back(buffer[index]);
		}
	    }
	    
	    // cout << __FILE__<< "  " << __LINE__ << " index " << index << " vector sizes " << fee_data[0].size()  << "  " << fee_data[1].size() << endl;

	}
      else
      {
        cout << __FILE__<< "  " << __LINE__ << " corrupted FEE data missing 0xfeee 0xba5e marker. Stop further decoding "<< endl;
        break;
      }

    }

  // here we are done unraveling the initil data structure.
  // We have now MAX_FEECOUNT * MAX_FEECHANNELS per-channel data structures in hand

  // first pass...
  
  for ( int fee = 0; fee < MAX_FEECOUNT; fee++)
    {

      unsigned int record_len  = 0;
      //unsigned int packet_type;
      unsigned int channel;
      unsigned int sampa_addr;
      unsigned int bx_count;

      unsigned int old_bxcount = 0;
      unsigned int first_bx = 0xffffff;
      unsigned int rollover_offset = 0;
      
      for ( index = 0; index < fee_data[fee].size(); index += record_len + 2 )
	{
	  if (index >=  fee_data[fee].size() - HEADER_LENGTH) continue;
	  
	  if ( fee_data[fee].at(index) != 0xfeed)
	      {
		//	cerr << __FILE__<< "  " << __LINE__ << " FEE " << fee << " not starting with feed at " << index << " size " <<  fee_data[fee].size() << endl;
		while (index < fee_data[fee].size() - HEADER_LENGTH &&  fee_data[fee].at(index) != 0xfeed   )  index++;
	      }
	  
	    if (index >=  fee_data[fee].size() - HEADER_LENGTH) continue;
		
	    record_len  = fee_data[fee].at(index+1);
	    if ( index + record_len + 2  >= fee_data[fee].size()) break;
	    
	    //packet_type = fee_data[fee].at(index+2);
	    channel     = fee_data[fee].at(index+3)  & 0x1f;
	    sampa_addr  = (fee_data[fee].at(index+3) >> 5) & 0xf;
	    bx_count    = (fee_data[fee].at(index+4)  << 4) | (fee_data[fee].at(index+5) & 0xf);

	    
	    //	    coutfl << " bx " << bx_count << " delta to previous bx " << bx_count - old_bxcount << endl;
	    if ( bx_count < old_bxcount )
	      {
		rollover_offset += 0x100000;
		//		coutfl << " bx " << bx_count << " delta to previous bx " << bx_count - old_bxcount << " rollover " << hex << rollover_offset << endl;
	      }
	    old_bxcount = bx_count;

	    if ( first_bx == 0xffffff )
	      {
		first_bx = bx_count;
	      }
	    
	    // if ( record_len != 0x88)
	    //   {
	    // 	cerrfl << "wrong record length " << record_len << " fee " << fee << " channel " << channel << endl;
	    // 	//continue;
	    //   }
	   
	    unsigned short crc_value = crc16( fee, index, record_len+1);
	    if ( index + record_len +1  >= fee_data[fee].size() )
	      {
		cerrfl << "index out of range " << index + record_len + 1 << "  " << fee_data[fee].size() << " record_len " << record_len  << endl;
	      }
	    else
	      {
		if ( crc_value != fee_data[fee].at(index + record_len +1) )
		  {
		// cout << __FILE__<< "  " << __LINE__ << " FEE " << fee
		//      << " crc mismatch, index = " << index
		//      << " sampa address " << sampa_addr << " channel " << channel
		//      << " crc = " << hex << crc_value << " value " << fee_data[fee].at(index + record_len +1) << dec <<  endl;
		    continue;
		  }
	      }
	    
	    if ( channel >= 32 || sampa_addr > 7 )
	      {
		cout << __FILE__<< "  " << __LINE__ << " FEE " << fee
		     << " illegal value, sampa address " << sampa_addr << " channel " << channel << endl;
		continue;
	      }
	    
	    unsigned int fee_channel = sampa_addr * 32 + channel;
	
		for  ( unsigned int s = HEADER_LENGTH; s < record_len +1; s++) 
		  {
		    if ( index +s >= fee_data[fee].size() )
		      {
			cerrfl << "index out of range " << index +s << "  " << fee_data[fee].size() << " record_len " << record_len  << endl;
		      }
		    else
		      {
			tpc_sample x = { bx_count + s - HEADER_LENGTH + rollover_offset - first_bx, fee_data[fee].at(index + s ) };
			fee_samples[fee][fee_channel].push_back(x);
		      }
		  }

	}
    }


    
		 
    // now, what is the max sample length per FEE?
    unsigned int  maxvectorsize;
    for ( int fee = 0; fee < MAX_FEECOUNT; fee++)
      {
	maxvectorsize = 0;
	for ( int ch = 0; ch < MAX_FEECHANNELS; ch++)
	  {
	    if ( fee_samples[fee][ch].size() > maxvectorsize ) maxvectorsize = fee_samples[fee][ch].size();
	    unsigned int s = fee_samples[fee][ch].size();
	    if ( s > 0 )
	      {
		if ( fee_samples[fee][ch].at(0).bx_time < _lowest_bx[fee] ) _lowest_bx[fee] = fee_samples[fee][ch].at(0).bx_time;
		if ( fee_samples[fee][ch].at(s-1).bx_time > _highest_bx[fee] ) _highest_bx[fee] = fee_samples[fee][ch].at(s-1).bx_time;
	      }
	  }
	_max_samples[fee] = maxvectorsize;
      }

      
    return 0;
}
    


int oncsSub_idtpcfeev2::iValue(const int fee, const int sample)
{
  decode();
  
  return 0;
}

int oncsSub_idtpcfeev2::iValue(const int fee, const int ch, const int sample)
{
  decode();
  if ( fee > MAX_FEECOUNT ||
       ch < 0 || ch >= MAX_FEECHANNELS ||
       sample < 0 || sample >= fee_samples[fee][ch].size() ) return 0;
  
  //  if ( sample >= fee_samples[fee][ch].size() ) return 0;
  return fee_samples[fee][ch].at(sample).adc;

}

int oncsSub_idtpcfeev2::iValue(const int fee, const int ch, const int sample, const char *what)
{
  decode();

  if ( fee > MAX_FEECOUNT ||
       ch < 0 || ch >= MAX_FEECHANNELS ||
       sample < 0 || sample >= fee_samples[fee][ch].size() ) return 0;


  if ( strcmp(what,"BX") == 0 )
  {
    return fee_samples[fee][ch].at(sample).bx_time;
  }

}

int oncsSub_idtpcfeev2::iValue(const int fee, const int ch, const char *what)
{

  decode();

  if ( strcmp(what,"NR_SAMPLES") == 0 )  // say if a FEE has been seen at all
  {
    if ( fee > MAX_FEECOUNT || ch < 0 || ch >= MAX_FEECHANNELS )  return 0;
    return  fee_samples[fee][ch].size();

  }

  return 0;
}
  

int oncsSub_idtpcfeev2::iValue(const int n, const char *what)
{

  decode();

  if ( strcmp(what,"MAX_SAMPLES") == 0 )
  {
    if ( n < 0 || n >= MAX_FEECOUNT ) return 0;
    
    return _max_samples[n];
  }

  else if ( strcmp(what,"IS_PRESENT") == 0 )  // say if a FEE has been seen at all
  {
    if ( n < 0 || n >= MAX_FEECOUNT ) return 0;
    
    if (_max_samples[n] > 0) return 1;
    return 0;
  }

  else if ( strcmp(what,"LOWEST_BX") == 0 )  // say if a FEE has been seen at all
  {
    if ( n < 0 || n >= MAX_FEECOUNT ) return 0;
    
    return _lowest_bx[n];
  }

  else if ( strcmp(what,"HIGHEST_BX") == 0 )  // say if a FEE has been seen at all
  {
    if ( n < 0 || n >= MAX_FEECOUNT ) return 0;
    
    return _highest_bx[n];
  }

  else if ( strcmp(what,"MAX_FEECOUNT") == 0 )
  {
    return MAX_FEECOUNT;
  }


  else if ( strcmp(what,"BROKEN") == 0 )
  {
    return _broken;
  }


  return 0;

}

void  oncsSub_idtpcfeev2::dump ( OSTREAM& os )
{
  identify(os);

  os << "Samples:       ";
  for ( int i = 0 ; i < iValue(0,"MAX_FEECOUNT"); i++)
    {
      os << setw(9) << iValue(i,"MAX_SAMPLES") << " ";
    }
  os << std::endl;

  os << "Lowest BX:      ";
  for ( int i = 0 ; i < iValue(0,"MAX_FEECOUNT"); i++)
    {
      os << setw(9) << (unsigned int) iValue(i,"LOWEST_BX") << " ";
    }
  os << std::endl;

  os << "Highest BX:     ";
  for ( int i = 0 ; i < iValue(0,"MAX_FEECOUNT"); i++)
    {
      os << setw(9) << (unsigned int) iValue(i,"HIGHEST_BX") << " ";
    }
  os << std::endl;

  
  const int dwidth = 16;  // how many colums to display
 
  for ( int fee = 0; fee < iValue(0,"MAX_FEECOUNT") ; fee++)
    {
      for ( int chblock = 0; chblock < MAX_FEECHANNELS ; chblock+=dwidth)
	{
	  if ( iValue (fee,"IS_PRESENT") )
	    {
	      cout << "       +++++++++++++++ FEE " << fee << "   channels " << chblock << " through " << chblock + dwidth << " ++++++++" << endl;
	      
	      // print ithe smaple numbers
	      os << setw(3) << "nrs" << " | " ;
	      for ( int ch = chblock; ch < chblock + dwidth; ch++)
		{
		  os << setw(10) <<  iValue(fee,ch,  "NR_SAMPLES") << "  " ;
		}
		  
	      os << endl;

		  
	      for ( int s = 0; s < iValue(fee, "MAX_SAMPLES"); s++)
		{
		  
		  os << setw(3) << s << " | " ;
		  for ( int ch = chblock; ch < chblock + dwidth; ch++)
		    {
		      os << setw(6) << hex << iValue(fee,ch,s, "BX") << " " << setw(4) << iValue(fee,ch,s) << dec << " ";
		      
		    }
		  
		  os << endl;
		}
	      os << endl;
	    }
	}
  
    }
}

unsigned short oncsSub_idtpcfeev2::reverseBits(const unsigned short x) const
{
  unsigned short n = x;  
  n = ((n >> 1) & 0x55555555) | ((n << 1) & 0xaaaaaaaa);
  n = ((n >> 2) & 0x33333333) | ((n << 2) & 0xcccccccc);
  n = ((n >> 4) & 0x0f0f0f0f) | ((n << 4) & 0xf0f0f0f0);
  n = ((n >> 8) & 0x00ff00ff) | ((n << 8) & 0xff00ff00);
  //n = (n >> 16) & 0x0000ffff | (n << 16) & 0xffff0000;
  return n;
}



unsigned short oncsSub_idtpcfeev2::crc16(const unsigned int fee, const unsigned int index, const int  l) const 
{
  int len = l;
  int i = index;
  unsigned short crc = 0xffff;
    
  while (len--)
    {
      unsigned short x = fee_data[fee].at(i++);
      //      cout << "in crc " << hex << x << dec << endl;
      crc ^= reverseBits( x);
      for (unsigned short k = 0; k < 16; k++)
	{
	  crc = crc & 1 ? (crc >> 1) ^ 0xa001 : crc >> 1;
	}
      }
  crc = reverseBits(crc);
  return crc;
}

