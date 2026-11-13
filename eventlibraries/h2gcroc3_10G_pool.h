#ifndef __H2GCROC3_10G_POOL_H__
#define __H2GCROC3_10G_POOL_H__

#include "ROC_pool.h"

// Pool for packet format 302 (IDH2GCROC3_10G), same API as
// oncsSub_idh2gcroc3_10G. A "sample" holds the values of all channels
// for one timestamp; a waveform is a run of samples less than
// CONTIGUOUS_CLOCK apart.
//
// "NR_WF" is the number of complete waveforms on offer, wf = 0 .. NR_WF-1.
// The sample-indexed calls ("SAMPLES", "TIMESTAMP", "CMN", "CALIB",
// iValue(sample, ch [, what])) count the samples of the waveforms on offer.

class h2gcroc3_10G_pool : public ROC_pool
{
public:
  h2gcroc3_10G_pool(const unsigned int low_mark = 200, const unsigned int high_mark = 600);
  ~h2gcroc3_10G_pool() {}

  int    iValue(const int sample, const int ch);
  int    iValue(const int sample, const int ch, const char *what);
  int    iValue(const int n, const char *what);
  int    iValue(const int wf, const int sample, const int ch);
  int    iValue(const int wf, const int sample, const int ch, const char *what);
  long long lValue(const int wf, const int sample, const int ch);

  void dump(std::ostream &os = std::cout);

  static const unsigned int MAX_CHIPS = 4;
  static const uint64_t CONTIGUOUS_CLOCK = 164;

protected:

  int decode_datagram(const uint32_t *d, const unsigned int nbytes);
  void close_open();
  void update_offer();
  void drop_offer();
  bool has_pending() const;

  int decode_line(const uint8_t *d);
  void close_sample();

  static unsigned int u4frombyte(const uint8_t *v);
  static uint64_t u8frombyte(const uint8_t *v);

  struct sample
  {
    unsigned int ADC[72*MAX_CHIPS];
    unsigned int tot[72*MAX_CHIPS];
    unsigned int toa[72*MAX_CHIPS];
    unsigned int error[2*MAX_CHIPS];  // H1 H2 H3 hamming error bits per half-chip
    unsigned int trigger_in;
    unsigned int trigger_out;
    unsigned int event_counter;
    uint64_t timestamp;
    unsigned int cmn;
    unsigned int calib;
    unsigned int crc32;
  };

  // samples are numbered from the start of the stream; _samples[0] is
  // sample number _sample_base
  struct wf_bounds
  {
    unsigned long long first;
    unsigned int length;
  };

  const sample *getSample(const int wf, const int s) const;
  const sample *getSample(const int s) const;

  std::deque<sample> _samples;      // closed samples
  unsigned long long _sample_base;

  std::deque<wf_bounds> _waveforms; // closed waveforms
  bool _have_open_wf;
  wf_bounds _open_wf;
  uint64_t _last_closed_ts;

  sample _open_sample;              // the one being filled
  bool _have_open_sample;
  uint64_t _old_timestamp;

  unsigned int _nr_offered;
  unsigned long long _nr_offered_samples;
  int _nr_channels;
};

#endif
