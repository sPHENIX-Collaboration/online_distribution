#include "oncsSub_idmvtxv0.h"
#include <cstring>
#include <bitset>
#include <map>
#include <vector>

#include <arpa/inet.h>

using namespace std;

oncsSub_idmvtxv0::oncsSub_idmvtxv0(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{
  _is_decoded = 0;
  _unexpected_felix_counter = 0;
  _bad_ruid = 0;
  _bad_ruchns = 0;
  _bad_chipids = 0;
  _highest_row_overall = -1;
  _highest_chip = -1;
  memset ( chip_row, 0, 9*512*32*sizeof(unsigned int));
  memset ( chip_rowmap, 0, 9*512*sizeof(unsigned int));
  for ( int i = 0; i < MAXRUCHN+1; i++)
  {
      _chip_id[i] = -1;
      _highest_region[i] = -1;
      _unexpected_bytes[i] = 0;
      _bunchcounter[i] = -1;
      _header_found[i] = false;
      _trailer_found[i] = false;
      _readout_flags[i] = -1;
  }
  _excess_data_bytes = 0;
}
  
oncsSub_idmvtxv0::~oncsSub_idmvtxv0()
{

}


typedef struct 
{
  unsigned char d0[3][10];
  unsigned char counter;
  unsigned char ruid;
}  data32;

typedef struct
{
  unsigned int row;
  unsigned int region;
  unsigned int bit;
} chipdata;

#define CHIPHEADER     1
#define CHIPEMPTYFRAME 2
#define DATASHORT      3
#define DATALONG0      4
#define DATALONG1      5


int decode_thebit (int the_row, int encoder_id, int address)
{
  // the numbering "snakes" its way through a column (fig. 4.5 in the Alpide manual)
  //  0 1  > >    row 0 
  //  3 2  < <    row 1
  //  4 5  > >    row 2
  //  7 6  < <   so we need to know if we are in an even or odd row
  int is_an_odd_row = the_row & 1;
  int thebit;
  if ( is_an_odd_row)
    {
      thebit = (encoder_id*2) + (  ( address &1) ^ 1 );
    }
  else  
    {
      thebit = (encoder_id*2) + ( address&1);
    }
  return thebit;
}

int *oncsSub_idmvtxv0::decode ()
{
  if ( _is_decoded) return 0;
  _is_decoded = 1;
  
  //int i;

  unsigned int *payload = (unsigned int *) &SubeventHdr->data;  // here begins the payload

  int dlength = getLength()-4 - getPadding();
  unsigned char *the_end = ( unsigned char *) &payload[dlength+1];
  
  unsigned char *pos = (unsigned char *) payload;
  //unsigned char *start = pos;

  //  cout << hex << " pos = " << (unsigned long long ) pos << "  the end  " << (unsigned long long) the_end << dec << endl;

  pos = (unsigned char *) payload;

  //int first =1;

  unsigned char *c;
  unsigned char b;
  unsigned int address;

  map<int, int> map_chip_ru;
  vector<chipdata> running_chip_data;

  vector<unsigned char> ruchn_stream[MAXRUCHN+1];


  //  cout << hex << " pos = " << (unsigned long long ) pos << "  the end  " << (unsigned long long) the_end << dec << endl;

  int the_region = -1;

  int felix_counter = 0;

  while ( pos < the_end )
    {

      data32 *d32 = ( data32*) pos; 

      if (d32->ruid > MAXRUID)
      {
        //cout << __FILE__ << " " << __LINE__ << " --- invalid ruid " << hex << (int) d32->ruid << " at pos " << (long) pos << dec << endl;
        _bad_ruid = d32->ruid;
        break;
      }
      //FELIX counter is 8 bits, max value 256
      //check that the new counter value is consistent with the previous value (it should have increased by no more than 3, and the amount of the difference is the number of data chunks in this FELIX word)
      int counter_increment = (d32->counter + 256 - felix_counter)%256;
      if (counter_increment > 3)
      {
        if (d32->counter != 0)
        {
          //cout << __FILE__ << " " << __LINE__ << " --- unexpected FELIX counter " << hex << (int) d32->counter << " following previous counter value " << felix_counter << " at pos " << (long) pos << dec << endl;
          _unexpected_felix_counter = d32->counter;
        }
        break;
      }
      felix_counter = d32->counter;

      //for (int ichnk = 0; ichnk < 3; ichnk++)
      for (int ichnk = 0; ichnk < counter_increment; ichnk++)
	{
	  unsigned int ruchn = (unsigned int)d32->d0[ichnk][9];
          if (ruchn == RUHEADER)
          {
                //cout << __FILE__ << " " << __LINE__ << " --- RU header " << hex << ruchn << ", full RU word: ";
                //for (int ibyte=9;ibyte>=0;ibyte--)
                //{
                    //cout << setfill('0') << setw(2) << hex << (unsigned int) d32->d0[ichnk][ibyte] << " ";
                //}
                //cout << setfill(' ') << setw(0) << dec << endl;
          }
          else if (ruchn == RUTRAILER)
          {
                //cout << __FILE__ << " " << __LINE__ << " --- RU trailer " << hex << ruchn << ", full RU word: ";
                //for (int ibyte=9;ibyte>=0;ibyte--)
                //{
                    //cout << setfill('0') << setw(2) << hex << (unsigned int) d32->d0[ichnk][ibyte] << " ";
                //}
                //cout << setfill(' ') << setw(0) << dec << endl;
          }
          else if (ruchn > MAXRUCHN)
            {
                _bad_ruchns++;
                cout << __FILE__ << " " << __LINE__ << " --- invalid ruchn " << hex << ruchn << ", full RU word: ";
                for (int ibyte=9;ibyte>=0;ibyte--)
                {
                    cout << setfill('0') << setw(2) << hex << (unsigned int) d32->d0[ichnk][ibyte] << " ";
                }
                cout << setfill(' ') << setw(0) << dec << endl;
            }
            else for ( int ibyte = 0; ibyte < 9; ibyte++)
            {
	      //cout << __FILE__ << " " << __LINE__ << hex << " --- ruchn " << ruchn << " byte " << (unsigned int) d32->d0[ichnk][ibyte] << dec << endl;
	      ruchn_stream[ruchn].push_back(d32->d0[ichnk][ibyte]);
            }
	} // ichnk

      pos += sizeof(*d32);
    } // while (pos < the_end)

  //we ignore ruchn 0 (trigger information and padding words) for now
  /*
  if (ruchn_stream[0].size()!=9) for ( int ruchn = 0; ruchn < MAXRUCHN+1; ruchn++)
  {
    cout << __FILE__ << " " << __LINE__ << " --- ruchn " << ruchn << " has " << ruchn_stream[ruchn].size() << " bytes" << endl;
  }
  */

  for ( int ruchn = 1; ruchn < MAXRUCHN+1; ruchn++)
    {

      bool header_seen=false;
      int status = 0;
      int ibyte_endofdata = -1;
      unsigned int encoder_id;
      for (unsigned int ibyte = 0; ibyte < ruchn_stream[ruchn].size(); ibyte++)
	{
	  b = ruchn_stream[ruchn].at(ibyte);

	  //cout << __FILE__ << " " << __LINE__ << " --- ruchn " << hex << ruchn << " next value " << (unsigned int)  b << dec << " status:" << status << endl;
	  
	  if (status) // we mop up what we started in the last round -
	              // these are all cases with more than one byte
	    {
	      switch (status)
		{
		case CHIPHEADER:
		  
                  _bunchcounter[ruchn] = b;
		  //cout << __FILE__ << " " << __LINE__ << " chip header, chip id= " << chip_id << " bunchctr= " << hex << _bunchcounter[ruchn] << dec << endl;
                  if ( (int)ruchn > _highest_chip) 
                    {
                      _highest_chip = ruchn; 
                    }
		  status = 0;
                  _header_found[ruchn] = true;
		  break;

		case CHIPEMPTYFRAME:
		  
                  _bunchcounter[ruchn] = b;
		  // cout << __FILE__ << " " << __LINE__ << " chip empty frame " << hex << chip_id << " " << _bunchcounter[ruchn] << dec << endl;
                    if ( (int)ruchn > _highest_chip) 
                      {
                        _highest_chip = ruchn; 
                      }
                  _highest_region[ruchn] = 0;
                  ibyte_endofdata = ibyte;
                  _header_found[ruchn] = true;
                  _trailer_found[ruchn] = true;
		  status = 0;
		  break;

		case DATASHORT:
		  address += b;	
		  //cout << __FILE__ << " " << __LINE__ << " data short report, hex:" << hex << address << dec << " enc. id " << encoder_id << " address= " << address << " the_region:" << the_region << " the_chip:" << chip_id;
		  if ( the_region >= 0 && encoder_id >=0 ) 
		    {
		      int the_row = (address >> 1);
		      if ( the_row > 511)
			{
			  cout << __FILE__ << " " << __LINE__ << " impossible row: " << the_row
			       << " encoder " <<  encoder_id << " addr " << address << endl;
			}
		      else
			{
			  int thebit = decode_thebit(the_row, encoder_id, address);
			  //cout << " row:" << the_row << " col:" << the_region*32 + thebit << endl;
			  //  cout << __FILE__ << " " << __LINE__ << " the bit " << thebit << endl;
			  chip_row[ruchn][the_row][the_region] |= ( 1<<thebit);
			  chip_rowmap[ruchn][the_row] |= ( 1<<the_region);
			  if ( the_row > _highest_row_overall)  _highest_row_overall = the_row;

			}
		    }

		  //cout << endl;
		  status = 0;
		  break;

		case DATALONG0:
		  address += b;	
		  status = DATALONG1;
		  break;

		case DATALONG1:
                  if ( (b & 0x80) != 0) //required to be 0
                  {
                      //cout << __FILE__ << " " << __LINE__ << " unexpected word " << hex << (unsigned int) b << dec << " at ibyte " << ibyte << endl;
                      _unexpected_bytes[ruchn]++;
                  }
		  //cout << __FILE__ << " " << __LINE__ << " data long report, hex:" << hex << address << dec << " enc. id " << encoder_id << " address= " << address << " the_region:" << the_region << " the_chip:" << chip_id << endl;
                  // Loop over the hits in the cluster. The pixel specified by the address always has a hit.
                  // The next 7 pixels (in priority encoder order) have hits if the corresponding bit in this byte is high. 
                  // See ALPIDE manual section 3.4.1 on DATA LONG: page 63.
                  for (int ihit = 0; ihit<8; ihit++)
                  {
                      if (ihit==0 || ((b >> (ihit-1)) & 1))
                      {
                      int hit_address = address + ihit;
                      if ( the_region >= 0 && encoder_id >=0 ) 
                        {
                          int the_row = (hit_address >> 1);
                          if ( the_row > 511)
                            {
                              cout << __FILE__ << " " << __LINE__ << " impossible row: " << the_row
                                   << " encoder " <<  encoder_id << " addr " << hit_address << endl;
                            }
                          else
                            {
                              int thebit = decode_thebit(the_row, encoder_id, hit_address);
                              //cout << " row:" << the_row << " col:" << the_region*32 + thebit << endl;
                              //  cout << __FILE__ << " " << __LINE__ << " the bit " << thebit << endl;
                              chip_row[ruchn][the_row][the_region] |= ( 1<<thebit);
                              chip_rowmap[ruchn][the_row] |= ( 1<<the_region);
                              if ( the_row > _highest_row_overall)  _highest_row_overall = the_row;

                            }
                        }
                      }
                  }
		  status = 0;
		  break;
		  
		}
	      c++;
	      continue;
	    }
	    
          if (ibyte_endofdata != -1) break;

	  if ( b == 0xff)  // Idle byte, skip
	    {
	      //cout << __FILE__ << " " << __LINE__ << " IDLE byte " << hex << (unsigned int)  b << dec << endl;
	      status = 0;
	    }

	  else if ( ( b >> 4) == 0xa) // we have a chip header
	    {
	      _chip_id[ruchn] = ( b & 0xf);
              header_seen = true;
	      status = CHIPHEADER;
	    }

	  else if ( ( b >> 4) == 0xb) // we have a chip trailer
	    {
	      // cout << __FILE__ << " " << __LINE__ << " chip trailer, chip id= " << hex << chip_id << dec << endl;
	      // break out of the loop, done with this chip
              ibyte_endofdata = ibyte;
              _trailer_found[ruchn] = true;
              _readout_flags[ruchn] = (b & 0xf);
	      break;
	    }

	  else if ( ( b >> 4) == 0xE) // we have a chip empty frame
	    {
	      _chip_id[ruchn] = ( b & 0xf);
              header_seen = true;
	      status = CHIPEMPTYFRAME;
	    }

	  else if ( ( b >> 5) == 0x6) // we have a region header
	    {
	      unsigned int region_id = (b & 0x1f);
	      if ( region_id <32)
		{
		  the_region = region_id;
		  _highest_region[ruchn] = region_id;
		}
	      else //this is impossible since the region ID is 5 bits
		{
		  cout << __FILE__ << " " << __LINE__ << " wrong region header, id=  " << hex << region_id << dec << endl;
		}
	    }

	  else if ( ( b >> 6) == 0x1) // we have a DATA short report
	    {
	      encoder_id = ( b>>2) & 0xf;
	      address = (b & 0x3) << 8;
	      status = DATASHORT;
	    }

	  else if ( ( b >> 6) == 0x0) // we have a DATA long report
	    {
	      encoder_id = ( b>>2) & 0xf;
	      address = (b & 0x3) << 8;
	      status = DATALONG0;
	    }

	  else if ( b == 0xF1) // we have a BUSY on
	    {
	      //cout << __FILE__ << " " << __LINE__ << " Busy on "  << endl;
	    }
      
	  else if ( b == 0xF0) // we have a BUSY off
	    {
	      //cout << __FILE__ << " " << __LINE__ << " Busy off "  << endl;
	    }
       
	  else
	    {
	      //cout << __FILE__ << " " << __LINE__ << " unexpected word " << hex << (unsigned int) b << dec << " at ibyte " << ibyte << endl;
                _unexpected_bytes[ruchn]++;
	    }
          if (ibyte==0 && !header_seen) break;//first byte of the ALPIDE stream must be a chip header or chip empty; if not, abort so we don't get confused by bad data

	} // ibyte

	//cout << __FILE__ << " " << __LINE__ << "  ruchn " << ruchn << "  ibyte_endofdata " << ibyte_endofdata << endl;
        for (unsigned int ibyte = ibyte_endofdata+1; ibyte < ruchn_stream[ruchn].size(); ibyte++)
	  {
	    b = ruchn_stream[ruchn].at(ibyte);
            if (b!=0)
            {
                //cout << __FILE__ << " " << __LINE__ << " --- ruchn " << hex << ruchn << " unexpected nonzero value " << (unsigned int)  b << dec << " at ibyte " << ibyte << " after ibyte_endofdata " << ibyte_endofdata << endl;
                _excess_data_bytes++;
            }
          }
    } // ruchn


  return 0;
}


int oncsSub_idmvtxv0::iValue(const int ich,const char *what)
{
  decode();
  if ( strcmp(what,"UNEXPECTED_FELIX_COUNTER") == 0 )
  {
    return _unexpected_felix_counter;
  }

  else if ( strcmp(what,"BAD_RUID") == 0 )
  {
    return _bad_ruid;
  }

  else if ( strcmp(what,"BAD_RUCHNS") == 0 )
  {
    return _bad_ruchns;
  }

  else if ( strcmp(what,"BAD_CHIPIDS") == 0 )
  {
    return _bad_chipids;
  }

  else if ( strcmp(what,"HIGHEST_CHIP") == 0 )
  {
    return _highest_chip;
  }

  else if ( strcmp(what,"CHIP_ID") == 0 )
  {
    if ( ich > _highest_chip) return -1; // no such chip
    return _chip_id[ich];
  }

  else if ( strcmp(what,"HIGHEST_REGION") == 0 )
  {
    if ( ich > _highest_chip) return -1; // no such chip
    return _highest_region[ich];
  }

  else if ( strcmp(what,"HIGHEST_ROW") == 0 )
  {
    return _highest_row_overall;
  }

  else if ( strcmp(what,"EXCESS_DATA_BYTES") == 0 )
  {
    return _excess_data_bytes;
  }

  else if ( strcmp(what,"UNEXPECTED_BYTES") == 0 )
  {
    if ( ich > _highest_chip) return -1; // no such chip
    return _unexpected_bytes[ich];
  }

  else if ( strcmp(what,"BUNCHCOUNTER") == 0 )
  {
    if ( ich > _highest_chip) return -1; // no such chip
    return _bunchcounter[ich];
  }

  else if ( strcmp(what,"HEADER_FOUND") == 0 )
  {
    if ( ich > _highest_chip) return -1; // no such chip
    return _header_found[ich]?1:0;
  }

  else if ( strcmp(what,"TRAILER_FOUND") == 0 )
  {
    if ( ich > _highest_chip) return -1; // no such chip
    return _trailer_found[ich]?1:0;
  }

  else if ( strcmp(what,"READOUT_FLAGS") == 0 )
  {
    if ( ich > _highest_chip) return -1; // no such chip
    return _readout_flags[ich];
  }

  return 0;

}

int oncsSub_idmvtxv0::iValue(const int chip, const int region, const int row)
{
  decode();

  if ( chip < 0  || chip > _highest_chip) return 0;
  if ( region < 0 || region > 31 ) return 0;
  if ( row < 0    || row > 511) return 0;
  return chip_row[chip][row][region];
 }


int oncsSub_idmvtxv0::iValue(const int chip, const int row)
{
  decode();

  if ( chip < 0  || chip > _highest_chip) return 0;
  if ( row < 0    || row > 511) return 0;
  return chip_rowmap[chip][row];
 }


void  oncsSub_idmvtxv0::dump ( OSTREAM& os )
{

  identify(os);


  //int x;
  decode();
  os << "Highest chip:      " << setw(4) << iValue(0, "HIGHEST_CHIP") +1<< endl;
  os << "Regions:              ";
  for ( int ichip = 0; ichip < iValue(0, "HIGHEST_CHIP") +1; ichip++)
    {
      os << setw(4) << iValue(ichip, "HIGHEST_REGION");
    }
  os << endl;
  os << "Highest populated row " << iValue(0,"HIGHEST_ROW") << endl;

  // now dump the chip info
  
  for ( int ichip = 0; ichip < iValue(0, "HIGHEST_CHIP")+1; ichip++) // go through the chips
    {
        if (iValue(ichip, "HIGHEST_REGION") == -1) continue; // skip chips we didn't see
      os << "  *** Chip " << ichip << ", chip ID " << iValue(ichip,"CHIP_ID") <<  "  ***" << endl;
      for ( int irow = 0; irow < iValue(0,"HIGHEST_ROW")+1; irow++)
	{
          bool has_hit = false;
	  for ( int iregion = 0; iregion < iValue(ichip, "HIGHEST_REGION")+1; iregion++) // check if there are any hits in this row
	    {
                if (iValue(ichip,iregion, irow) != 0)
                {
                    has_hit = true;
                    break;
                }
            }
          if (has_hit)
            {
	    os << "  Row  Region" << endl;
	    for ( int iregion = 0; iregion < iValue(ichip, "HIGHEST_REGION")+1; iregion++)
	      {
	        os << setw(4) << irow << "  " << setw(4) << iregion << " | ";
	        unsigned int bits =  iValue(ichip,iregion, irow);
	        for ( int i = 0; i < 32; i++)
		  {
		    if ( (bits >> i) & 1)
		      {
		        os << "X";
		      }
		    else
		      {
		        os << "-";
		      }
		  }
	        os << endl;
	      }
	      os << endl;
            }
	}
    }
  
}


void print_stuff(OSTREAM& out, unsigned int data, int width, int shift, bool blank)
{
    unsigned int mask = 0xffffffff;
    if (width<8) mask = ((1 << width*4)-1);
    if (blank)
        for (int i=0;i<width;i++) out << " ";
    else
        out << std::hex << SETW(width) << std::setfill('0') << ((data>>shift) & mask);
}

//copied from oncsSubevent.cc for a generic dump 12/21/17
void oncsSub_idmvtxv0::gdump(const int i, OSTREAM& out) const
{

  int *SubeventData = &SubeventHdr->data;
  //int dword_to_print;
  int j,l;
  identify(out);

  int current_offset;
  const int DWORDS_PER_WORD = 8;
  
  switch (i)
    {
    case (EVT_HEXADECIMAL):
      //j = 0;
      current_offset = 0;
      while (1)
	{
	  out << SETW(5) << current_offset << " |  ";
	  //for (l=0;l<DWORDS_PER_WORD;l++)
	  
          //FELIX header
          print_stuff(out, SubeventData[current_offset+7], 4, 16, current_offset+7>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);

          //RU word 2
          print_stuff(out, SubeventData[current_offset+7], 4, 0, current_offset+7>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          print_stuff(out, SubeventData[current_offset+6], 8, 0, current_offset+6>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          print_stuff(out, SubeventData[current_offset+5], 8, 0, current_offset+5>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          out << " ";

          //RU word 1
          print_stuff(out, SubeventData[current_offset+4], 8, 0, current_offset+4>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          print_stuff(out, SubeventData[current_offset+3], 8, 0, current_offset+3>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          print_stuff(out, SubeventData[current_offset+2], 4, 16, current_offset+2>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          out << " ";

          //RU word 0
          print_stuff(out, SubeventData[current_offset+2], 4, 0, current_offset+2>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          print_stuff(out, SubeventData[current_offset+1], 8, 0, current_offset+1>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          print_stuff(out, SubeventData[current_offset+0], 8, 0, current_offset+0>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4);
          out << " " << std::dec << std::endl << std::setfill(' ');

          int dwords_remaining = SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4 - current_offset;
	  //if (current_offset>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) break;
	  //if (current_offset>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding/4) break; //hack to deal with our incorrect padding in daq_device_felix.cc
          if (dwords_remaining<8) break;

          current_offset += DWORDS_PER_WORD;
	}
      break;

    case (EVT_DECIMAL):
      j = 0;
      while (1)
	{
	  out << std::dec << std::endl << SETW(5) << j << " |  ";

	  for (l=0;l<6;l++)
	    {
	      out << SETW(10) << SubeventData[j++] << " ";
	      if (j>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) break;
	    }
	  if (j>=SubeventHdr->sub_length-SEVTHEADERLENGTH - SubeventHdr->sub_padding) break;
	}
      break;

    default: 
      break;
    }
  out << std::endl;

}

