#include "oncsSub_idinttv0.h"

#include <string.h>
#include <stdint.h>

#define coutfl cout << __FILE__<< "  " << __LINE__ << " "
#define cerrfl cerr << __FILE__<< "  " << __LINE__ << " "

using namespace std;

oncsSub_idinttv0::oncsSub_idinttv0(subevtdata_ptr data)
  :oncsSubevent_w2 (data)
{

 _is_decoded = 0;
 
}


oncsSub_idinttv0::~oncsSub_idinttv0()
{
  std::vector<intt_hit*>::const_iterator hit_itr;

  for ( hit_itr = intt_hits.begin(); hit_itr != intt_hits.end(); ++hit_itr)
    {
      delete (*hit_itr);
    }
}


int oncsSub_idinttv0::intt_decode ()
{

  if (_is_decoded ) return 0;
  _is_decoded = 1;

  unsigned int payload_length = 2 * (getLength() - SEVTHEADERLENGTH)  - getPadding() ;
  
  unsigned int index = 0;
  

  unsigned short *buffer = ( unsigned short *)  &SubeventHdr->data;

  while ( index < payload_length)
    {
      // find the "ba" index
      while ( (buffer[index] & 0xff00) !=  0xba00  )
	{
	  coutfl << "skipping  at " << index << " values " << hex << buffer[index] << dec << endl;
	  index++;
	  if (index >= payload_length)
	    {
	      coutfl << " reached end at  " << index << " values " << hex << buffer[index] << dec << endl;
	      _broken = 1;
	      return -1;
	    }
	}
      
      uint16_t fee = ( buffer[index] >> 4 ) & 0xf;
      uint16_t len = ( buffer[index] ) & 0xf;
      coutfl << "found start at index " << index << " values " << hex << buffer[index] << dec << " fee: " << fee << " len: " << len << endl;
      index++;
      for ( int i = 0; i < len ; i++)
	{
	  //	  coutfl << "adding to ladder " << fee << "  " << hex << buffer[index] << dec << endl;
	  fee_data[fee].push_back(buffer[index++]);
	}
    }

  std::vector<unsigned short>::const_iterator fee_data_iter[MAX_FEECOUNT];
  
  for ( int i = 0 ; i < MAX_FEECOUNT ; i++)
    {
      fee_data_iter[i] = fee_data[i].begin();
      coutfl << "ladder " << i << "  number of words " << fee_data[i].size() << endl;
      
      while (fee_data_iter[i] != fee_data[i].end())
	{
	  
	  uint16_t c = 0;
	  while ( (fee_data_iter[i] != fee_data[i].end()) && c != 0xfeed )
	    {
	      c = *(fee_data_iter[i]);
	      ++(fee_data_iter[i]);
	    }

	  if ( fee_data_iter[i] == fee_data[i].end()) continue;

	  c = *(fee_data_iter[i]);
	  if ( c !=  0xc0de)
	    {
	      coutfl << " error with code " << hex << c << dec << endl;
	      //continue;
	    }
	  
	  uint16_t x[2];
	  
	  ++(fee_data_iter[i]);
	  if ( fee_data_iter[i] == fee_data[i].end()) continue;

	  x[0] = *(fee_data_iter[i]);
	  ++(fee_data_iter[i]);
	  if ( fee_data_iter[i] == fee_data[i].end()) continue;
	  x[1] = *(fee_data_iter[i]);

	  if ( x[0] != 0xfeed && x[1] != 0xc0de )
	    {
	      intt_hit * hit= new intt_hit;
	      hit->ladder  = i;
	      hit->full_fphx  = (x[0] >>  7) & 0x1;   // 1
	      hit->bco        = (x[0]      ) & 0x7f;  // 7
	      hit->adc        = (x[1] >> 13) & 0x7;   // 3
	      hit->chip_id    = (x[1] >>  7) & 0x3f;  // 6
	      hit->channel_id = (x[1]      ) & 0x7f;  // 7
	      hit->word0      = x[0];
	      hit->word1      = x[1];
	 
	      intt_hits.push_back(hit);
	      //coutfl << "list size: " << intt_hits.size() << endl;
	    }
	}
    }
      
  return 0;
}


enum ITEM
{
 F_ADC = 0,
 F_AMPLITUDE,
 F_BCO,
 F_CHIP_ID,
 F_CHANNEL_ID,
 F_FULL_FPHX,
};


int oncsSub_idinttv0::iValue(const int hit, const int field)
{
  intt_decode();
  if ( hit < 0 || hit >= (int) intt_hits.size()) return 0;

  switch (field)
    {
    case F_AMPLITUDE:
      return intt_hits[hit]->amplitude;
      break;
      
    case F_FULL_FPHX:
      return intt_hits[hit]->full_fphx;
      break;
      
    case F_ADC:
      return intt_hits[hit]->adc;
      break;
      
    case F_CHIP_ID:
      return intt_hits[hit]->chip_id;
      break;

    case F_CHANNEL_ID:
      return intt_hits[hit]->channel_id;
      break;

    }

  return 0;
}



long long  oncsSub_idinttv0::lValue(const int hit, const int field)
{
  intt_decode();
  if ( hit < 0 || hit >= (int) intt_hits.size()) return 0;

  switch (field)
    {
    case F_BCO:
      return intt_hits[hit]->bco;
      break;
    }
  
  return 0;
}


int oncsSub_idinttv0::iValue(const int hit, const char *what)
{
  if ( strcmp(what,"NR_HITS") == 0)
    {
      return intt_hits.size();
    }
    
  else if ( strcmp(what,"ADC") == 0)
    {
      return iValue(hit,F_ADC);
    }
    
  else if ( strcmp(what,"AMPLITUDE") == 0)
    {
      return iValue(hit,F_AMPLITUDE);
    }
      
  if ( strcmp(what,"CHIP_ID") == 0)
    {
      return iValue(hit,F_CHIP_ID);
    }

  if ( strcmp(what,"CHANNEL_ID") == 0)
        {
	  return iValue(hit,F_CHANNEL_ID);
	}

  if ( strcmp(what,"FULL_FPHX") == 0)
    {
      return iValue(hit,F_FULL_FPHX);
    }

  return 0;
}

long long  oncsSub_idinttv0::lValue(const int hit, const char *what)
{
  if ( strcmp(what,"BCO") == 0)
    {
      return lValue(hit,F_BCO);
    }
  return 0;
}


void  oncsSub_idinttv0::dump ( OSTREAM& os )
{
  os << "number_of_hits: " << iValue(0, "NR_HITS") << endl;
  intt_decode();
  identify(os);

  os << "number_of_hits: " << iValue(0, "NR_HITS") << endl;

  std::vector<intt_hit*>::const_iterator hit_itr;

  os << " Ladder    BCO    Amp   full_phx ADC  chip_id channel_id " << endl;
  
  for ( hit_itr = intt_hits.begin(); hit_itr != intt_hits.end(); ++hit_itr)
    {
      os  << setw(3) <<   (*hit_itr)->ladder     << " "
	  << setw(9) <<   (*hit_itr)->bco        << " " 
	  << setw(5) <<   (*hit_itr)->amplitude  << " " 
	  << setw(5) <<   (*hit_itr)->full_fphx  << "     " 
	  << setw(5) <<   (*hit_itr)->adc        << "   " 
	  << setw(5) <<   (*hit_itr)->chip_id    << " " 
	  << setw(5) <<   (*hit_itr)->channel_id << "         "
	  << "0x" << setw(4) <<  hex << setfill('0') << (*hit_itr)->word0      << " "
	  << "0x" << setw(4) << (*hit_itr)->word1	
	  <<  setfill(' ') << dec << endl;
      
    }
  
}


unsigned short oncsSub_idinttv0::reverseBits(const unsigned short x) const
{
  unsigned short n = x;  
  n = ((n >> 1) & 0x55555555) | ((n << 1) & 0xaaaaaaaa);
  n = ((n >> 2) & 0x33333333) | ((n << 2) & 0xcccccccc);
  n = ((n >> 4) & 0x0f0f0f0f) | ((n << 4) & 0xf0f0f0f0);
  n = ((n >> 8) & 0x00ff00ff) | ((n << 8) & 0xff00ff00);
  //n = (n >> 16) & 0x0000ffff | (n << 16) & 0xffff0000;
  return n;
}



unsigned short oncsSub_idinttv0::crc16(const unsigned int fee, const unsigned int index, const int  l) const 
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

