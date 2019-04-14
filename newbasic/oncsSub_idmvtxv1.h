#ifndef __ONCSSUB_IDMVTXV1_H__
#define __ONCSSUB_IDMVTXV1_H__

#include "oncsSubevent.h"
#include <vector>

using namespace std;

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv1 : public  oncsSubevent_w4 {
#else
    class  oncsSub_idmvtxv1 : public  oncsSubevent_w4 {
#endif

#define MAXRUID       4
#define RUHEADER      0xE0
#define RUTRAILER     0xF0
#define MAXRUCHN      28
#define MAXCHIPID     9
        public:
            oncsSub_idmvtxv1( subevtdata_ptr);
            ~oncsSub_idmvtxv1();

            int    iValue(const int ruid, const char *what);
            int    iValue(const int ruid);
            int    iValue(const int ruid, const int ruchn, const char *what);
            int    iValue(const int ruid, const int ruchn);
            int    iValue(const int ruid, const int ruchn, const int i);

            //max 24 RUs
            //max 28 ruchn/RU (each is a chip)
            //ruid, what -> RU info
            //ruid -> chn mask
            //ruid, ruchn, what -> chip info
            //ruid, ruchn -> chip hit count
            //ruid, ruchn, i -> hit info
            //hit info: 32 bits, 1024x512 -> 9 bits row, 10 bits col

            void  dump ( OSTREAM& os = COUT) ;
            void  gdump ( const int how=EVT_HEXADECIMAL, OSTREAM& os = COUT) const; // add this to override the generic gdump

            int encode_hit(unsigned short row, unsigned short col) const;
            unsigned short decode_row(int hit) const;
            unsigned short decode_col(int hit) const;
            bool mask_contains_ruchn(int mask, int ruchn);

        protected:
            int *decode ();

            int _is_decoded;

            int _highest_ruid;

            vector<int> _hit_vectors[MAXRUID+1][MAXRUCHN+1];

            int _unexpected_felix_counters;
            int _bad_ruids;
            int _bad_ruchns[MAXRUID+1];
            int _lanes_active[MAXRUID+1];
            int _lane_stops[MAXRUID+1];
            int _lane_timeouts[MAXRUID+1];
            int _chip_id[MAXRUID+1][MAXRUCHN+1];
            int _bad_bytes[MAXRUID+1][MAXRUCHN+1];
            int _excess_bytes[MAXRUID+1][MAXRUCHN+1];
            int _bunchcounter[MAXRUID+1][MAXRUCHN+1];
            bool _header_found[MAXRUID+1][MAXRUCHN+1];
            bool _trailer_found[MAXRUID+1][MAXRUCHN+1];
            int _readout_flags[MAXRUID+1][MAXRUCHN+1];

            int decode_thebit(int the_row, int encoder_id, int address) const; //helper function to decode the column number
            void print_stuff(OSTREAM& out, unsigned int data, int width, int shift, bool blank = false) const;
    };



#endif /* __ONCSSUB_IDMVTXV1_H__ */
