#include "oncsSub_idinttv0.h"

#include <string.h>
#include <stdint.h>


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
    intt_hits.clear();

}


int oncsSub_idinttv0::intt_decode_hitlist (std::vector<unsigned int> &hitlist , const int fee)
{
  
  //  coutfl << "hit  for fee " << fee << "  size " << hitlist.size()  << endl;
  
  // for ( unsigned int i = 0; i < hitlist.size(); i++)
  //   {
  //     coutfl << setw(3) << i << " " << hex << hitlist[i] << dec << endl;
  //   }
  // cout << endl;

  if ( hitlist.size() < 3)
    {
      //coutfl << "hitlist too short " << hitlist.size() << endl;
      return 1;
    }
	
  unsigned long long BCO = 0;
  unsigned long long l = 0;
  
  // we assemble the BCO from 1 byte from hitlist[0]
  l = hitlist[0];
  BCO |= ( ((l >> 16 ) & 0xff) << 32);
  // and 8 bytes from hitlist[1]
  l = hitlist[1];
  BCO |= ( (l & 0xffff) << 16);
  BCO |= ( (l >> 16) & 0xffff);

  unsigned int event_counter =hitlist[2];
  //  coutfl << "pushing back  0x" << hex << BCO << dec << " fee = " << fee << " hitlist size  = " << hitlist.size() << endl;

  BCO_List.insert(BCO);

  FEE_BCO_Association[BCO].insert(fee);


  for  (unsigned int i = 3; i < hitlist.size(); i++)
    {
      unsigned int x = hitlist[i];
      intt_hit * hit= new intt_hit;
      hit->event_counter = event_counter;
      hit->fee        = fee;
      hit->bco        = BCO;
      hit->channel_id = (x >> 16) & 0x7f;  // 7bits
      hit->chip_id    = (x >> 23) & 0x3f;  // 6
      hit->adc        = (x >> 29) & 0x7;   // 3
      
      hit->FPHX_BCO   = x  & 0x7f;
      hit->full_FPHX  = (x >> 7) & 0x1;   // 1
      hit->full_ROC   = (x >> 8) & 0x1;   // 1
      hit->amplitude  = (x >> 9) & 0x3f;   // 1
      hit->word      = x;

      intt_hits.push_back(hit);
    }	  

  return 0;
}
  
int oncsSub_idinttv0::intt_decode ()
{

  if (_is_decoded ) return 0;
  _is_decoded = 1;

  unsigned int payload_length = ( getLength() - SEVTHEADERLENGTH)  - getPadding() ;
  
  unsigned int index = 0;
  

  unsigned int *buffer = ( unsigned int *)  &SubeventHdr->data;

  while ( index < payload_length)
    {
      // find the "ba" index
      while ( (buffer[index] & 0xff00ffff ) !=  0xf000caf0 )
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

      
      unsigned short fee = ( buffer[index] >> 20 ) & 0xf;
      unsigned short len = ( (buffer[index] >> 16) & 0xf) >>1;
      //coutfl << "found start at index " << index << " values " << hex << buffer[index] << dec << " fee: " << fee << " len: " << len << endl;
      index++;

      for ( int i = 0; i < len ; i++)
	{
	  //coutfl << "adding to ladder " << fee << "  " << hex << buffer[index] << dec << endl;
	  fee_data[fee].push_back(buffer[index++]);
	}
    }

  // for ( int fee = 0; fee < MAX_FEECOUNT; fee++)
  //   {
  //     for ( unsigned int i = 0; i < fee_data[fee].size(); i++)
  // 	{
  // 	  coutfl << setw(3) << fee << "  " << hex << fee_data[fee][i] << dec << endl;
  // 	}
  //     cout << endl;
  //   }
  


  for ( int fee = 0 ; fee < MAX_FEECOUNT ; fee++)
    {

      unsigned int j;

      // for ( j = 0;  j <  fee_data[fee].size(); j++)
      // 	{
      // 	  coutfl << "fee " << fee << "  " << j << " found code 0x" << hex << fee_data[fee][j] << dec << endl;
      // 	}


      //      int go_on = 0;
      int header_found = 0;
      
      std::vector<unsigned int> hitlist;
      j = 0;
      
      while ( j < fee_data[fee].size() )
	{
	  
	  //skip until we have found the first header
	  if (! header_found &&  (fee_data[fee][j] & 0xff00ffff )!= 0xad00cade )
	    {
	      j++;
	      continue;
	    }
	  header_found = 1;

	  // here j points to a "cade" word

	  // push back the cdae word, the BCO, and event counter
	  if ( fee_data[fee].size() -j >=3 )
	    {
	      for ( int k = 0; k < 3; k++) hitlist.push_back(fee_data[fee][j++]);
	    }
	  else
	    {
	      //coutfl << " Warning - index is " << j << " and size is " << fee_data[fee].size() << endl;
	      j+= fee_data[fee].size() -j;
	    }

	  // ok, now let's go until we hit the end, or hit the next header, or a footer
	  while ( j < fee_data[fee].size() )
	    {
	      
	      // we break here if find the next header or a footer
	      if ( ( fee_data[fee][j] & 0xff00ffff ) == 0xad00cade )
		{
		  header_found  = 0;
		  j--;
		  // we have a full hitlist in the vector here
		  //coutfl << "calling decode with size " << hitlist.size() << endl;
		  intt_decode_hitlist (hitlist, fee);
		  hitlist.clear();
		  break;
		}
	      
	      
	      if ( fee_data[fee][j] == 0xcafeff80 )
		{
		  // we have a full hitlist in the vector here
		  //		  coutfl << "calling decode with size " << hitlist.size() << endl;
		  intt_decode_hitlist (hitlist, fee);
		  hitlist.clear();
		  j++;
		  break;
		}
	      
	      hitlist.push_back(fee_data[fee][j]);

	      j++;
	    }
	}

      //      coutfl << "calling decode with size " << hitlist.size() << endl;
      intt_decode_hitlist (hitlist, fee);
      hitlist.clear();
    
    }
  return 0;
}

enum ITEM
{
 F_BCO = 1,
  F_FEE,
  F_CHANNEL_ID,
  F_CHIP_ID,
  F_ADC,
  F_FPHX_BCO,
  F_FULL_FPHX,
  F_FULL_ROC,
  F_AMPLITUDE,
  F_DATAWORD
};


int oncsSub_idinttv0::iValue(const int hit, const int field)
{
  intt_decode();
  if ( hit < 0 || hit >= (int) intt_hits.size()) return 0;

  switch (field)
    {
    case F_FEE:
      return intt_hits[hit]->fee;
      break;

    case F_CHANNEL_ID:
      return intt_hits[hit]->channel_id;
      break;
      
    case F_CHIP_ID:
      return intt_hits[hit]->chip_id;
      break;
      
    case F_ADC:
      return intt_hits[hit]->adc;
      break;
      
    case F_FPHX_BCO:
      return intt_hits[hit]->FPHX_BCO;
      break;
      
    case F_FULL_FPHX:
      return intt_hits[hit]->full_FPHX;
      break;
      
    case F_FULL_ROC:
      return intt_hits[hit]->full_ROC;
      break;
      
    case F_AMPLITUDE:
      return intt_hits[hit]->amplitude;
      break;
      
    case F_DATAWORD:
      return intt_hits[hit]->word;
      break;

    }

  return 0;
}

int  oncsSub_idinttv0::iValue(const int i, const int j, const char * what)
{
  //coutfl << "do we get called? " << endl;
  
  // so here we have the index i of the BCO, and the position j of that FEE
  if ( strcmp(what,"FEELIST") == 0)
    {
      
      unsigned long long BCO = lValue(i,"BCOLIST");
      if ( BCO == 0) return -1;
      unsigned int uj = j;
      if ( j < 0 || uj >=FEE_BCO_Association[BCO].size() )
	{
	  return -1;
	}
      auto it = FEE_BCO_Association[BCO].cbegin();
      for (unsigned int k = 0; k< uj; k++) ++it;
      return *it;
    }

  int fee = i;
  int index=j;

  if ( fee < 0 || fee >= MAX_FEECOUNT) return 0;

  if ( index < 0 || (unsigned int) index >= fee_data[fee].size() ) return 0;
  intt_decode();
  return fee_data[fee][index];


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
  //  unsigned int i= hit; //  we compare with size() later that is unsigned
  intt_decode();

  if ( strcmp(what,"NR_HITS") == 0)
    {
      return intt_hits.size();
    }
    
  if ( strcmp(what,"NR_BCOS") == 0)
    {
      return BCO_List.size();
    }
    
  //the new API "get the number of fees associated with the BCO i
  // we do not have an i or lValue that would take an long long argument
  if ( strcmp(what,"NR_FEES") == 0)
    {
      unsigned long long BCO = lValue(hit,"BCOLIST");
      if ( BCO == 0) return 0;
      return FEE_BCO_Association[BCO].size();
      //eturn FEE_List.size();
    }
    
  if ( strcmp(what,"FEE_LENGTH") == 0)
    {
      if ( hit < 0 || hit >= MAX_FEECOUNT) return 0;
      return fee_data[hit].size();
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

  if ( strcmp(what,"FEE") == 0)
    {
      return iValue(hit,F_FEE);
    }

  if ( strcmp(what,"FPHX_BCO") == 0)
    {
      return iValue(hit,F_FPHX_BCO);
    }

  if ( strcmp(what,"FULL_FPHX") == 0)
    {
      return iValue(hit,F_FULL_FPHX);
    }

  if ( strcmp(what,"FULL_ROC") == 0)
    {
      return iValue(hit,F_FULL_ROC);
    }

  if ( strcmp(what,"DATAWORD") == 0)
    {
      return iValue(hit,F_DATAWORD);
    }






  return 0;
}

long long  oncsSub_idinttv0::lValue(const int hit, const char *what)
{
  intt_decode();

  if ( strcmp(what,"BCO") == 0)
    {
      return lValue(hit,F_BCO);
    }

  unsigned int i= hit; //  size() is unsigned
  if ( strcmp(what,"BCOLIST") == 0)
    {
      if ( hit < 0 || i >= BCO_List.size()) return 0;
      auto it = BCO_List.cbegin();
      for (unsigned int j = 0; j< i; j++) ++it;
      return *it;
    }

  return 0;
}


void  oncsSub_idinttv0::dump ( OSTREAM& os )
{

  //  os << "number_of_hits: " << iValue(0, "NR_HITS") << endl;
  intt_decode();
  identify(os);

  os << " Number of unique BCOs: " << iValue(0, "NR_BCOS") << endl;
  for ( int b = 0; b < iValue(0, "NR_BCOS"); b++)
    {
      os << " BCO " << setw(3) << b << ":  0x" << hex << lValue(b, "BCOLIST") << dec << "    number of FEEs for this BCO " << setw(3) << iValue(b,"NR_FEES") <<  endl;
      os << "           Number of unique FEEs: ";
      for ( int i = 0; i < iValue(b, "NR_FEES"); i++)
	{
	  os << " " << setw(3) << iValue(b, i, "FEELIST");
	}
      os << endl;
    }

  os << " Number of hits: " << iValue(0, "NR_HITS") << endl;


  os << "   #    FEE    BCO      chip_BCO  chip_id channel_id    ADC  full_phx full_ROC Ampl." << endl;

  for ( int i = 0; i < iValue(0, "NR_HITS"); i++)
    {
      os << setw(4) << i << " "
	 << setw(5) <<             iValue(i, F_FEE)     << " "
	 <<  hex <<  setw(11) <<   lValue(i, F_BCO)  << dec << "   " 
	 <<  hex <<  setw(2) << "0x" <<  iValue(i,F_FPHX_BCO)  << dec  << "   " 
	 << setw(5) <<             iValue(i,F_CHIP_ID)    << " " 
	 << setw(9) <<             iValue(i,F_CHANNEL_ID) << "     "
	 << setw(5) <<             iValue(i,F_ADC)        << " " 
	 << setw(5) <<             iValue(i,F_FULL_FPHX) << " "
	 << setw(9) <<             iValue(i,F_FULL_ROC)
	 << setw(8) <<             iValue(i,F_AMPLITUDE) 
	 << "     " 
	 << "0x" << setw(8) <<  hex << setfill('0') << iValue(i,F_DATAWORD)
	 <<  setfill(' ') << dec << endl;
      
    }
  os << endl;
  
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

