#include "oncsSub_idmvtxv3.h"

#ifdef MVTX_DECODER
#include "MVTXDecoder/ChipMappingMVTX.h"
#include "MVTXDecoder/RawPixelReader.h"
//#include "MVTXDecoder/mvtx_decoder.h"
#include "MVTXDecoder/FLXWord.h"
#endif

#include <cassert>  // for assert

//#include <cstring>
//#include <bitset>
//#include <map>
//#include <vector>
//#include <functional>
//
//#include <arpa/inet.h>
//

//
//extern void mvtx_decoder(std::vector<oncsSub_idmvtxv3::data32_mvtx*>);

oncsSub_idmvtxv3::oncsSub_idmvtxv3(subevtdata_ptr data)
  : oncsSubevent_w4(data)
  , m_is_decoded(false)
{
//    _highest_ruid = -1;
//    _decoder_error = 0;
//    for (int ruid=0; ruid<IDMVTXV3_MAXRUID+1; ruid++)
//    {
//        _bad_ruchns[ruid] = 0;
//        _lanes_active[ruid] = -1;
//        _lane_stops[ruid] = -1;
//        _lane_timeouts[ruid] = -1;
//        _inconsistent_bc[ruid] = -1;
//        for ( int ruchn = 0; ruchn < IDMVTXV3_MAXRUCHN+1; ruchn++)
//        {
//            _chip_id[ruid][ruchn] = -1;
//            _bad_bytes[ruid][ruchn] = 0;
//            _excess_bytes[ruid][ruchn] = 0;
//            _bunchcounter[ruid][ruchn] = -1;
//            _header_found[ruid][ruchn] = false;
//            _trailer_found[ruid][ruchn] = false;
//            _readout_flags[ruid][ruchn] = -1;
//        }
//    }
//    _unexpected_felix_counters = 0;
//    _bad_ruids = 0;
}


//bool oncsSub_idmvtxv3::mask_contains_ruchn ( int mask, int ruchn )
//{
//    if (ruchn<=0) return false; //invalid ruchn
//    if (mask<0) return false;
//    return (((mask >> (ruchn-1)) & 1) == 1);
//}
//
//int *oncsSub_idmvtxv3::decode_old ()
//{
//    if ( _is_decoded) return 0;
//    _is_decoded = 1;
//    _decoder_error = 0;
//
//    unsigned int *payload = (unsigned int *) &SubeventHdr->data;  // here begins the payload
//
//    int dlength = getDataLength() - getPadding() - 1; //padding is supposed to be in units of dwords, this assumes dwords
//    unsigned char *the_end = ( unsigned char *) &payload[dlength];
//
//    unsigned char *pos = (unsigned char *) payload;
//
//    unsigned char b;
//
//    vector<unsigned char> ruchn_stream[IDMVTXV3_MAXRUID+1][IDMVTXV3_MAXRUCHN+1];
//
//    unsigned char felix_counter [IDMVTXV3_MAXRUID+1];
//
//    for( int i=0; i < IDMVTXV3_MAXRUID+1; i++)
//    {
//        felix_counter[i] = 0;
//    }
//
//    while ( pos < the_end )
//    {
//
//        data32 *d32 = ( data32*) pos;
//
//        //cout << hex << " pos = " << (unsigned long long ) pos << "  the end  " << (unsigned long long) the_end << dec << endl;
//        if (d32->ruid > IDMVTXV3_MAXRUID)
//        {
//            cout << __FILE__ << " " << __LINE__ << " --- invalid ruid " << hex << (int) d32->ruid << " at pos " << (long) pos << dec << endl;
//            _bad_ruids++;
//            _decoder_error |= ( 1 << 0 );
//            break;
//        }
//        else if (d32->ruid > _highest_ruid)
//        {
//            _highest_ruid = d32->ruid;
//        }
//        //FELIX counter is 8 bits, max value 256
//        //check that the new counter value is consistent with the previous value (it should have increased by no more than 3, and the amount of the difference is the number of data chunks in this FELIX word)
//        int counter_increment = (d32->counter + 256 - felix_counter[d32->ruid])%256;
//        if (counter_increment > 3)
//        {
//            if (d32->counter != 0)
//            {
//                //cout << __FILE__ << " " << __LINE__ << " --- unexpected FELIX counter " << hex << (int) d32->counter << " following previous counter value " << felix_counter << " at pos " << (long) pos << dec << endl;
//                _unexpected_felix_counters++;
//            }
//            break;
//        }
//        felix_counter[d32->ruid] = d32->counter;
//
//        //for (int ichnk = 0; ichnk < 3; ichnk++)
//        for (int ichnk = 0; ichnk < counter_increment; ichnk++)
//        {
//            unsigned char ruchn = d32->d0[ichnk][9];
//            if (ruchn >> 4 == 0x2) {
//              ruchn &= 0x1F;
//              ruchn += 1;
//            }
//            if (ruchn == IDMVTXV3_RUHEADER)
//            {
//                memcpy(&_lanes_active[d32->ruid],&d32->d0[ichnk][2],4);
//            }
//            else if (ruchn == IDMVTXV3_RUTRAILER)
//            {
//                memcpy(&_lane_stops[d32->ruid],&d32->d0[ichnk][0],4);
//                memcpy(&_lane_timeouts[d32->ruid],&d32->d0[ichnk][4],4);
//            }
//            else if (ruchn > IDMVTXV3_MAXRUCHN)
//            {
//                _bad_ruchns[d32->ruid]++;
//                cout << __FILE__ << " " << __LINE__ << " --- invalid ruchn " << hex << ruchn << ", full RU word: ";
//                _decoder_error |= ( 1 << 1 );
//                for (int ibyte=9;ibyte>=0;ibyte--)
//                {
//                    cout << setfill('0') << setw(2) << hex << (unsigned int) d32->d0[ichnk][ibyte] << " ";
//                }
//                cout << setfill(' ') << setw(0) << dec << endl;
//            }
//            else for ( int ibyte = 0; ibyte < 9; ibyte++)
//            {
//                //cout << __FILE__ << " " << __LINE__ << hex << " --- ruchn " << (unsigned int) ruchn << " byte " << (unsigned int) d32->d0[ichnk][ibyte] << dec << endl;
//                ruchn_stream[d32->ruid][ruchn].push_back(d32->d0[ichnk][ibyte]);
//            }
//        } // ichnk
//
//        pos += sizeof(*d32);
//    } // while (pos < the_end)
//
//    //we ignore ruchn 0 (trigger information and padding words) for now
//    /*
//       if (ruchn_stream[0].size()!=9) for ( int ruchn = 0; ruchn < IDMVTXV3_MAXRUCHN+1; ruchn++)
//       {
//       cout << __FILE__ << " " << __LINE__ << " --- ruchn " << ruchn << " has " << ruchn_stream[ruchn].size() << " bytes" << endl;
//       }
//       */
//
//    for ( int ruid = 1; ruid < IDMVTXV3_MAXRUID+1; ruid++)
//    {
//        for ( int ruchn = 1; ruchn < IDMVTXV3_MAXRUCHN+1; ruchn++)
//        {
//
//            bool header_seen=false;
//            int status = 0;
//            int ibyte_endofdata = -1;
//            int the_region = -1;
//            unsigned int address = 0;
//            unsigned int encoder_id = 0;
//            for (unsigned int ibyte = 0; ibyte < ruchn_stream[ruid][ruchn].size(); ibyte++)
//            {
//                b = ruchn_stream[ruid][ruchn].at(ibyte);
//
//                //cout << __FILE__ << " " << __LINE__ << " --- ruchn " << hex << ruchn << " next value " << (unsigned int)  b << dec << " status:" << status << endl;
//
//                if (status) // we mop up what we started in the last round -
//                    // these are all cases with more than one byte
//                {
//                    switch (status)
//                    {
//                        case CHIPHEADER:
//
//                            _bunchcounter[ruid][ruchn] = b;
//                            //cout << __FILE__ << " " << __LINE__ << " chip header, ruchn= " << ruchn << " bunchctr= " << hex << _bunchcounter[ruchn] << dec << endl;
//                            status = 0;
//                            _header_found[ruid][ruchn] = true;
//                            break;
//
//                        case CHIPEMPTYFRAME:
//
//                            _bunchcounter[ruid][ruchn] = b;
//                            // cout << __FILE__ << " " << __LINE__ << " chip empty frame " << hex << ruchn << " " << _bunchcounter[ruchn] << dec << endl;
//                            ibyte_endofdata = ibyte;
//                            _header_found[ruid][ruchn] = true;
//                            _trailer_found[ruid][ruchn] = true;
//                            status = 0;
//                            break;
//
//                        case DATASHORT:
//                            address += b;
//                            //cout << __FILE__ << " " << __LINE__ << " data short report, hex:" << hex << address << dec << " enc. id " << encoder_id << " address= " << address << " the_region:" << the_region << " ruchn:" << ruchn;
//                            // the numbering "snakes" its way through a column (fig. 4.5 in the Alpide manual)
//                            //  0 1  > >    row 0
//                            //  3 2  < <    row 1
//                            //  4 5  > >    row 2
//                            //  7 6  < <   so we need to know if we are in an even or odd row
//                            if ( the_region >= 0 && encoder_id >=0 )
//                            {
//                                unsigned int row = (address >> 1);
//                                unsigned int col = (the_region * 32);
//                                col += (encoder_id * 2) + ( (row & 1) ? ( (address & 1) ^ 1 ) : (address & 1) );
//                                if ( row < 0 || row >= NROW || col < 0 || col >= NCOL )
//                                {
//                                    cout << __FILE__ << " " << __LINE__ << " impossible pixel coord: " << row
//                                         << ", " << col << "." << endl;
//                                    cout << "Check region " << the_region << ", encoder " <<  encoder_id
//                                         << ", addr " << address << endl;
//                                    _decoder_error |= ( 1 << 2);
//                                }
//                                _hit_vectors[ruid][ruchn].push_back(encode_hit(row,col));
//                            }
//
//                            //cout << endl;
//                            status = 0;
//                            break;
//
//                        case DATALONG0:
//                            address += b;
//                            status = DATALONG1;
//                            break;
//
//                        case DATALONG1:
//                            if ( (b & 0x80) != 0) //required to be 0
//                            {
//                                //cout << __FILE__ << " " << __LINE__ << " unexpected word " << hex << (unsigned int) b << dec << " at ibyte " << ibyte << endl;
//                                _bad_bytes[ruid][ruchn]++;
//                            }
//                            //cout << __FILE__ << " " << __LINE__ << " data long report, hex:" << hex << address << dec << " enc. id " << encoder_id << " address= " << address << " the_region:" << the_region << " ruchn:" << ruchn << endl;
//                            // Loop over the hits in the cluster. The pixel specified by the address always has a hit.
//                            // The next 7 pixels (in priority encoder order) have hits if the corresponding bit in this byte is high.
//                            // See ALPIDE manual section 3.4.1 on DATA LONG: page 63.
//                            for (int ihit = 0; ihit<8; ihit++)
//                            {
//                                if (ihit==0 || ((b >> (ihit-1)) & 1))
//                                {
//                                    int hit_address = address + ihit;
//                                    if ( the_region >= 0 && encoder_id >=0 )
//                                    {
//                                        unsigned int row = (hit_address >> 1);
//                                        unsigned int col = the_region*32;
//                                        col += (encoder_id * 2) + ( (row & 1) ? ( (hit_address & 1) ^ 1 ) : (hit_address & 1) );
//                                        if ( row < 0 || row >= NROW || col < 0 || col >= NCOL )
//                                        {
//                                            cout << __FILE__ << " " << __LINE__ << " impossible pixel coord: " << row
//                                                  << ", " << col << "." << endl;
//                                            cout << "Check region " << the_region << ", encoder " <<  encoder_id
//                                                  << ", addr " << hit_address << endl;
//                                            _decoder_error |= ( 1 << 2);
//                                        }
//                                        _hit_vectors[ruid][ruchn].push_back(encode_hit(row,col));
//                                    }
//                                }
//                            }
//                            status = 0;
//                            break;
//                    }
//                    continue;
//                }
//
//                if (ibyte_endofdata != -1) break;
//
//                if ( b == 0xff)  // Idle byte, skip
//                {
//                    //cout << __FILE__ << " " << __LINE__ << " IDLE byte " << hex << (unsigned int)  b << dec << endl;
//                    status = 0;
//                }
//
//                else if ( ( b >> 4) == 0xa) // we have a chip header
//                {
//                    _chip_id[ruid][ruchn] = ( b & 0xf);
//                    header_seen = true;
//                    status = CHIPHEADER;
//                }
//
//                else if ( ( b >> 4) == 0xb) // we have a chip trailer
//                {
//                    // cout << __FILE__ << " " << __LINE__ << " chip trailer, ruchn= " << hex << ruchn << dec << endl;
//                    // break out of the loop, done with this chip
//                    ibyte_endofdata = ibyte;
//                    _trailer_found[ruid][ruchn] = true;
//                    if (!_header_found[ruid][ruchn]) // shouldn't see a trailer without having seen a header
//                    {
//                        _bad_bytes[ruid][ruchn]++;
//                    }
//                    else
//                    {
//                        _readout_flags[ruid][ruchn] = (b & 0xf);
//                    }
//                    break;
//                }
//
//                else if ( ( b >> 4) == 0xE) // we have a chip empty frame
//                {
//                    _chip_id[ruid][ruchn] = ( b & 0xf);
//                    header_seen = true;
//                    status = CHIPEMPTYFRAME;
//                }
//
//                else if ( ( b >> 5) == 0x6) // we have a region header
//                {
//                    unsigned int region_id = (b & 0x1f);
//                    if ( region_id <32)
//                    {
//                        the_region = region_id;
//                    }
//                    else //this is impossible since the region ID is 5 bits
//                    {
//                        cout << __FILE__ << " " << __LINE__ << " wrong region header, id=  " << hex << region_id << dec << endl;
//                        _decoder_error |= ( 1 << 3);
//                    }
//                }
//
//                else if ( ( b >> 6) == 0x1) // we have a DATA short report
//                {
//                    encoder_id = ( b>>2) & 0xf;
//                    address = (b & 0x3) << 8;
//                    status = DATASHORT;
//                }
//
//                else if ( ( b >> 6) == 0x0) // we have a DATA long report
//                {
//                    encoder_id = ( b>>2) & 0xf;
//                    address = (b & 0x3) << 8;
//                    status = DATALONG0;
//                }
//
//                else if ( b == 0xF1) // we have a BUSY on
//                {
//                    //cout << __FILE__ << " " << __LINE__ << " Busy on "  << endl;
//                }
//
//                else if ( b == 0xF0) // we have a BUSY off
//                {
//                    //cout << __FILE__ << " " << __LINE__ << " Busy off "  << endl;
//                }
//
//                else
//                {
//                    cout << __FILE__ << " " << __LINE__ << " unexpected word " << hex << (unsigned int) b << dec << " at ibyte " << ibyte << endl;
//                    _bad_bytes[ruid][ruchn]++;
//                    _decoder_error |= ( 1 << 4 );
//                }
//
//                if (ibyte==0 && !header_seen)
//                {
//                  cout << __FILE__ << " " << __LINE__ << " " << "first byte of the ALPIDE stream must be a chip header or chip empty;" <<endl;
//                  cout << "Aborting so we don't get confused by bad data." << endl;
//                  _decoder_error |= ( 1 << 5 );
//                   break;
//                }
//
//            } // ibyte
//
//            //cout << __FILE__ << " " << __LINE__ << "  ruchn " << ruchn << "  ibyte_endofdata " << ibyte_endofdata << endl;
//            for (unsigned int ibyte = ibyte_endofdata+1; ibyte < ruchn_stream[ruid][ruchn].size(); ibyte++)
//            {
//                b = ruchn_stream[ruid][ruchn].at(ibyte);
//                if (b!=0)
//                {
//                    //cout << __FILE__ << " " << __LINE__ << " --- ruchn " << hex << ruchn << " unexpected nonzero value " << (unsigned int)  b << dec << " at ibyte " << ibyte << " after ibyte_endofdata " << ibyte_endofdata << endl;
//                    _excess_bytes[ruid][ruchn]++;
//                }
//            }
//        } // ruchn
//        if (_lanes_active[ruid] != -1)
//        {
//          _inconsistent_bc[ruid]  = this->checkBC(ruid);
//        }
//    } // ruid
//
//
//    return 0;
//}
//
//int oncsSub_idmvtxv3::iValue(const int ruid, const char *what)
//{
//    decode();
//    if ( strcmp(what, "DECODER_ERROR") == 0 )
//    {
//        return _decoder_error;
//    }
//    if ( strcmp(what,"UNEXPECTED_FELIX_COUNTERS") == 0 )
//    {
//        return _unexpected_felix_counters;
//    }
//
//    else if ( strcmp(what,"BAD_RUIDS") == 0 )
//    {
//        return _bad_ruids;
//    }
//
//    if (ruid > _highest_ruid) return -1; // no such RU
//    if (_lanes_active[ruid]==-1) return -1; // no such RU
//
//    if ( strcmp(what,"BAD_RUCHNS") == 0 )
//    {
//        return _bad_ruchns[ruid];
//    }
//
//    else if ( strcmp(what,"LANE_STOPS") == 0 )
//    {
//        return _lane_stops[ruid];
//    }
//
//    else if ( strcmp(what,"LANE_TIMEOUTS") == 0 )
//    {
//        return _lane_timeouts[ruid];
//    }
//
//    else if ( strcmp(what,"CHECK_BC") == 0 )
//    {
//      return _inconsistent_bc[ruid];
//    }
//
//    return 0;
//}
//
//int oncsSub_idmvtxv3::iValue(const int ruid)
//{
//    decode();
//
//    if (ruid > _highest_ruid) return -1; // no such RU
//    return _lanes_active[ruid];
//}
//
//int oncsSub_idmvtxv3::iValue(const int ruid, const int ruchn, const char *what)
//{
//    decode();
//
//    if (ruid > _highest_ruid) return -1; // no such RU
//    if (_lanes_active[ruid]==-1) return -1; // no such RU
//
//    if ( strcmp(what,"CHIP_ID") == 0 )
//    {
//        return _chip_id[ruid][ruchn];
//    }
//
//    else if ( strcmp(what,"BAD_BYTES") == 0 )
//    {
//        return _bad_bytes[ruid][ruchn];
//    }
//
//    else if ( strcmp(what,"EXCESS_BYTES") == 0 )
//    {
//        return _excess_bytes[ruid][ruchn];
//    }
//
//    else if ( strcmp(what,"BUNCHCOUNTER") == 0 )
//    {
//        return _bunchcounter[ruid][ruchn];
//    }
//
//    else if ( strcmp(what,"HEADER_FOUND") == 0 )
//    {
//        return _header_found[ruid][ruchn]?1:0;
//    }
//
//    else if ( strcmp(what,"TRAILER_FOUND") == 0 )
//    {
//        return _trailer_found[ruid][ruchn]?1:0;
//    }
//
//    else if ( strcmp(what,"READOUT_FLAGS") == 0 )
//    {
//        return _readout_flags[ruid][ruchn];
//    }
//
//    return 0;
//}
//
//int oncsSub_idmvtxv3::iValue(const int ruid, const int ruchn)
//{
//    decode();
//
//    if (ruid > _highest_ruid) return -1; // no such RU
//    if (_lanes_active[ruid]==-1) return -1; // no such RU
//    if (!mask_contains_ruchn(_lanes_active[ruid],ruchn)) return -1; // no such RU channel
//    return _hit_vectors[ruid][ruchn].size();
//}
//
//int oncsSub_idmvtxv3::iValue(const int ruid, const int ruchn, const int i)
//{
//    decode();
//
//    if (ruid > _highest_ruid) return -1; // no such RU
//    if (_lanes_active[ruid]==-1) return -1; // no such RU
//    if (!mask_contains_ruchn(_lanes_active[ruid],ruchn)) return -1; // no such RU channel
//    return _hit_vectors[ruid][ruchn].at(i);
//}

#ifdef MVTX_DECODER

//_________________________________________________
packet_range_t oncsSub_idmvtxv3::get_packet_range() const
{
  uint8_t *payload = (uint8_t *) &SubeventHdr->data; // here begins the data
  size_t dlength = (getDataLength() - getPadding()) * 4; // length in bytes

  if ((dlength % mvtx::FLXWordLength) != 0)
  {
    dlength -= dlength % mvtx::FLXWordLength;
    COUT
      << ENDL
      << "!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!! \n"
      << "DMA packet has incomplete FLX words, only "
      << dlength << " bytes(" << (dlength / mvtx::FLXWordLength)
      << " FLX words), will be decoded. \n"
      << "!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!! \n"
      << ENDL;
  }

  uint8_t *the_end = (uint8_t *) &payload[dlength];
  uint8_t *the_pos = (uint8_t *) payload;

  return std::make_pair(the_pos, the_end);
}

//_________________________________________________
int oncsSub_idmvtxv3::decode()
{
  if (m_is_decoded)
  {
    return 0;
  }
  //packet_range_t range = get_packet_range();
  //mvtx::mvtx_decoder(range.first, range.second);
  mvtx::RawPixelReader<mvtx::ChipMappingMVTX> mRawReader;


  m_is_decoded = true;

  return 0;
}

//_________________________________________________
void oncsSub_idmvtxv3::dump(OSTREAM &os)
{
  identify(os);
  decode();
  return;
}

//_________________________________________________
void oncsSub_idmvtxv3::gdump(const int how, OSTREAM &out) const
{
  identify(out);

  packet_range_t range = get_packet_range();
  uint8_t *pos = range.first;

  switch (how)
  {
    case (EVT_HEXADECIMAL):
      mvtx::FLXWord flxWord;
      while (pos < range.second)
      {
        flxWord.copyFrom((void*)pos);
        out << SETFILL(' ') << SETW(5) << (pos - range.first) << " |  ";
        out << flxWord.asString() << " ";
        out << ENDL;
        pos += mvtx::FLXWordLength;
      }
      break;

    case (EVT_DECIMAL):
      out << ENDL
      << "!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!\n"
      << "Not implemented...\n"
      << "!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!!!!!!!!\n"
      << ENDL;
      break;

    default:
      break;
  }

  out << ENDL;
}

#endif /* MVTX_DECODER */
