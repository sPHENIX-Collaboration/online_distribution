#ifndef __ONCSSUB_IDH2GCROC3_10G_H__
#define __ONCSSUB_IDH2GCROC3_10G_H__

#include "oncsSubevent.h"
#include <cstdint>
#include<vector>
#include<map>


#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idh2gcroc3_10G : public  oncsSubevent_w4 {
#else
class  oncsSub_idh2gcroc3_10G : public  oncsSubevent_w2 {
#endif

public:
  oncsSub_idh2gcroc3_10G( subevtdata_ptr);
  ~oncsSub_idh2gcroc3_10G();

  int    iValue(const int sample, const int ch);
  int    iValue(const int sample, const int ch, const char *what);
  int    iValue(const int ,const char * what);
  int    iValue(const int event, const int sample, const int ch);
  int    iValue(const int wf, const int sample, const int ch, const char *what);
  long long   lValue(const int wf, const int sample, const int ch);
  
  void  dump ( OSTREAM& os = COUT);  

protected:
  int decode ();
  int decode_line (uint8_t *);
  int parse_timeline ();
  unsigned int u4swapfrombyte(uint8_t* v);
  unsigned int u4frombyte(uint8_t* v);
  unsigned long long u8frombyte(uint8_t* v);
  static unsigned int crc32_frame(const uint8_t* d, const int n);

  int is_decoded;
  
  int _broken;

  unsigned int _crc_errors;      // lines whose CRC-32 did not match
  unsigned int _hamming_errors;  // lines with any of the H1 H2 H3 header flags set

  #define MAX_CHIPS 4
  unsigned int  old_trigger_in;
  unsigned int  old_trigger_out;
  uint64_t      old_timestamp;

  // this structure is the main datatset after we are done.
  // it holds all ADC values for a given timestamp.
  // once we see the timestamp move on, we declare that sample "closed"
  
  struct sample {
    unsigned int ADC[72*MAX_CHIPS];
    unsigned int tot[72*MAX_CHIPS];
    unsigned int toa[72*MAX_CHIPS];
    unsigned int error[2*MAX_CHIPS];  // H1 H2 H3 hamming error bits per half-chip
    unsigned int trigger_in ;
    unsigned int trigger_out;
    unsigned int event_counter;
    uint64_t timestamp;
    unsigned int cmn;
    unsigned int calib;
    unsigned int crc32;
    unsigned int crc_error[2*MAX_CHIPS];   // per link (2*chip + group): 1 if the line's CRC-32 did not match
  } ;

  // The lines of one sample arrive within a few clocks of each other, but the links can run a
  // sample ahead of each other now and then, so lines of neighbouring samples interleave. Samples
  // that can still receive lines stay open here, keyed by timestamp, and are handed to the
  // "waveform" timeline in timestamp order once the stream is REORDER_WINDOW clocks past them.
  std::map<uint64_t, sample *> _open;
  uint64_t _max_timestamp;
  uint64_t _closed_before;      // samples below this timestamp are closed; a line for one is "late"
  void close_samples(const uint64_t before);

  // decoding problems, per packet
  unsigned int _bad_frames;     // UDP frames without the 0xaabbccdd marker, or with a bad length
  unsigned int _bad_lines;      // lines without the 0xaa5a marker, or with chip / half out of range
  unsigned int _late_lines;     // lines that arrived after their sample was closed (dropped)
  unsigned int _resets;         // the timestamp jumped back by more than RESET_CLOCK
  
  std::vector<sample *> waveform;

  
  int _nr_channels;
  int _nr_events;

  int _nr_samples;

  // this structure is meant to parse the timeline into event boundaries.
  struct event_bounds
  {
    unsigned int first;
    unsigned int length;
  };
  
  std::vector<event_bounds *> _eventlist;
    
};


#endif /* __ONCSSUB_IDH2GCROC3_H__ */
