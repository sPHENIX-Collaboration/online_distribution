#ifndef __ONCSSUB_IDMVTXV2_H__
#define __ONCSSUB_IDMVTXV2_H__

#include "oncsSubevent.h"
#include <vector>

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idmvtxv2 : public  oncsSubevent_w4 {
#else
    class  oncsSub_idmvtxv2 : public  oncsSubevent_w4 {
#endif

#define IDMVTXV2_MAXRUID       8
#define IDMVTXV2_RUHEADER      0xE0
#define IDMVTXV2_RUTRAILER     0xF0
#define IDMVTXV2_MAXRUCHN      9

#define NROW  512
#define NCOL 1024

#define CHIPHEADER     1
#define CHIPEMPTYFRAME 2
#define DATASHORT      3
#define DATALONG0      4
#define DATALONG1      5

        public:
            oncsSub_idmvtxv2( subevtdata_ptr);
            ~oncsSub_idmvtxv2();

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
            bool checkBC(const int ruid);

            int _is_decoded;

            int _highest_ruid;
            int _decoder_error;

            std::vector<int> _hit_vectors[IDMVTXV2_MAXRUID+1][IDMVTXV2_MAXRUCHN+1];

            int _unexpected_felix_counters;
            int _bad_ruids;
            int _bad_ruchns[IDMVTXV2_MAXRUID+1];
            int _lanes_active[IDMVTXV2_MAXRUID+1];
            int _lane_stops[IDMVTXV2_MAXRUID+1];
            int _lane_timeouts[IDMVTXV2_MAXRUID+1];
            int _inconsistent_bc[IDMVTXV2_MAXRUID+1];
            int _chip_id[IDMVTXV2_MAXRUID+1][IDMVTXV2_MAXRUCHN+1];
            int _bad_bytes[IDMVTXV2_MAXRUID+1][IDMVTXV2_MAXRUCHN+1];
            int _excess_bytes[IDMVTXV2_MAXRUID+1][IDMVTXV2_MAXRUCHN+1];
            int _bunchcounter[IDMVTXV2_MAXRUID+1][IDMVTXV2_MAXRUCHN+1];
            bool _header_found[IDMVTXV2_MAXRUID+1][IDMVTXV2_MAXRUCHN+1];
            bool _trailer_found[IDMVTXV2_MAXRUID+1][IDMVTXV2_MAXRUCHN+1];
            int _readout_flags[IDMVTXV2_MAXRUID+1][IDMVTXV2_MAXRUCHN+1];

            void print_stuff(OSTREAM& out, unsigned int data, int width, int shift, bool blank = false) const;
    };



#endif /* __ONCSSUB_IDMVTXV2_H__ */
