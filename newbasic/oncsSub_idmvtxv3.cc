#include "oncsSub_idmvtxv3.h"

// for memset
#include <string.h>  


using namespace std;


oncsSub_idmvtxv3::oncsSub_idmvtxv3(subevtdata_ptr data)
  : oncsSubevent_w4(data)
{
  m_is_decoded = false;
  payload = 0;
  payload_position = 0;
}

// 01234567890 123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789
// 00000000000 000000000 0000000000 0000000000 1010101000 0000000000 0000000000 1000000000

//_________________________________________________
void  oncsSub_idmvtxv3::pretty_print( const std::bitset<80> gbtword) const 
{

  //  cout  << std::setfill('0');
  for ( int i = 0; i < 10; i++)
    {
      //  cout << hex << setw(2) << get_GBT_value (gbtword, i * 8, 8) << " " << dec;
    }
  //cout  << std::setfill(' ');
}

//_________________________________________________
int  oncsSub_idmvtxv3::decode_lane( const std::vector<uint8_t> v) 
{

  uint32_t pos = 0;
  std::vector<uint8_t>::const_iterator itr = v.begin();

  int ret = 0; // currently we just print stuff, but we will add stuff to our
               // structures and return a status later (that's why it's not a const function)

  unsigned int chipid;
  unsigned int region_id;
  unsigned int readout_flags;
  unsigned int addr;
  unsigned int bunchcounter;
  unsigned int encoder_id;

  mvtx_hit *h = 0;


  
  while ( pos < v.size() )
    {
      unsigned int marker = v[pos];
      if  ((marker >> 4) ==  0xa)
	{
	  //	  cout  << " chip header found at position " << pos << endl;
	  break;
	}
      pos++; 
    }
      
  while ( pos < v.size() )
    {
      if ( pos >= v.size() ) break;
      unsigned int marker = v[pos++];
      
      if  ((marker >> 4) ==  0xa)  // chip header
	{
	  chipid    = marker & 0xf;
	  bunchcounter = v[pos++];
	  if ( pos >= v.size() ) break;
	  // cout  << " chip header " << hex << marker << " chip id: " << chipid << " bunch " << bunchcounter << dec;
	}
  
      else if  ((marker ) ==  0xff)  // idle frame
	{
	  // cout  << " idle frame " << hex << marker <<  dec;
	  if ( pos >= v.size() ) break;
	}
  
      else if  ((marker >> 4) ==  0xe)  // chip empty frame
	{
	  chipid    = marker & 0xf;
	  bunchcounter = v[pos++];
	  if ( pos >= v.size() ) break;
	  // cout  << " chip empty frame " << hex << marker << " chip id: " << chipid << " bunch " << bunchcounter << dec;
	}


      else if  ((marker >> 4) ==  0xb)  // chip trailer
	{
	  readout_flags    = marker & 0xf;
	  // cout  << " chip header " << hex << marker << " readout_flags " << readout_flags << dec;
	}

      else if  ((marker & 0xe0) ==  0xc0)  // region header 110x xxxx
	{
	  region_id    = marker & 0x1f;
	  // cout  << " chip region hdr " << hex << marker << " region id " << region_id  << " next word is " << (unsigned short) v[pos] << dec;
	}

      else if  ((marker & 0xc0) ==  0x40) // we have a DATA short report
	{
	  unsigned short secondword = v[pos++];
	  if ( pos >= v.size() ) break;
	  encoder_id     = (marker >>2) & 0xf;
	  addr           = (marker & 0x3) << 8 | secondword;
	  // cout  << " data short hdr " << hex << setw(2) << marker << " " << setw(2) << secondword 
	  // 	<< " encoder_id " << setw(5) << encoder_id
	  // 	<< " addr " << setw(5) << addr << dec ;

	  h = new mvtx_hit;
	  memset(h, 0, sizeof(*h));
	  
	  h->BCO = last_BCO;
	  h->source_id = last_source_id;
	  h->fee_id = last_fee_id;
	  h->lane = last_lane;
	  h->addr  = addr;
	  hit_vector.push_back(h);
	}

      else if  ((marker & 0xc0) ==  0x00) // we have a DATA long report
	{
	  unsigned short secondword = v[pos++];
	  if ( pos >= v.size() ) break;
	  unsigned short bits  = v[pos++];
	  if ( pos >= v.size() ) break;
	  encoder_id     = (marker >>2) & 0xf;
	  addr           = (marker & 0x3) << 8 | secondword;
	  // cout  << " data long hdr  " << hex << setw(2) << marker << " " << setw(2) << secondword << " " << setw(2) << bits
	  // 	<< " encoder_id " << setw(5) << encoder_id
	  // 	<< " addr " << setw(5) << addr; 
	  h = new mvtx_hit;
	  memset(h, 0, sizeof(*h));
	  
	  h->BCO = last_BCO;
	  h->source_id = last_source_id;
	  h->fee_id = last_fee_id;
	  h->lane = last_lane;
	  h->addr  = addr;
	  hit_vector.push_back(h);
	  
	  while ( bits != 0)
	    {
	      addr++;
	      if ( bits & 1)
		{
		  // another bit in a data long
		  h = new mvtx_hit;
		  memset(h, 0, sizeof(*h));
		  
		  h->BCO = last_BCO;
		  h->source_id = last_source_id;
		  h->fee_id = last_fee_id;
		  h->lane = last_lane;
		  h->addr  = addr;
		  hit_vector.push_back(h);


		  // cout << setw(5) << addr;
		}
	      bits >>=1;
	    }
	  
	}
  
      else if  ((marker & 0xF0) == 0xB0) // we have a CHIP trailer
	{
	  readout_flags     = (marker >> 4) & 0xf;
	  //  cout  << " chip trailer " << hex  << " readout flags " << setw(5) << readout_flags << dec;
	}

      //cout << endl;
    }
  
  return ret;
  
}

//_________________________________________________
int  oncsSub_idmvtxv3::decode_chipdata( const std::bitset<80> gbtword) 
{
  // unsigned int chipid;
  // unsigned int timestamp;
  // unsigned int region_id;
  // unsigned int dl_bitmap;
  // unsigned int readout_flags;
  // unsigned int layer;
  // unsigned int chipnr;
  // unsigned int addr;

  unsigned int marker =  get_GBT_value (gbtword, 72,8);
  unsigned int laneidx = marker & 0x1F;

  // add a check if that lane matches the embedded lane
  
  std::map<unsigned int, std::vector<uint8_t>>::iterator itr;
  itr = chipdata.find(laneidx);
  // if ( itr == chipdata.end())
  //   {
  //     cout << __FILE__ << " " << __LINE__ << " new laneidx " << laneidx << endl;
  //   }
  // else
  //   {
  //     cout << __FILE__ << " " << __LINE__ << " adding to laneidx " << laneidx << endl;
  //   }
  
  //take the first 9 bytes off
  for ( unsigned int index = 0; index < 72; index += 8)
    {
      chipdata[laneidx].push_back ( get_GBT_value (gbtword, index,8));
    }

  return 0;
}


//_________________________________________________
unsigned long  oncsSub_idmvtxv3::get_GBT_value( const std::bitset<80> gbtword, const int pos, const int size) const 
{
  unsigned long value = 0;
  
  for (int i = 0; i < size; i++)
    {
      //cout << __FILE__ << " " << __LINE__ << " pos " << pos+i << " " << gbtword.test(pos+i) << endl;
      value |= ( gbtword.test(pos+i) << i) ;
    }
  return value;
}

//_________________________________________________
// this one can do sizes > 32 bit (like the BCO)
unsigned long  long oncsSub_idmvtxv3::get_GBT_lvalue( const std::bitset<80> gbtword, const int pos, const int size) const 
{
  unsigned long long value = 0;
  
  for (int i = 0; i < size; i++)
    {
      //cout << __FILE__ << " " << __LINE__ << " pos " << pos+i << " " << gbtword.test(pos+i) << endl;
      value |= ( gbtword.test(pos+i) << i) ;
    }
  return value;
}

//_________________________________________________
// this function reads in 32 bytes  =  3 GBT words and 2 bytes 
int oncsSub_idmvtxv3::read_32( std::bitset<80> &gbtword0
			       , std::bitset<80> &gbtword1 
			       , std::bitset<80> &gbtword2
			       , unsigned short &w0
			       , unsigned short &w1 )
{


  std::bitset<80> gbtw[3] = {0};
  std::bitset<80> b80;

  for ( int k = 0; k < 3; k++)
    {
      // we shift the bytes in so it looks like a normal binary number
      for ( int i = 9; i >=0 ; i--)
	{
	  b80 = payload[i + payload_position];
	  gbtw[k] = ( gbtw[k] << 8) | b80;
	}
      payload_position +=10;
    }
  gbtword0 = gbtw[0];
  gbtword1 = gbtw[1];
  gbtword2 = gbtw[2];
    
  w0 = payload[payload_position++];
  w1 = payload[payload_position++];

  return 0;
}
//_________________________________________________
int oncsSub_idmvtxv3::decode()
{
  if (m_is_decoded)
  {
    return 0;
  }
  m_is_decoded = true;

  payload = (uint8_t *) &SubeventHdr->data;  // here begins the payload
  payload_position = 0;

  
  unsigned int dlength = getDataLength() - getPadding(); //padding is supposed to be in units of dwords, this assumes dwords
  dlength *= 4;

  std::bitset<80> report[3] = {0};
  std::bitset<80> b80;
  unsigned short w0,  w1, old_len;
  w1 = 0;
  
  do
    {
      read_32( report[0], report[1], report[2], w0, w1);
    }  while (w1 != 0xab);
  // here we have found the "ab" unit

  unsigned int packet_cnt = get_GBT_value (report[2],flxhdr_word2::DMA_WRD_CTR_LSB,16);
  
  //  cout << __FILE__ << " " << __LINE__ <<  " packet count " << packet_cnt << endl;

  w0 = w1 = old_len = 0;

  for ( unsigned int i = 0 ; i < packet_cnt; i++)
    {
      // do we still have 32 bytes to read?
      if (payload_position >  dlength - 32 )
	{
	  cerr <<  __FILE__ << " " << __LINE__ <<  " unexpected end of data at pos " << payload_position << " length = " << dlength << endl;
	  break;
	}
      read_32( report[0], report[1], report[2], w0, w1);

      //calculate how many GBT words these 32 bytes contain
      unsigned short c = (w1 << 8 ) + w0  - old_len;
      old_len = (w1 << 8 ) + w0;

      //  cout << __FILE__ << " " << __LINE__ << " found "<< c << " GBT words w0 = " << hex << w0 << " "  << w1 << dec << endl;
      
      for ( int i = 0; i < c; i++) gbtvector.push_back(report[i]);
  }   

  //  cout << __FILE__ << " " << __LINE__ << " number of GBT words " << gbtvector.size()  << endl;
  
  vector<std::bitset<80>>::const_iterator itr = gbtvector.begin();

//  cout << "RDH Header *** " << endl;

  last_BCO = get_GBT_lvalue (b80, Rdh8ByteMapW1::BCO_SB0, 40);
  last_source_id = get_GBT_value (b80, Rdh8ByteMapW0::SOURCE_ID, 16);
  last_fee_id = get_GBT_value (b80, Rdh8ByteMapW0::FEEID_LSB, 16);
  
  // decode the RDH
  b80 = *itr++; //  RDH Word 0
  // cout << " h_ver    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::HDRVERSION, 8) << dec << endl;
  // cout << " h_len    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::SIZE, 8) << dec << endl;
  // cout << " feeid    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::FEEID_LSB, 16) << dec << endl;
  // cout << " sourceid = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::SOURCE_ID, 16) << dec << endl;
  // cout << " detfield = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::DET_FIELD_SB0, 32) << dec << endl;

  b80 = *itr++; //  RDH Word 1
  // cout << " LHC BC   = 0x" << hex << (get_GBT_value (b80, Rdh8ByteMapW1::BC_LSB, 16) & 0xfff)  << dec << endl;
  // cout << " BCO      = 0x" << hex << get_GBT_lvalue (b80, Rdh8ByteMapW1::BCO_SB0, 40)   << dec << endl;
  
  b80 = *itr++; //  RDH Word 2
  // cout << " trg_type    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW2::TRG_TYPE_SB0, 32)  << dec << endl;
  // cout << " pages_count = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW2::PAGE_CNT_LSB, 16)  << dec << endl;
  // cout << " stop_bit    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW2::STOP_BIT, 8)  << dec << endl;

  //  cout << endl;
  
  for ( ; itr != gbtvector.end();  itr++)
    {
      unsigned int marker =  get_GBT_value (*itr, 72,8);
      //  cout << __FILE__ << " " << __LINE__ << " marker: " << hex << marker << dec << endl;
      if ( marker == WordTypeMarker::IHW)
	{
	  //	  cout << " IHW        "; 
	  pretty_print(*itr);
	}
      else if ( marker == WordTypeMarker::TDH)
	{
	  // mlp needs to decode this ..... BCO  pg 11
	  //cout << " TDH        ";
	  pretty_print(*itr);
	}
      else if ( marker == WordTypeMarker::TDT)
	{
	  //cout << " TDT        ";
	  pretty_print(*itr);
	}
      else if ( marker == WordTypeMarker::DDW)
	{
	  //cout << " DDW        ";
	  pretty_print(*itr);
	}
      else if ( marker == WordTypeMarker::CDW)
	{
	  //cout << " CDW        ";
	  pretty_print(*itr);
	}
      else if ( marker >>5 == 0b101 )
	{
	  //cout << " IB DIAG DATA ";
	  pretty_print(*itr);
	}
      else if ( marker >>5 == 0b001 )
	{
	  //cout << " IB data ";
	  pretty_print(*itr);
	  decode_chipdata (*itr);
	}	
      else if ( marker >>5 == 0b110 )
	{
	  //cout << " OB DIAG DATA ";
	  pretty_print(*itr);
	}
      else if ( marker >>5 == 0b101 )
	{
	  //cout << " IB DIAG DATA ";
	  pretty_print(*itr);
	}
      else if ( marker == 0xe4 )
	{
	  //cout << " DIAGNOSTIC DATA WORD (DDW) ";
	  pretty_print(*itr);
	}
      else if ( marker == 0xF8)
	{
	  //cout << " CALIBRATION DATA WORD ";
	  pretty_print(*itr);
	}
      else
	{
	  //cout << "  unknown data  ";
	  pretty_print(*itr);
	  //cout << "   " ;
	}
      // cout <<  endl;
    }
  
  std::map<unsigned int, std::vector<unsigned char>>::iterator mitr;
  for ( mitr = chipdata.begin(); mitr != chipdata.end(); ++mitr)
  {

    
    //    cout << " lane: " << hex << mitr->first << "   ";
    last_lane = mitr->first ;
    
    // for (unsigned int i = 0; i < mitr->second.size() ; i++)
    //   {
    // 	cout << (unsigned short ) mitr->second[i] << " ";
    //   }
    // cout << endl << endl;
    
    decode_lane (  mitr->second );

  }

  
  return 0;
}


int oncsSub_idmvtxv3::iValue(const int n, const char *what)
{

  decode();
  unsigned int i = n;
  
  if ( strcmp(what,"NR_HITS") == 0 )  // the number of datasets
  {
    return hit_vector.size();
  }

  if ( i >= hit_vector.size())
    {
      return 0;
    }
  
  else if ( strcmp(what,"SOURCE_ID") == 0 )
    {
      return hit_vector[i]->source_id;
    }

  else if ( strcmp(what,"LANE") == 0 )
    {
      return hit_vector[i]->lane;
    }

  else if ( strcmp(what,"ENCODER_ID") == 0 )
    {
      return hit_vector[i]->encoder_id;
    }

  else if ( strcmp(what,"ADDR") == 0 )
    {
      return hit_vector[i]->addr;
    }

  return 0;
}

//_________________________________________________
void oncsSub_idmvtxv3::dump(OSTREAM &os)
{
  //  identify(os);
  decode();

  unsigned int n;

  unsigned int nr_hits = iValue(0, "NR_HITS");

  os << "Number of hits: " << nr_hits << endl;

  os << " hit number   addr   enc.id   lane   src.id " << endl;  

  for ( n = 0; n < nr_hits; n++)
    {
      os << setw(6) << n
	 << "     " << setw(6) << iValue(n, "ADDR")
	 << "  " << setw(6)  << iValue(n, "ENCOODER_ID")    
	 << "  " << setw(6)  << iValue(n, "LANE")    
	 << "  " << setw(6)  << iValue(n, "SOURCE_ID")
	 << endl;
    }

  
  return;
}

//_________________________________________________
oncsSub_idmvtxv3::~oncsSub_idmvtxv3()
{

  chipdata.clear();
  
  std::vector<mvtx_hit *>::iterator itr = hit_vector.begin();
  for ( ; itr != hit_vector.end(); ++itr)
    {
      delete (*itr);
    }
  hit_vector.clear();
  
}
