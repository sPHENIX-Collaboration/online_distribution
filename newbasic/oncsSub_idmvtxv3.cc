#include "oncsSub_idmvtxv3.h"

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

  cout  << std::setfill('0');
  for ( int i = 0; i < 10; i++)
    {
      cout << hex << setw(2) << get_GBT_value (gbtword, i * 8, 8) << " " << dec;
    }
  cout  << std::setfill(' ');
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

  payload = (unsigned char *) &SubeventHdr->data;  // here begins the payload
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
  
  cout << __FILE__ << " " << __LINE__ <<  " packet count " << packet_cnt << endl;

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

      //cout << __FILE__ << " " << __LINE__ << " found "<< c << " GBT words w0 = " << w0 << " " << hex << w1 << dec << endl;
      
      for ( int i = 0; i < c; i++) gbtvector.push_back(report[i]);
  }   
  
  vector<std::bitset<80>>::const_iterator itr = gbtvector.begin();

  cout << "RDH Header *** " << endl;
  
  // decode the RDH
  b80 = *itr++; //  RDH Word 0
  cout << " h_ver    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::HDRVERSION, 8) << dec << endl;
  cout << " h_len    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::SIZE, 8) << dec << endl;
  cout << " feeid    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::FEEID_LSB, 16) << dec << endl;
  cout << " sourceid = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::SOURCE_ID, 16) << dec << endl;
  cout << " detfield = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW0::DET_FIELD_SB0, 32) << dec << endl;

  b80 = *itr++; //  RDH Word 1
  cout << " LHC BC   = 0x" << hex << (get_GBT_value (b80, Rdh8ByteMapW1::BC_LSB, 16) & 0xfff)  << dec << endl;
  cout << " BCO      = 0x" << hex << (get_GBT_lvalue (b80, Rdh8ByteMapW1::BCO_SB0, 40) & 0xfff)  << dec << endl;
  
  b80 = *itr++; //  RDH Word 2
  cout << " trg_type    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW2::TRG_TYPE_SB0, 32)  << dec << endl;
  cout << " pages_count = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW2::PAGE_CNT_LSB, 16)  << dec << endl;
  cout << " stop_bit    = 0x" << hex << get_GBT_value (b80, Rdh8ByteMapW2::STOP_BIT, 8)  << dec << endl;

  cout << endl;
  
  for ( ; itr != gbtvector.end();  itr++)
    {
      unsigned int marker =  get_GBT_value (*itr, 72,8);
      //      cout << __FILE__ << " " << __LINE__ << " marker: " << hex << marker << dec << endl;
      switch (marker)
	{
	case WordTypeMarker::IHW:
	  cout << " IHW        "; 
	  break;

	case WordTypeMarker::TDH:
	  cout << " TDH        ";
	  break;

	case WordTypeMarker::TDT:
	  cout << " TDT        ";
	  break;

	case WordTypeMarker::DDW:
	  cout << " DDW        ";
	  break;

	case WordTypeMarker::CDW:
	  cout << " CDW        ";
	  break;
	  
	default:
	  cout << " chip data  ";
	  
	  break;
	}
      pretty_print(*itr);
      cout <<  endl;
    }
      
  //     b80 = 0xffff;
  //     b80 = ((*itr)>>64) & b80; 
  //     unsigned long x = b80.to_ulong();
  //     cout << hex << setw(4) << x << "  " << (*itr) << dec << endl;
  //   }
      
  
  
  
  // cout << hex << report[0] << "  " << report[1] << "  " << report[2] << "  " << w0 << " " << w1 << dec << endl;
  // b80 = 0xffff;
  // b80 = ((report[0] >> 8) & b80);
  // unsigned long x = b80.to_ulong();
  // cout << x << endl;
  // if ( w1 == 0xab)
  //   {
  //   }
  
  return 0;
}

//_________________________________________________
void oncsSub_idmvtxv3::dump(OSTREAM &os)
{
  //  identify(os);
  decode();
  return;
}

//_________________________________________________
