#ifndef __ONCSSUB_IDCALOROC1B_H__
#define __ONCSSUB_IDCALOROC1B_H__

#include "oncsSubevent.h"
#include <cstdint>
#include <vector>

// Decoder for the CALOROC1B ASIC's UDP datastream (IN2P3/LLR readout), packet format 310.
//
// A subevent's data area can hold several received UDP datagrams back to
// back (each one prefixed with the "0xaabbccdd" + received-length safety
// belt added by the capturing plugin), and each UDP datagram can in turn
// hold several CALOROC data frames for one link channel (0..5, i.e. 2 links
// per ASIC x 3 ASICs, any of which can be individually absent).
//
// Within one link, a "waveform" is N_VALUE (<=7 in self-trigger mode, more
// under a forced multi-L1A burst) consecutive frames: consecutive frames of
// the same waveform have their 16b timestamp step by exactly +1, and a
// bigger (signed, 16b-wraparound-safe) step means a new waveform started.
// evt_counter has no gap across a waveform boundary, so only the timestamp
// step can be used to find it. Confirmed by the IN2P3/LLR firmware group
// (2026-10-12) and validated against real N<=7 self-trigger data.
//
// Reporting follows oncsSub_idh2gcroc3_10G's scheme: ask how many waveforms
// were found (NR_WF), then iValue(wf, sample, channel [, what]) - channel
// spans a single combined address space across all 6 links (link =
// channel/18, local channel = channel%18), the same way oncsSub_idcaenv1742
// sizes CHANNELS from the highest enabled group (see its constructor): here
// CHANNELS = 18 * (highest link that produced any data + 1), so a disabled
// link in the middle (e.g. 0,1,4,5 present) still reports the full 108 and
// just reads back as 0 for the missing link's channels. LINKPRESENT mirrors
// oncsSub_idcaenv1742's GROUPPRESENT.

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idcaloroc1b : public oncsSubevent_w4 {
#else
class  oncsSub_idcaloroc1b : public oncsSubevent_w2 {
#endif

public:
  oncsSub_idcaloroc1b( subevtdata_ptr);

  void  dump ( OSTREAM& os = COUT);

  // "what" is one of:
  // "CHANNELS"    - total addressable channels (n ignored)
  // "NR_WF"       - number of waveforms found, across all links (n ignored)
  // "SAMPLESIZE"  - number of samples in waveform n
  // "LINKPRESENT" - 1 if link n (0..5) produced any data, else 0
  int    iValue(const int n, const char *what);

  // per-channel, per-sample access within waveform wf. channel is
  // 0..CHANNELS-1 (link = channel/NUM_CHANNELS_PER_LINK). Defaults to ADC1
  // ("HG" / 2-ADC-mode high gain).
  int    iValue(const int wf, const int sample, const int ch);

  // "what" is one of "ADC1", "ADC2", "TOA", "GAIN"
  int    iValue(const int wf, const int sample, const int ch, const char *what);

  static const int NUM_LINKS = 6;
  static const int NUM_CHANNELS_PER_LINK = 18;

protected:

  int decode();
  int decode_packet( const unsigned int *payload, const unsigned int payload_words );
  void groupWaveforms();

  int is_decoded;
  int _broken;

  struct caloroc_frame
  {
    unsigned int mode;
    unsigned int evt_counter;
    unsigned int timestamp;

    unsigned int hitmap;
    unsigned int numhits;
    unsigned int truncated;
    unsigned int af_flag;

    unsigned int crc;

    // indexed 0..NUM_CHANNELS_PER_LINK-1, only valid where the
    // corresponding hitmap bit is set (zero otherwise)
    unsigned int adc1[NUM_CHANNELS_PER_LINK];
    unsigned int adc2[NUM_CHANNELS_PER_LINK];
    unsigned int toa [NUM_CHANNELS_PER_LINK];
    unsigned int gain[NUM_CHANNELS_PER_LINK];
  };

  struct caloroc_waveform
  {
    std::vector<caloroc_frame> samples;
  };

  // shared bounds-checked lookup for the iValue(wf,sample,ch[,what])
  // pair, so the plain (most frequently called) overload can return
  // ADC1 directly without paying for a string comparison. Returns 0 and
  // leaves localch unset if (wf,sample,ch) is out of range.
  const caloroc_frame *getFrame( const int wf, const int sample, const int ch, int &localch );

  std::vector<caloroc_frame>    frames_by_link[NUM_LINKS];
  std::vector<caloroc_waveform> waveforms_by_link[NUM_LINKS];

  int link_mask;   // bit i set if link i produced at least one frame
  int channels;    // NUM_CHANNELS_PER_LINK * (highest present link + 1)
  int nr_wf;       // max waveform count across all present links

  // within one waveform, consecutive frames step by +1 in the 16b
  // timestamp (signed, wraparound-safe); anything bigger starts a new
  // waveform. Placeholder pending more real beam data - see project notes.
  static const int CONTIGUOUS_CLOCK = 5;

};

#endif /* __ONCSSUB_IDCALOROC1B_H__ */
