#include "caloroc1b_pool.h"

#include <cstring>
#include "oncsSubConstants.h"
#include <iomanip>
#include <sstream>

using namespace std;

caloroc1b_pool::caloroc1b_pool(const unsigned int low_mark, const unsigned int high_mark)
  : ROC_pool(IDCALOROC1B, "IDCALOROC1B", low_mark, high_mark)
{
  _link_mask = 0;
  _channels = 0;
  _nr_offered = 0;
}

// one datagram, starting after the marker/length (same as
// Packet_idcaloroc1b::decode_packet())
int caloroc1b_pool::decode_datagram(const uint32_t *payload, const unsigned int nbytes)
{
  const unsigned int payload_words = nbytes / 4;
  if ( payload_words < 2) return 0;

  unsigned int link = payload[1] & 0xff;
  if ( link >= (unsigned int) NUM_LINKS) return 0;

  unsigned int idx = 2;

  while ( idx < payload_words)
    {
      unsigned int specs = payload[idx++];

      bool invalid = (specs >> 31) & 0x1;
      bool zeroed  = (specs >> 30) & 0x1;

      if ( invalid) break;
      if ( zeroed) continue;

      unsigned int stored    =  specs        & 0x7f;
      unsigned int truncated = (specs >>  7) & 0x1;
      unsigned int numhits   = (specs >> 16) & 0x1f;
      unsigned int af_flag   = (specs >> 24) & 0x1;
      unsigned int has_ts    = (specs >> 29) & 0x1;

      // the 48-bit global timestamp, in 2 words before the frame (TSH, TSL)
      unsigned long long global_ts = 0;
      if ( has_ts)
	{
	  if ( idx + 2 > payload_words) break;
	  global_ts = ( (unsigned long long) (payload[idx] & 0xffff) << 32 ) | payload[idx+1];
	  idx += 2;
	}

      if ( stored == 0 || idx + stored > payload_words) break;

      unsigned int frame_end = idx + stored;

      unsigned int dh = payload[idx++];
      unsigned int hh = payload[idx++];

      caloroc_frame f;
      memset(&f, 0, sizeof(f));

      f.mode        = (dh >> 26) & 0x3;
      f.evt_counter = (dh >> 18) & 0x3f;
      f.timestamp   = (dh >>  1) & 0xffff;
      f.global_ts   = global_ts;
      f.hitmap      = hh & 0x3ffff;
      f.numhits     = numhits;
      f.truncated   = truncated;
      f.af_flag     = af_flag;

      for ( int ch = 0; ch < NUM_CHANNELS_PER_LINK && idx < frame_end - 1; ch++)
	{
	  if ( !((f.hitmap >> ch) & 0x1) ) continue;

	  unsigned int cw = payload[idx++];
	  f.gain[ch] = (cw >> 30) & 0x3;
	  f.adc1[ch] = (cw >> 20) & 0x3ff;
	  f.adc2[ch] = (cw >> 10) & 0x3ff;
	  f.toa [ch] =  cw        & 0x3ff;
	}

      if ( idx < frame_end)
	{
	  f.crc = payload[idx++];
	}
      idx = frame_end;

      add_frame(link, f);
    }
  return 0;
}

// same grouping rule as Packet_idcaloroc1b::groupWaveforms()
void caloroc1b_pool::add_frame(const unsigned int link, const caloroc_frame &f)
{
  std::vector<caloroc_frame> &open = _open[link].samples;

  if ( !open.empty())
    {
      int16_t diff = (int16_t) (uint16_t) (f.timestamp - open.back().timestamp);
      if ( diff > CONTIGUOUS_CLOCK || diff < -CONTIGUOUS_CLOCK)
	{
	  _closed[link].push_back(std::move(_open[link]));
	  _open[link].samples.clear();
	}
    }
  open.push_back(f);

  if ( !(_link_mask & (1 << link)) )
    {
      _link_mask |= (1 << link);
      int highest = 0;
      for ( int i = 0; i < NUM_LINKS; i++)
	{
	  if ( _link_mask & (1 << i)) highest = i;
	}
      _channels = NUM_CHANNELS_PER_LINK * (highest + 1);
    }
}

void caloroc1b_pool::close_open()
{
  for ( int link = 0; link < NUM_LINKS; link++)
    {
      if ( _open[link].samples.empty()) continue;
      _closed[link].push_back(std::move(_open[link]));
      _open[link].samples.clear();
    }
}

void caloroc1b_pool::update_offer()
{
  unsigned int n = 0;
  bool first = true;

  for ( int link = 0; link < NUM_LINKS; link++)
    {
      if ( !(_link_mask & (1 << link)) ) continue;
      unsigned int s = _closed[link].size();
      if ( _draining)
	{
	  if ( s > n) n = s;   // whatever is left, as the Packet does
	}
      else
	{
	  if ( first || s < n) n = s;   // complete on all links
	}
      first = false;
    }
  _nr_offered = n;
}

void caloroc1b_pool::drop_offer()
{
  for ( int link = 0; link < NUM_LINKS; link++)
    {
      std::deque<caloroc_waveform> &c = _closed[link];
      unsigned int n = _nr_offered;
      if ( n > c.size()) n = c.size();
      c.erase(c.begin(), c.begin() + n);
    }
  _nr_offered = 0;
}

bool caloroc1b_pool::has_pending() const
{
  for ( int link = 0; link < NUM_LINKS; link++)
    {
      if ( !_closed[link].empty() || !_open[link].samples.empty()) return true;
    }
  return false;
}

int caloroc1b_pool::iValue(const int n, const char *what)
{
  decode();

  if ( strcmp(what, "CHANNELS") == 0)
    {
      return _channels;
    }

  if ( strcmp(what, "NR_WF") == 0)
    {
      return (int) _nr_offered;
    }

  if ( strcmp(what, "LINKPRESENT") == 0)
    {
      if ( n < 0 || n >= NUM_LINKS) return 0;
      return (_link_mask >> n) & 1;
    }

  if ( strcmp(what, "SAMPLESIZE") == 0)
    {
      if ( n < 0 || (unsigned int) n >= _nr_offered) return 0;
      unsigned int size = 0;
      for ( int link = 0; link < NUM_LINKS; link++)
	{
	  if ( (unsigned int) n < _closed[link].size() )
	    {
	      unsigned int s = _closed[link][n].samples.size();
	      if ( s > size) size = s;
	    }
	}
      return (int) size;
    }

  return 0;
}

const caloroc1b_pool::caloroc_frame *
caloroc1b_pool::getFrame(const int wf, const int sample, const int ch, int &localch) const
{
  if ( wf < 0 || sample < 0 || ch < 0 || ch >= _channels) return 0;
  if ( (unsigned int) wf >= _nr_offered) return 0;

  int link = ch / NUM_CHANNELS_PER_LINK;
  localch  = ch % NUM_CHANNELS_PER_LINK;

  if ( (unsigned int) wf >= _closed[link].size()) return 0;
  const caloroc_waveform &w = _closed[link][wf];
  if ( (unsigned int) sample >= w.samples.size()) return 0;
  return &w.samples[sample];
}

int caloroc1b_pool::iValue(const int wf, const int sample, const int ch)
{
  decode();
  int localch;
  const caloroc_frame *f = getFrame(wf, sample, ch, localch);
  if ( !f) return 0;
  return f->adc1[localch];
}

int caloroc1b_pool::iValue(const int wf, const int sample, const int ch, const char *what)
{
  decode();
  int localch;
  const caloroc_frame *f = getFrame(wf, sample, ch, localch);
  if ( !f) return 0;

  if      ( strcmp(what, "ADC1") == 0) return f->adc1[localch];
  else if ( strcmp(what, "ADC2") == 0) return f->adc2[localch];
  else if ( strcmp(what, "TOA") == 0)  return f->toa[localch];
  else if ( strcmp(what, "GAIN") == 0) return f->gain[localch];
  return 0;
}

long long caloroc1b_pool::lValue(const int wf, const int sample, const int ch)
{
  decode();
  int localch;
  const caloroc_frame *f = getFrame(wf, sample, ch, localch);
  if ( !f) return 0;
  return (long long) f->global_ts;
}

void caloroc1b_pool::dump(std::ostream &os)
{
  int nwf = iValue(0, "NR_WF");

  os << " caloroc1b_pool  depth: " << depth() << " datagrams" << endl;
  os << " Number of Channels:  " << iValue(0, "CHANNELS") << endl;
  os << " Number of Waveforms: " << nwf << endl;
  os << " Link present:        ";
  for ( int link = 0; link < NUM_LINKS; link++) os << iValue(link, "LINKPRESENT") << " ";
  os << endl << endl;

  for ( int w = 0; w < nwf; w++)
    {
      int ssize = iValue(w, "SAMPLESIZE");
      os << " ----- Waveform " << w << "  Samples: " << ssize << endl;

      // the global timestamp of the first sample, per link
      for ( int link = 0; link < NUM_LINKS; link++)
	{
	  if ( !iValue(link, "LINKPRESENT") ) continue;
	  long long ts = lValue(w, 0, link * NUM_CHANNELS_PER_LINK);
	  if ( ts) os << "  link " << link << " timestamp 0x" << hex << ts << dec << " (" << ts << ")" << endl;
	}

      for ( int ch = 0; ch < iValue(0, "CHANNELS"); ch++)
	{
	  int show_this = 0;
	  for ( int s = 0; s < ssize; s++)
	    {
	      if ( iValue(w, s, ch) || iValue(w, s, ch, "ADC2") || iValue(w, s, ch, "TOA") ) show_this = 1;
	    }
	  if ( !show_this) continue;

	  os << setw(4) << ch << " | ADC1 |   " << hex << left;
	  for ( int s = 0; s < ssize; s++) os << setw(7) << iValue(w, s, ch, "ADC1");
	  os << right << dec << endl;

	  os << setw(4) << ch << " | ADC2 |   " << left;
	  for ( int s = 0; s < ssize; s++)
	    {
	      ostringstream oss;
	      oss << hex << iValue(w, s, ch, "ADC2") << "(" << dec << iValue(w, s, ch, "GAIN") << ")";
	      os << setw(7) << oss.str();
	    }
	  os << right << endl;

	  os << setw(4) << ch << " | TOA  |   " << hex << left;
	  for ( int s = 0; s < ssize; s++) os << setw(7) << iValue(w, s, ch, "TOA");
	  os << right << dec << endl << endl;
	}
    }
}
