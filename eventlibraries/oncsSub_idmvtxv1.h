#ifndef __ONCSSUB_IDMVTXV1_H__
#define __ONCSSUB_IDMVTXV1_H__

#include "oncsSubevent.h"
#include <vector>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv1 : public  oncsSubevent_w4 {
#else
    class  oncsSub_idmvtxv1 : public  oncsSubevent_w4 {
#endif

#define IDMVTXV1_MAXRUID       4
#define IDMVTXV1_RUHEADER      0xE0
#define IDMVTXV1_RUTRAILER     0xF0
#define IDMVTXV1_MAXRUCHN      28

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
            int *mvtx_decode ();

            int _is_decoded;

            int _highest_ruid;

            std::vector<int> _hit_vectors[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];

            int _unexpected_felix_counters;
            int _bad_ruids;
            int _bad_ruchns[IDMVTXV1_MAXRUID+1];
            int _lanes_active[IDMVTXV1_MAXRUID+1];
            int _lane_stops[IDMVTXV1_MAXRUID+1];
            int _lane_timeouts[IDMVTXV1_MAXRUID+1];
            int _chip_id[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];
            int _bad_bytes[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];
            int _excess_bytes[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];
            int _bunchcounter[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];
            bool _header_found[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];
            bool _trailer_found[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];
            int _readout_flags[IDMVTXV1_MAXRUID+1][IDMVTXV1_MAXRUCHN+1];

            int decode_thebit(int the_row, int encoder_id, int address) const; //helper function to decode the column number
            void print_stuff(OSTREAM& out, unsigned int data, int width, int shift, bool blank = false) const;
    };



#endif /* __ONCSSUB_IDMVTXV1_H__ */
