#include "oncsSub_idtpcfeev2.h"

#include <string.h>
#include <stdint.h>
#include <vector>
#include <map>

using namespace std;

oncsSub_idtpcfeev2::oncsSub_idtpcfeev2(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{

 _is_decoded = 0;
 
 for ( int i = 0; i < MAX_FEECOUNT; i++) _max_samples[i] = 0;
 
}


oncsSub_idtpcfeev2::~oncsSub_idtpcfeev2()
{
  for (int fee = 0; fee < MAX_FEECOUNT; fee++)
    {
      delete fee_data[fee];
    }

}


#define HEADER_LENGTH 9

int oncsSub_idtpcfeev2::decode ()
{

  if (_is_decoded ) return 0;
  _is_decoded = 1;

  int payload_length = 2 * (getLength() - SEVTHEADERLENGTH - getPadding() );
  
  unsigned int index = 0;
  

  unsigned short *buffer = ( unsigned short *)  &SubeventHdr->data;


  for (int fee = 0; fee < MAX_FEECOUNT; fee++)
    {
      fee_data[fee] = new std::vector<unsigned short>();
    }
  
    while (buffer[index] != 0xfeee && buffer[index+1] != 0xba5e)
      {
        index += 2;
      }

    unsigned short word_count;
    unsigned short link_count;
    unsigned short link_enabled;
    unsigned short offset_idx = 0;

    
    int locked[16];
    for (int a = 0; a < 16; a++) locked[a] = 0;


    //    int bitcnt = 0;

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
		    //	    	    cout << __FILE__<< "  " << __LINE__ << " link_enabled " << hex << link_enabled << dec << " offset table " << offset_idx << " value " << bitcnt << endl;
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
		    //		    cout << __FILE__<< "  " << __LINE__ << " pushing idx " << idx << " value " << hex << buffer[index] << dec << " into " << link << endl;
		    fee_data[link]->push_back(buffer[index]);
		  }
		else if (buffer[index] == 0xfeed)
		  {
		    locked[link] = 1;
		    //  cout << __FILE__<< "  " << __LINE__ << " pushing idx " << idx << " value " << hex << buffer[index] << dec << " into " << link << " ** locking" << endl;
		    fee_data[link]->push_back(buffer[index]);
		  }
	      }
	    
	    // cout << __FILE__<< "  " << __LINE__ << " index " << index << " vector sizes " << fee_data[0]->size()  << "  " << fee_data[1]->size() << endl;

	  }

      }


    for ( int fee = 0; fee < MAX_FEECOUNT; fee++)
      {

	unsigned int record_len  = 0;
	unsigned int packet_type;
	unsigned int channel;
	unsigned int sampa_addr;
	unsigned int bx_count;

	//	index = 0;
	//record_len  = fee_data[fee]->at(1);  // preset it...
	
	for ( index = 0; index < fee_data[fee]->size(); index += record_len + 2 )
	  {
	    if ( fee_data[fee]->at(index) != 0xfeed)
	      {
		//		cout << __FILE__<< "  " << __LINE__ << " FEE " << fee << " not starting with feed at " << index << endl;
		while (fee_data[fee]->at(index) != 0xfeed && index < fee_data[fee]->size() )  index++;
	      }

	    while (index >=  fee_data[fee]->size() ) continue;
		
	    record_len  = fee_data[fee]->at(index+1);

	    if ( index + record_len + 2  >= fee_data[fee]->size()) break;
	    
	    packet_type = fee_data[fee]->at(index+2);
	    channel     = fee_data[fee]->at(index+3)  & 0x1f;
	    sampa_addr  = (fee_data[fee]->at(index+3) >> 5) & 0xf;
	    bx_count    = (fee_data[fee]->at(index+4)  << 4) | (fee_data[fee]->at(index+5) & 0xf);

	   
	    if ( channel >= 32 || sampa_addr > 7 )
	      {
		cout << __FILE__<< "  " << __LINE__ << " FEE " << fee << " illegal value, sampa address " << sampa_addr << " channel " << channel << endl;
		continue;
	      }
	    
	    //	    unsigned short crc_value = crc16( &(fee_data[fee]->at(index)),record_len+1);
	    unsigned short crc_value = crc16( fee, index, record_len+1);
	    if ( crc_value != fee_data[fee]->at(index + record_len +1) )
	      {
		//		cout << __FILE__<< "  " << __LINE__ << " FEE " << fee << " crc mismatch, index = " << index << " sampa address " << sampa_addr << " channel " << channel << " crc = " << hex << crc_value << " value " << fee_data[fee]->at(index + record_len +1) << dec <<  endl;
		continue;
	      }
	    // else
	    //   {
	    // 	cout << __FILE__<< "  " << __LINE__ << " FEE " << fee << " crc ok, sampa address " << sampa_addr << " channel " << channel << endl;
	    //   }
	    
		
	    unsigned int fee_channel = sampa_addr * 32 + channel;

	    unsigned int x = fee_data[fee]->size(); // & 0xffffffff; // size() is unsigned long
	    bx bxrecord = {x, bx_count};
	    fee_bx[fee][fee_channel].push_back(bxrecord);
	    
	    for  ( unsigned int s = HEADER_LENGTH; s < record_len+1; s++)
	      {
		fee_samples[fee][fee_channel].push_back( fee_data[fee]->at(s + index) );
	      }

	  }
      }

    int status;
    
    while (  (status = prune_bx() ) );
      // {
      // 	cout << __FILE__<< "  " << __LINE__ << " prune return  " << status << endl;
      // }
   
    // now, what is the max sample length per FEE?
    int  maxvectorsize;
    for ( int fee = 0; fee < MAX_FEECOUNT; fee++)
      {
	maxvectorsize = 0;
	for ( int ch = 0; ch < MAX_FEECHANNELS; ch++)
	  {
	    if ( fee_samples[fee][ch].size() > maxvectorsize ) maxvectorsize = fee_samples[fee][ch].size();
	  }
	_max_samples[fee] = maxvectorsize;
      }

      
    // for ( int i = 0; i < maxvectorsize ; i++)
    //   {
    // 	cout << setw(5) << i << " | ";
    // 	for ( int j = 0; j < MAX_FEECOUNT ; j++)
    // 	  {
    // 	    if  ( i < fee_data[j]->size())
    // 	      {
    // 		cout << setw(5) << hex << setw(5) << fee_data[j]->at(i)  << "  " << dec;
    // 	      }
    // 	    else
    // 	      {
    // 		cout << setw(5) <<  setw(5) << " X  "  << "  ";
    // 	      }
    // 	  }
    // 	cout << endl;
    //   }
    
    return 0;
}
    


int oncsSub_idtpcfeev2::iValue(const int fee, const int sample)
{
     decode();

  if ( fee > MAX_FEECOUNT ||
       sample < 0 || sample >= fee_data[fee][0].size() ) return 0;
  unsigned int x = sample/128;
     
  if ( x >= fee_bx[fee][0].size()) return 0;
  return fee_bx[fee][0].at(x).bx_count;
}

int oncsSub_idtpcfeev2::iValue(const int fee, const int ch, const int sample)
{
  decode();
  if ( fee > MAX_FEECOUNT ||
       ch < 0 || ch > 255 ||
       sample < 0 || sample >= fee_samples[fee][ch].size() ) return 0;
  
  //  if ( sample >= fee_samples[fee][ch].size() ) return 0;
  return fee_samples[fee][ch].at(sample);

}

int oncsSub_idtpcfeev2::iValue(const int n, const char *what)
{

  decode();

  if ( strcmp(what,"MAX_SAMPLES") == 0 )
  {
    if ( n < 0 || n >= MAX_FEECOUNT ) return 0;
    
    return _max_samples[n];
  }

  if ( strcmp(what,"IS_PRESENT") == 0 )  // say if a FEE has been seen at all
  {
    if ( n < 0 || n >= MAX_FEECOUNT ) return 0;
    
    if (_max_samples[n] > 0) return 1;
    return 0;
  }

  if ( strcmp(what,"MAX_FEECOUNT") == 0 )
  {
    return MAX_FEECOUNT;
  }


  if ( strcmp(what,"BROKEN") == 0 )
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
      os << setw(6) << iValue(i,"MAX_SAMPLES") << " ";
    }
  os << std::endl;
  
  const int dwidth = 16;  // how many colums to display
 
  for ( int fee = 0; fee < iValue(0,"MAX_FEECOUNT") ; fee++)
    {
      for ( int chblock = 0; chblock < MAX_FEECHANNELS ; chblock+=dwidth)
	{
	  cout << "       +++++++++++++++ FEE " << fee << "   channels " << chblock << " through " << chblock + dwidth << " ++++++++" << endl;

	  for ( int s = 0; s < iValue(fee, "MAX_SAMPLES"); s++)
	    {

	      if ( (s % 128) == 0) // display timing values
		{
		  os << setw(6) << " time " << " | " ;
		  for ( int ch = chblock; ch < chblock + dwidth; ch++)
		    {
		      //	      unsigned bxi = s/128;
		      os << setw(5) << hex << iValue(fee, s) << dec << " ";
		    }
		  os << endl;

		}

	      os << setw(6) << s << " | " ;
	      for ( int ch = chblock; ch < chblock + dwidth; ch++)
		{
		  //if ( s < fee_samples[fee][ch].size()) os << setw(5) << hex << fee_samples[fee][ch].at(s) << dec << " ";
		  //else                                  os << setw(5) <<  "  X  " << " ";
		  os << setw(5) << hex << iValue(fee,ch,s) << dec << " ";

		}
		
	      os << endl;
	    }
      os << endl;
	}
  
    }
}

unsigned short oncsSub_idtpcfeev2::reverseBits(const unsigned short x) const
{
  unsigned short n = x;  
  n = (n >> 1) & 0x55555555 | (n << 1) & 0xaaaaaaaa;
  n = (n >> 2) & 0x33333333 | (n << 2) & 0xcccccccc;
  n = (n >> 4) & 0x0f0f0f0f | (n << 4) & 0xf0f0f0f0;
  n = (n >> 8) & 0x00ff00ff | (n << 8) & 0xff00ff00;
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
      unsigned short x = fee_data[fee]->at(i++);
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

int oncsSub_idtpcfeev2::prune_bx() 
{
  int returncode = 0;
  // we count the frequency of bx values per card
  // if there is more than one value, we eradicate the data from the lowest count
  // if we do drop data, we return 1. We only drop one bx value per round, so we keep calling this
  // until we return 0;

  
  for ( int fee = 0; fee < MAX_FEECOUNT; fee++)
    {
      // we now count how many bx values there are, and what frequency
      std::map <unsigned int, unsigned int  > bxmap;
      
      for ( int ch = 0; ch < MAX_FEECHANNELS; ch++)
	{
	  if ( fee_bx[fee][ch].size() > 0 )  // skip if we don't have data
	    {
	      unsigned int bx = fee_bx[fee][ch].at(0).bx_count; 
	      // have we seen this aleady?
	      if ( ! bxmap.count(bx) )  // no...
		{
		  bxmap[bx] = 1;
		}
	      else  // yes..
		{
		  bxmap[bx] += 1;
		}
	    }
	}
      
      // don't no one hold this crap-code against me... need to get this in a hurry.

      if ( bxmap.size() > 1) // ok, more than one bx
	{
	  std::map <unsigned int, unsigned int>::iterator it;
	  std::map <unsigned int, unsigned int>::iterator list[bxmap.size()];
	  int i = 0;
	  unsigned int lowest = 1000;
	  unsigned int drop_candidate= 0xffffff;
	  for ( it = bxmap.begin(); it != bxmap.end(); ++it)
	    {
	      // cout << __FILE__<< "  " << __LINE__ << " FEE "
	      // 	   << fee << " bx value " << hex << "0x" << it->first << dec << " count  " << it->second 
	      // 	   << " bxmapsize " << bxmap.size() << endl;
	      
	      if ( it->second < lowest)
		{
		  lowest = it->second;
		  drop_candidate = it->first;
		}
	    }
	  //	  cout << __FILE__<< "  " << __LINE__ << " drop candidate: " << hex << "0x" << drop_candidate << dec << " count " << lowest << endl;
	  returncode =1; // say we did drop something here
	  
	  for ( int ch = 0; ch < MAX_FEECHANNELS; ch++)
	    {
	      if ( fee_bx[fee][ch].at(0).bx_count == drop_candidate)
		{
		  fee_bx[fee][ch].erase( fee_bx[fee][ch].begin() );
		  fee_samples[fee][ch].erase( fee_samples[fee][ch].begin(), fee_samples[fee][ch].begin()+128); 
		}
	    }
	}
    }
  return returncode;
}
