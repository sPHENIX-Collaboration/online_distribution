#include "oncsSub_idmvtxv1.h"
#include <cstring>
#include <bitset>
#include <map>
#include <vector>

#include <arpa/inet.h>

using namespace std;

oncsSub_idmvtxv1::oncsSub_idmvtxv1(subevtdata_ptr data)
    :oncsSubevent_w4 (data)
{
    _is_decoded = 0;
    _highest_ruid = -1;
    for (int ruid=0; ruid<IDMVTXV1_MAXRUID+1; ruid++)
    {
        _bad_ruchns[ruid] = 0;
        _lanes_active[ruid] = -1;
        _lane_stops[ruid] = -1;
        _lane_timeouts[ruid] = -1;
        for ( int ruchn = 0; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
        {
            _chip_id[ruid][ruchn] = -1;
            _bad_bytes[ruid][ruchn] = 0;
            _excess_bytes[ruid][ruchn] = 0;
            _bunchcounter[ruid][ruchn] = -1;
            _header_found[ruid][ruchn] = false;
            _trailer_found[ruid][ruchn] = false;
            _readout_flags[ruid][ruchn] = -1;
        }
    }
    _unexpected_felix_counters = 0;
    _bad_ruids = 0;
}

oncsSub_idmvtxv1::~oncsSub_idmvtxv1()
{

}


typedef struct 
{
    unsigned char d0[3][10];
    unsigned char counter;
    unsigned char ruid;
}  data32;

#define CHIPHEADER     1
#define CHIPEMPTYFRAME 2
#define DATASHORT      3
#define DATALONG0      4
#define DATALONG1      5

int oncsSub_idmvtxv1::decode_thebit(int the_row, int encoder_id, int address) const
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

void oncsSub_idmvtxv1::print_stuff(OSTREAM& out, unsigned int data, int width, int shift, bool blank) const
{
    unsigned int mask = 0xffffffff;
    if (width<8) mask = ((1 << width*4)-1);
    if (blank)
        for (int i=0;i<width;i++) out << " ";
    else
        out << std::hex << SETW(width) << std::setfill('0') << ((data>>shift) & mask);
}

int oncsSub_idmvtxv1::encode_hit(unsigned short row, unsigned short col) const
{
    return (row << 16) + col;
}

unsigned short oncsSub_idmvtxv1::decode_row(int hit) const
{
    return hit >> 16;
}

unsigned short oncsSub_idmvtxv1::decode_col(int hit) const
{
    return hit & 0xffff;
}

bool oncsSub_idmvtxv1::mask_contains_ruchn ( int mask, int ruchn )
{
    if (ruchn<=0) return false; //invalid ruchn
    if (mask<0) return false;
    return (((mask >> (ruchn-1)) & 1) == 1);
}

int *oncsSub_idmvtxv1::decode ()
{
    if ( _is_decoded) return 0;
    _is_decoded = 1;

    unsigned int *payload = (unsigned int *) &SubeventHdr->data;  // here begins the payload

    int dlength = getDataLength() - getPadding() - 1; //padding is supposed to be in units of dwords, this assumes dwords
    unsigned char *the_end = ( unsigned char *) &payload[dlength];

    unsigned char *pos = (unsigned char *) payload;

    unsigned char b;

    vector<unsigned char> ruchn_stream[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];

    unsigned char felix_counter [IDMVTXV1_MAXRUID+1];

    for( int i=0; i < IDMVTXV1_MAXRUID+1; i++)
    {
        felix_counter[i] = 0;
    }

    while ( pos < the_end )
    {

        data32 *d32 = ( data32*) pos; 

        //  cout << hex << " pos = " << (unsigned long long ) pos << "  the end  " << (unsigned long long) the_end << dec << endl;
        if (d32->ruid > IDMVTXV1_MAXRUID)
        {
            cout << __FILE__ << " " << __LINE__ << " --- invalid ruid " << hex << (int) d32->ruid << " at pos " << (long) pos << dec << endl;
            _bad_ruids++;
            break;
        }
        else if (d32->ruid > _highest_ruid)
        {
            _highest_ruid = d32->ruid;
        }
        //FELIX counter is 8 bits, max value 256
        //check that the new counter value is consistent with the previous value (it should have increased by no more than 3, and the amount of the difference is the number of data chunks in this FELIX word)
        int counter_increment = (d32->counter + 256 - felix_counter[d32->ruid])%256;
        if (counter_increment > 3)
        {
            if (d32->counter != 0)
            {
                //cout << __FILE__ << " " << __LINE__ << " --- unexpected FELIX counter " << hex << (int) d32->counter << " following previous counter value " << felix_counter << " at pos " << (long) pos << dec << endl;
                _unexpected_felix_counters++;
            }
            break;
        }
        felix_counter[d32->ruid] = d32->counter;

        //for (int ichnk = 0; ichnk < 3; ichnk++)
        for (int ichnk = 0; ichnk < counter_increment; ichnk++)
        {
            unsigned char ruchn = d32->d0[ichnk][9];
            if (ruchn == IDMVTXV1_RUHEADER)
            {
                memcpy(&_lanes_active[d32->ruid],&d32->d0[ichnk][2],4);
            }
            else if (ruchn == IDMVTXV1_RUTRAILER)
            {
                memcpy(&_lane_stops[d32->ruid],&d32->d0[ichnk][0],4);
                memcpy(&_lane_timeouts[d32->ruid],&d32->d0[ichnk][4],4);
            }
            else if (ruchn > IDMVTXV1_MAXRUCHN)
            {
                _bad_ruchns[d32->ruid]++;
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
                ruchn_stream[d32->ruid][ruchn].push_back(d32->d0[ichnk][ibyte]);
            }
        } // ichnk

        pos += sizeof(*d32);
    } // while (pos < the_end)

    //we ignore ruchn 0 (trigger information and padding words) for now
    /*
       if (ruchn_stream[0].size()!=9) for ( int ruchn = 0; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
       {
       cout << __FILE__ << " " << __LINE__ << " --- ruchn " << ruchn << " has " << ruchn_stream[ruchn].size() << " bytes" << endl;
       }
       */

    for ( int ruid = 1; ruid < IDMVTXV1_MAXRUID+1; ruid++)
    {
        for ( int ruchn = 1; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
        {

            bool header_seen=false;
            int status = 0;
            int ibyte_endofdata = -1;
            int the_region = -1;
            unsigned int address = 0;
            unsigned int encoder_id = 0;
            for (unsigned int ibyte = 0; ibyte < ruchn_stream[ruid][ruchn].size(); ibyte++)
            {
                b = ruchn_stream[ruid][ruchn].at(ibyte);

                //cout << __FILE__ << " " << __LINE__ << " --- ruchn " << hex << ruchn << " next value " << (unsigned int)  b << dec << " status:" << status << endl;

                if (status) // we mop up what we started in the last round -
                    // these are all cases with more than one byte
                {
                    switch (status)
                    {
                        case CHIPHEADER:

                            _bunchcounter[ruid][ruchn] = b;
                            //cout << __FILE__ << " " << __LINE__ << " chip header, ruchn= " << ruchn << " bunchctr= " << hex << _bunchcounter[ruchn] << dec << endl;
                            status = 0;
                            _header_found[ruid][ruchn] = true;
                            break;

                        case CHIPEMPTYFRAME:

                            _bunchcounter[ruid][ruchn] = b;
                            // cout << __FILE__ << " " << __LINE__ << " chip empty frame " << hex << ruchn << " " << _bunchcounter[ruchn] << dec << endl;
                            ibyte_endofdata = ibyte;
                            _header_found[ruid][ruchn] = true;
                            _trailer_found[ruid][ruchn] = true;
                            status = 0;
                            break;

                        case DATASHORT:
                            address += b;	
                            //cout << __FILE__ << " " << __LINE__ << " data short report, hex:" << hex << address << dec << " enc. id " << encoder_id << " address= " << address << " the_region:" << the_region << " ruchn:" << ruchn;
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
                                    unsigned short col = the_region*32 + thebit;
                                    _hit_vectors[ruid][ruchn].push_back(encode_hit(the_row,col));
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
                                _bad_bytes[ruid][ruchn]++;
                            }
                            //cout << __FILE__ << " " << __LINE__ << " data long report, hex:" << hex << address << dec << " enc. id " << encoder_id << " address= " << address << " the_region:" << the_region << " ruchn:" << ruchn << endl;
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
                                            unsigned short col = the_region*32 + thebit;
                                            _hit_vectors[ruid][ruchn].push_back(encode_hit(the_row,col));
                                        }
                                    }
                                }
                            }
                            status = 0;
                            break;
                    }
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
                    _chip_id[ruid][ruchn] = ( b & 0xf);
                    header_seen = true;
                    status = CHIPHEADER;
                }

                else if ( ( b >> 4) == 0xb) // we have a chip trailer
                {
                    // cout << __FILE__ << " " << __LINE__ << " chip trailer, ruchn= " << hex << ruchn << dec << endl;
                    // break out of the loop, done with this chip
                    ibyte_endofdata = ibyte;
                    _trailer_found[ruid][ruchn] = true;
                    if (!_header_found[ruid][ruchn]) // shouldn't see a trailer without having seen a header
                    {
                        _bad_bytes[ruid][ruchn]++;
                    }
                    else
                    {
                        _readout_flags[ruid][ruchn] = (b & 0xf);
                    }
                    break;
                }

                else if ( ( b >> 4) == 0xE) // we have a chip empty frame
                {
                    _chip_id[ruid][ruchn] = ( b & 0xf);
                    header_seen = true;
                    status = CHIPEMPTYFRAME;
                }

                else if ( ( b >> 5) == 0x6) // we have a region header
                {
                    unsigned int region_id = (b & 0x1f);
                    if ( region_id <32)
                    {
                        the_region = region_id;
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
                    _bad_bytes[ruid][ruchn]++;
                }
                if (ibyte==0 && !header_seen) break;//first byte of the ALPIDE stream must be a chip header or chip empty; if not, abort so we don't get confused by bad data

            } // ibyte

            //cout << __FILE__ << " " << __LINE__ << "  ruchn " << ruchn << "  ibyte_endofdata " << ibyte_endofdata << endl;
            for (unsigned int ibyte = ibyte_endofdata+1; ibyte < ruchn_stream[ruid][ruchn].size(); ibyte++)
            {
                b = ruchn_stream[ruid][ruchn].at(ibyte);
                if (b!=0)
                {
                    //cout << __FILE__ << " " << __LINE__ << " --- ruchn " << hex << ruchn << " unexpected nonzero value " << (unsigned int)  b << dec << " at ibyte " << ibyte << " after ibyte_endofdata " << ibyte_endofdata << endl;
                    _excess_bytes[ruid][ruchn]++;
                }
            }
        } // ruchn
    } // ruid


    return 0;
}

int oncsSub_idmvtxv1::iValue(const int ruid, const char *what)
{
    decode();

    if ( strcmp(what,"UNEXPECTED_FELIX_COUNTERS") == 0 )
    {
        return _unexpected_felix_counters;
    }

    else if ( strcmp(what,"BAD_RUIDS") == 0 )
    {
        return _bad_ruids;
    }

    if (ruid > _highest_ruid) return -1; // no such RU
    if (_lanes_active[ruid]==-1) return -1; // no such RU

    if ( strcmp(what,"BAD_RUCHNS") == 0 )
    {
        return _bad_ruchns[ruid];
    }

    else if ( strcmp(what,"LANE_STOPS") == 0 )
    {
        return _lane_stops[ruid];
    }

    else if ( strcmp(what,"LANE_TIMEOUTS") == 0 )
    {
        return _lane_timeouts[ruid];
    }

    return 0;
}

int oncsSub_idmvtxv1::iValue(const int ruid)
{
    decode();

    if (ruid > _highest_ruid) return -1; // no such RU
    return _lanes_active[ruid];
}

int oncsSub_idmvtxv1::iValue(const int ruid, const int ruchn, const char *what)
{
    decode();

    if (ruid > _highest_ruid) return -1; // no such RU
    if (_lanes_active[ruid]==-1) return -1; // no such RU

    if ( strcmp(what,"CHIP_ID") == 0 )
    {
        return _chip_id[ruid][ruchn];
    }

    else if ( strcmp(what,"BAD_BYTES") == 0 )
    {
        return _bad_bytes[ruid][ruchn];
    }

    else if ( strcmp(what,"EXCESS_BYTES") == 0 )
    {
        return _excess_bytes[ruid][ruchn];
    }

    else if ( strcmp(what,"BUNCHCOUNTER") == 0 )
    {
        return _bunchcounter[ruid][ruchn];
    }

    else if ( strcmp(what,"HEADER_FOUND") == 0 )
    {
        return _header_found[ruid][ruchn]?1:0;
    }

    else if ( strcmp(what,"TRAILER_FOUND") == 0 )
    {
        return _trailer_found[ruid][ruchn]?1:0;
    }

    else if ( strcmp(what,"READOUT_FLAGS") == 0 )
    {
        return _readout_flags[ruid][ruchn];
    }

    return 0;
}

int oncsSub_idmvtxv1::iValue(const int ruid, const int ruchn)
{
    decode();

    if (ruid > _highest_ruid) return -1; // no such RU
    if (_lanes_active[ruid]==-1) return -1; // no such RU
    if (!mask_contains_ruchn(_lanes_active[ruid],ruchn)) return -1; // no such RU channel
    return _hit_vectors[ruid][ruchn].size();
}

int oncsSub_idmvtxv1::iValue(const int ruid, const int ruchn, const int i)
{
    decode();

    if (ruid > _highest_ruid) return -1; // no such RU
    if (_lanes_active[ruid]==-1) return -1; // no such RU
    if (!mask_contains_ruchn(_lanes_active[ruid],ruchn)) return -1; // no such RU channel
    return _hit_vectors[ruid][ruchn].at(i);
}

void  oncsSub_idmvtxv1::dump ( OSTREAM& os )
{

    identify(os);

    decode();
    bool first;
    for (int ruid=0; ruid<IDMVTXV1_MAXRUID+1; ruid++)
    {
        if (iValue(ruid)!=-1)
        {
            os << "RU ID: " << ruid;
            os << ", bad_ruchns=" << iValue(ruid,"BAD_RUCHNS");
            os << hex << setfill('0');
            os << ", lanes_active 0x" << setw(7) << iValue(ruid);
            os << dec;
            first=true;
            os << " (";
            for ( int ruchn = 0; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
            {
                if (mask_contains_ruchn(iValue(ruid),ruchn))
                {
                    if (!first)
                        os << ", ";
                    os << ruchn;
                    first = false;
                }
            }
            os << hex << setfill('0');
            os << "), lane_stops=0x" << setw(7) << iValue(ruid,"LANE_STOPS");
            os << dec;
            first=true;
            os << " (";
            for ( int ruchn = 0; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
            {
                if (mask_contains_ruchn(iValue(ruid,"LANE_STOPS"),ruchn))
                {
                    if (!first)
                        os << ", ";
                    os << ruchn;
                    first = false;
                }
            }
            os << hex << setfill('0');
            os << "), lane_timeouts=0x" << setw(7) << iValue(ruid,"LANE_TIMEOUTS");
            os << dec;
            first=true;
            os << " (";
            for ( int ruchn = 0; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
            {
                if (mask_contains_ruchn(iValue(ruid,"LANE_TIMEOUTS"),ruchn))
                {
                    if (!first)
                        os << ", ";
                    os << ruchn;
                    first = false;
                }
            }
            os << ")";
            os << dec << setfill(' ') << endl;
            for ( int ruchn = 0; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
            {
                if (iValue(ruid,ruchn)!=-1)
                {
                    os << "RU channel " << setw(2) << ruchn;
                    os << " bad_bytes=" << iValue(ruid,ruchn,"BAD_BYTES") << " excess_bytes=" << iValue(ruid,ruchn,"EXCESS_BYTES") <<" header_found=" << iValue(ruid,ruchn,"HEADER_FOUND") << " trailer_found=" << iValue(ruid,ruchn,"TRAILER_FOUND");
                    os << " bunchcounter=" << setw(3) << iValue(ruid,ruchn,"BUNCHCOUNTER") << " flags=" << iValue(ruid,ruchn,"READOUT_FLAGS");
                    os << ", chip ID " << iValue(ruid,ruchn,"CHIP_ID") << ", " << iValue(ruid,ruchn) << " hits: ";
                    for (int i=0;i<iValue(ruid,ruchn);i++)
                    {
                        int hit = iValue(ruid,ruchn,i);
                        os << "(row " << decode_row(hit) << ", col " << decode_col(hit) << ") ";
                    }
                    os << endl;
                }
            }
        }
    }
}

//copied from oncsSubevent.cc for a generic dump 12/21/17
void oncsSub_idmvtxv1::gdump(const int i, OSTREAM& out) const
{

    int *SubeventData = &SubeventHdr->data;
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
                //int dwords_remaining = getLength()-SEVTHEADERLENGTH - getPadding()/4 - current_offset; //padding is supposed to be in units of dwords, this assumes bytes
                int dwords_remaining = getLength()-SEVTHEADERLENGTH - getPadding() - current_offset; //padding is supposed to be in units of dwords, this assumes dwords

                out << SETW(5) << current_offset << " |  ";
                //for (l=0;l<DWORDS_PER_WORD;l++)

                //FELIX header
                print_stuff(out, SubeventData[current_offset+7], 4, 16, (dwords_remaining<=7));
                out << " ";

                //RU word 2
                print_stuff(out, SubeventData[current_offset+7], 4, 0, (dwords_remaining<=7));
                print_stuff(out, SubeventData[current_offset+6], 8, 0, (dwords_remaining<=6));
                print_stuff(out, SubeventData[current_offset+5], 8, 0, (dwords_remaining<=5));
                out << " ";

                //RU word 1
                print_stuff(out, SubeventData[current_offset+4], 8, 0, (dwords_remaining<=4));
                print_stuff(out, SubeventData[current_offset+3], 8, 0, (dwords_remaining<=3));
                print_stuff(out, SubeventData[current_offset+2], 4, 16, (dwords_remaining<=2));
                out << " ";

                //RU word 0
                print_stuff(out, SubeventData[current_offset+2], 4, 0, (dwords_remaining<=2));
                print_stuff(out, SubeventData[current_offset+1], 8, 0, (dwords_remaining<=1));
                print_stuff(out, SubeventData[current_offset+0], 8, 0, (dwords_remaining<=0));
                out << " " << std::dec << std::endl << std::setfill(' ');

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

