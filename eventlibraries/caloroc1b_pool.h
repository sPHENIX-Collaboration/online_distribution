#ifndef __CALOROC1B_POOL_H__
#define __CALOROC1B_POOL_H__

#include "ROC_pool.h"

// Pool for packet format 310 (IDCALOROC1B), same API as oncsSub_idcaloroc1b.
//
// Waveforms are found per link (a timestamp step of more than
// CONTIGUOUS_CLOCK starts a new one) and, as in the Packet, waveform wf
// means the wf-th waveform of each link, with the channels of all links
// in one address space (link = channel/18).
//
// So that this pairing by position survives links that run ahead of each
// other, a waveform is only offered once every link that has produced
// data so far has completed it. drain() offers whatever is left.

class caloroc1b_pool : public ROC_pool
{
public:
  caloroc1b_pool(const unsigned int low_mark = 200, const unsigned int high_mark = 600);
  ~caloroc1b_pool() {}

  // "CHANNELS", "NR_WF", "SAMPLESIZE", "LINKPRESENT" - as in the Packet
  int    iValue(const int n, const char *what);

  // ADC1
  int    iValue(const int wf, const int sample, const int ch);

  // "ADC1", "ADC2", "TOA", "GAIN"
  int    iValue(const int wf, const int sample, const int ch, const char *what);

  // the 48-bit global timestamp of the sample, 0 if the frame has none
  long long lValue(const int wf, const int sample, const int ch);

  void dump(std::ostream &os = std::cout);

  static const int NUM_LINKS = 12;
  static const int NUM_CHANNELS_PER_LINK = 18;
  static const int CONTIGUOUS_CLOCK = 5;

protected:

  int decode_datagram(const uint32_t *d, const unsigned int nbytes);
  void close_open();
  void update_offer();
  void drop_offer();
  bool has_pending() const;

  struct caloroc_frame
  {
    unsigned int mode;
    unsigned int evt_counter;
    unsigned int timestamp;           // the ASIC's 16b timestamp
    unsigned long long global_ts;     // 48b global timestamp, 0 if not sent

    unsigned int hitmap;
    unsigned int numhits;
    unsigned int truncated;
    unsigned int af_flag;

    unsigned int crc;

    unsigned int adc1[NUM_CHANNELS_PER_LINK];
    unsigned int adc2[NUM_CHANNELS_PER_LINK];
    unsigned int toa [NUM_CHANNELS_PER_LINK];
    unsigned int gain[NUM_CHANNELS_PER_LINK];
  };

  struct caloroc_waveform
  {
    std::vector<caloroc_frame> samples;
  };

  void add_frame(const unsigned int link, const caloroc_frame &f);
  const caloroc_frame *getFrame(const int wf, const int sample, const int ch, int &localch) const;

  std::deque<caloroc_waveform> _closed[NUM_LINKS];
  caloroc_waveform _open[NUM_LINKS];

  int _link_mask;
  int _channels;
  unsigned int _nr_offered;
};

#endif
