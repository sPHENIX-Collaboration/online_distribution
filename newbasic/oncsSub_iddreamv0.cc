#include "oncsSub_iddreamv0.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <vector>

#include <arpa/inet.h>  // for htons


using namespace std;

oncsSub_iddreamv0::oncsSub_iddreamv0(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{

  _is_decoded = 0;
  

  swapped_array = 0;
}

oncsSub_iddreamv0::~oncsSub_iddreamv0()
{

  if (swapped_array) delete [] swapped_array; 

  std::map <int,  FEU_decoded_data *>::iterator it;
      
  for ( it=feu_map.begin() ; it != feu_map.end(); ++it)
    {
      FEU_decoded_data *fd = it->second;
      delete fd;
    }
}


int oncsSub_iddreamv0::iValue ( const int i,const char * what)
{
  int nw;
  decode (&nw);

  
  if ( strcmp(what,"NR_FEU") == 0 )
    {
      return feu_map.size();
    }

  if ( strcmp(what,"FEU_ID") == 0 )
    {
      unsigned ii = i;
      if ( ii >= feu_map.size() ) return 0;
      
      std::map <int,  FEU_decoded_data *>::const_iterator it = feu_map.begin();
      
      for ( int k =0; k < i; k++) ++it;
	
      FEU_decoded_data *fd = it->second;
      
      //      cout << __FILE__ << " " << __LINE__ << " returning feu_id= " << fd->_feu_id  << endl;

      return fd->_feu_id;
    }

  
  FEU_decoded_data * fd = feu_map[i];
  if ( !fd ) return 0;

  if ( strcmp(what,"SAMPLES") == 0 )
    {
      return fd->_nr_samples+1; 
    }

  if ( strcmp(what,"NR_DREAM") == 0 )
    {
      return fd->_nr_dreams; 
    }
  
  if ( strcmp(what,"PEDSUBTRACTED") == 0 )
    {
      return  fd->_feu_P; 
    }

  if ( strcmp(what,"COMMONNOISE") == 0 )
    {
      return  fd->_feu_C; 
    }
  
  if ( strcmp(what,"ZEROSUPPRESSED") == 0 )
    {
      return  fd->_feu_Z; 
    }


  
  return 0;
}


int oncsSub_iddreamv0::iValue(const int feu_id, const int dreamchip, const char *what)
{
  
  
  if ( dreamchip < 0 || dreamchip > 7) return 0;

  FEU_decoded_data * fd = feu_map[feu_id];
  if ( !fd ) return 0;
  
  
  if ( strcmp(what,"DREAM_ENABLED") == 0 )
    {
      return fd->_dream_enabled[dreamchip];
    }
  

  return 0;
}


int oncsSub_iddreamv0::iValue(const int channel, const int sample)
{
  //  int dream = channel/64;
  //int ch = channel%64;
  //return iValue(dream,ch,sample);
  return 0;
}


int oncsSub_iddreamv0::iValue(const int feu_id, const int channel, const int sample)
{

  int nw;
  decode (&nw);

  FEU_decoded_data * fd = feu_map[feu_id];
  if ( !fd ) return 0;
  
  int dreamchip = channel/64;
  int ch = channel%64;

  
  if (! fd->_dream_enabled[dreamchip]) return 0;

  if ( dreamchip < 0 || dreamchip >=8 ) return 0;
  if ( sample < 0 || sample > 254) return 0;

  return fd->samples[dreamchip][ch][sample];
}


void  oncsSub_iddreamv0::dump ( OSTREAM& os )
{
  identify(os);
  int i;
  int nw;
  decode (&nw);

  os << "Nr of FEUs:         " << setw(4) << iValue(0,"NR_FEU") <<endl;
  for ( i = 0; i < iValue(0,"NR_FEU"); i++)
    {
      int id = iValue(i,"FEU_ID");
      os << "FEU_ID:              " << setw(4) << i << setw(4) << id << endl;
      os << " DreamChips:         " << setw(4) << iValue(id,"NR_DREAM") << "  enabled:  ";
      for (int dream = 0; dream < 8; dream++)
	{
	  if ( iValue(id, dream,"DREAM_ENABLED") )
	    {
	      os << " 1 ";
	    }
	  else
	    {
	      os << " X ";
	    }
	}
      os << endl;
      os << " Nr of samples:      " << setw(4) << iValue(id,"SAMPLES") <<endl;
      os << " Pedestal subtracted " << setw(4) << iValue(id,"PEDSUBTRACTED") <<endl;
      os << " Zero suppressed     " << setw(4) << iValue(id,"ZEROSUPPRESSED") <<endl;
      os << " Common Noise supp.  " << setw(4) << iValue(id,"COMMONNOISE") <<endl;
      
  
      for (int dream = 0; dream < 8; dream++)
	{
	  if ( iValue(id,dream,"DREAM_ENABLED") )
	    {
	      os << "                                      ---- Dream chip " << dream << " ---- " << endl;
	      os << " ch-> " ;
	      for (int channel = 0; channel < 32; channel ++)
		{
		  os << setw(5) << channel;
		}
	      os << endl;
	      os << "smple|" ;
	      for (int channel = 32; channel < 64; channel ++)
		{
		  os << setw(5) << channel;
		}
	      os << endl;
	  
	      os << "-----+-" ;
	      for (int channel = 0; channel < 32; channel ++)
		{
		  os << "----+";
		}
	      os << endl;
	  
	      for (int sample = 0; sample < iValue(id,"SAMPLES"); sample ++)
		{
		  os << setw(4) << sample << " |";
	      
		  for (int channel = 0; channel < 32; channel ++)
		    {
		      os << setw(5) << iValue(id, dream*64 + channel, sample);
		    }
		  os << endl;
		  os << "    - ";
		  for (int channel = 32; channel < 64; channel ++)
		    {
		      os << setw(5) << iValue(id, dream*64 +channel, sample);
		    }
		  os << endl;
		}
	      os << endl;
	    }
	}
      
    }
}

int *oncsSub_iddreamv0::decode ( int *nwout)
{
  if ( _is_decoded) return 0;
  _is_decoded = 1;  // mark this as done already
  


  unsigned short *d = (unsigned short *) &SubeventHdr->data;  // here begins the payload
  int dlength =  2 * (getLength()-4 - getPadding() );

  swapped_array = new unsigned short[dlength];

  int array_position = 0;
  for ( int i = 0; i < dlength; i++)
    {
      if (  d[i] == 0xaabb &&  d[i+1] == 0xccdd )
	{
	  int len    = ntohs(d[i+3]) /2;
	  //	  cout << __FILE__ << " " << __LINE__ << " new packet, feu_id= " << feu_id << " len = " << len << endl;
	  i+=4;
	  for ( int k = 0; k < len; k++)
	    {
	      swapped_array[array_position++] = ntohs(d[i+k]);
	    }
	  i+= len-1;
	}
    }

  int arraylength = array_position;

  int status = decode_payload(swapped_array, arraylength);
  if (status) cerr << __FILE__ << " " << __LINE__ << " error status from decode_payload " << status << endl;

  *nwout = 0;
  return 0;
}

  
int oncsSub_iddreamv0::decode_payload ( unsigned short *d, const int size) 
{

  FEU_decoded_data *fd;

  int nwpacket_index = 0;  // this is supposed to point to the network packet start
  int index = 0;
  int feu_index = 0;
  int dream_index = 0;
  int dream_data_length = 0;


  int event_id;
  int time_stamp;

  int sample_id;
  int fine_tstp; 


  int dream_id;
 
  while ( nwpacket_index + 4 < size)
    {

      feu_index = nwpacket_index;
      
      // skip leading 0's
      while ( d[feu_index] == 0 ) feu_index++;

      // FEU header
      index = feu_index;  
      if ( (d[index] & 0x7000) == 0x6000)  // word 0 
	{
	  int feu_id = d[index] & 0xff;
	  
	  // do we know this FEU?
	  fd = feu_map[feu_id];
	  if ( !fd)  // no we don't
	    {
	      //	      cout << __FILE__ << " " << __LINE__ << " new feu_id= " << feu_id << " added" << endl;
	      feu_map[feu_id] = fd = new FEU_decoded_data;
	      fd->_feu_id = feu_id;
	      memset (fd->samples, 0,  8*64*255*sizeof(int) );
	      memset (fd->cellids, 0,  8*64*sizeof(unsigned long long) );
	    }

	  fd->_feu_P  = ( d[index] >>  8) & 0x1;
	  fd->_feu_C  = ( d[index] >>  9) & 0x1;
	  fd->_feu_Z  = ( d[index] >> 10) & 0x1;

	}  // word 0
	  
      index = feu_index+1;                  // word 1
      if ( (d[index] & 0x7000) == 0x6000)
	{
	  event_id = d[index] & 0xfff;
	}
      
      index = feu_index+2;                  // word 2
      if ( (d[index] & 0x7000) == 0x6000)
	{
	  time_stamp = d[index] & 0xfff;
	}
      
      index = feu_index+3;                  // word 3
      if ( (d[index] & 0x7000) == 0x6000)
	{
	  sample_id = ( d[index]>> 3) & 0x1ff;
	  if ( sample_id > fd->_nr_samples) fd->_nr_samples = sample_id;  // and update the max sample nr as needed
	  fine_tstp = ( d[index]) & 0x7;
	}
      //      cout << __FILE__ << " " << __LINE__ <<  " event id " << event_id << " sample id " <<  sample_id << endl;

      time_stamp = ( time_stamp<<3)  | fine_tstp;

      index = feu_index+4 ;
      if (  (d[index] & 0x7000) == 0x6000 )  // do we have the optional header?
	{
	  index += 4;      // skip if so
	}
      
      // done with FEU hdr -----


      // We will maintain dream_index as the index to the deam chip data start 

      
      dream_index = index;
      int done_with_this_feu = 0;

      if ( fd->_feu_Z )   // zero_suppressed data
	{
	
	  while( ( d[index] & 0x6000) != 0x6000 )  // test if x11x == end marker
	    {
	      dream_id = ( d[index] >> 6 ) & 0x7;
	      int channel_id =    d[index]  & 0x3f;
	      int channel_value = d[index+1] & 0xfff;

	      // cout << __FILE__ << " " << __LINE__ << "  " << hex << (d[index] & 0x7fff) << dec
	      // 	   << "  " << setw(5) << dream_id
	      // 	   << "  " << setw(5) << channel_id
	      // 	   << "  " << setw(5) << channel_value << endl;

	      fd->samples[dream_id][channel_id][sample_id] = channel_value;

	      if ( ! fd->_dream_enabled[dream_id] ) fd->_nr_dreams++;          // this is a dream id we hadn't seen before
	      fd->_dream_enabled[dream_id] = 1;                           // mark it as "seen"
	      index += 2;
	    }
	  //	  cout << __FILE__ << " " << __LINE__ << "  " << hex << (d[index] & 0x7fff) << dec << endl;
	  index += 2;

	  nwpacket_index = dream_index = index;
	}
      
      else   // not Zero-suppressed data 
	{

	  while( ! done_with_this_feu)
	    {
	      // first, we figure out where it ends. It should end 74 words in, but we better check.
	      // we first check that dream_index + 68 has a dream header tag, and that +73 shows the same
	      // dream id as our header here. 
	      
	      if ( ( d[dream_index + 68 ] & 0x6000) == 0x4000 )
		{
		  dream_id = (d[dream_index+3] >> 9) & 0x7;
		  int trailer_dream_id = (d[dream_index+73] >> 9) & 0x7;
		  //		  cout << __FILE__ << " " << __LINE__ << " dream_id= " << dream_id   <<  " trailer_dream_id= " << trailer_dream_id << endl;
		  
		  if (  dream_id == trailer_dream_id )   // all ok
		    {
		      dream_data_length = 74;
		    }
		  else
		    {
		      return -1;
		    }
		  
		  if ( ! fd->_dream_enabled[dream_id] ) fd->_nr_dreams++;          // this is a dream id we hadn't seen before
		  fd->_dream_enabled[dream_id] = 1;                           // mark it as "seen"
		  if ( sample_id > fd->_nr_samples) fd->_nr_samples = sample_id;  // and update the max sample nr as needed
		  
		  decode_dream( fd, &d[dream_index], dream_data_length, sample_id);
		  
		  dream_index += dream_data_length;
		  
		}
	      else
		{
		  // cout << __FILE__ << " " << __LINE__ << " FEU trailer reached " << endl;
		  nwpacket_index = dream_index + 2;
		  //  cout << __FILE__ << " " << __LINE__ << " EoE " <<  (( d[dream_index] >> 11) & 1) << " length  "  << (d[dream_index] & 0x7ff) <<dec << endl;
		  done_with_this_feu = 1;
		}
	    }
	}
    }
  return 0;
}

int oncsSub_iddreamv0::decode_dream( FEU_decoded_data *fd, unsigned short *d, const int size, const unsigned int sample_nr)
{
  unsigned long long cell_id;
  unsigned long long trigger_id;

  unsigned int cmnl;
  unsigned int cmnh;
  unsigned int dream_id;

  
  dream_id = (d[3] >> 9) & 0x7;

  if ( dream_id >=8) return -1;       // can have only 0...7
  if ( sample_nr >= 254 ) return -2;  // sample id 0...255 max

  trigger_id =  (d[0] & 0xfff) << 24;
  trigger_id |= (d[1] & 0xfff) << 12;
  trigger_id |= (d[2] & 0xfff);

  cmnl = d[68] & 0xfff;
  cmnh = d[69] & 0xfff;
  
  cell_id =  (d[70] & 0xfff) << 24;
  cell_id |= (d[71] & 0xfff) << 12;
  cell_id |= (d[72] & 0xfff);

  
  fd->cellids [dream_id][sample_nr] = cell_id; 

  
  for ( int i = 0; i < 64; i++)
    {
      fd->samples[dream_id][i][sample_nr] = d[4+i] & 0xfff; 
    }

  return 0;
}

