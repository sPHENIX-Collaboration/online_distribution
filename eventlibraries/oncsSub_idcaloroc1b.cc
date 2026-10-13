#include "oncsSub_idcaloroc1b.h"

#include <string.h>
#include <sstream>
#include "buffer.h"

using namespace std;

oncsSub_idcaloroc1b::oncsSub_idcaloroc1b(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{
  is_decoded = 0;
  _broken = 0;
  link_mask = 0;
  channels = 0;
  nr_wf = 0;
}

// decodes one captured UDP datagram, starting right after the
// "0xaabbccdd"/received-length safety belt (i.e. "payload" is the Pkt
// Header 1 word). Pushes one caloroc_frame per CALOROC data frame found
// onto frames_by_link[link].
int oncsSub_idcaloroc1b::decode_packet( const unsigned int *payload, const unsigned int payload_words )
{
  if ( payload_words < 2) return 0;

  // --- Pkt Header 2 (Pkt Header 1's PSIZE is informational only here -
  // decode_packet() is already bounded by payload_words from the caller) ---
  unsigned int w1   = payload[1];
  unsigned int link = w1 & 0xff;

  if ( link >= NUM_LINKS) return 0;   // not one of our 6 link channels - ignore

  unsigned int idx = 2;

  while ( idx < payload_words )
    {
      unsigned int specs = payload[idx++];

      bool invalid = (specs >> 31) & 0x1;
      bool zeroed  = (specs >> 30) & 0x1;

      if ( invalid) break;   // empty Specs FIFO read - nothing more in this datagram

      if ( zeroed)
	{
	  // 'zeroed' marker frame (Specs FIFO was full) - no frame data to decode
	  continue;
	}

      unsigned int stored    =  specs        & 0x7f;   // words following this specs word
      unsigned int truncated = (specs >>  7) & 0x1;
      unsigned int numhits   = (specs >> 16) & 0x1f;
      unsigned int af_flag   = (specs >> 24) & 0x1;

      if ( stored == 0 || idx + stored > payload_words) break;   // malformed/truncated - bail

      unsigned int frame_end = idx + stored;   // one past the last word of this frame (the CRC)

      // Frame Word 0: Data Header
      unsigned int dh         = payload[idx++];
      unsigned int mode       = (dh >> 26) & 0x3;
      unsigned int evt_counter= (dh >> 18) & 0x3f;
      unsigned int timestamp  = (dh >>  1) & 0xffff;

      // Frame Word 1: Hit Header
      unsigned int hh     = payload[idx++];
      unsigned int hitmap =  hh & 0x3ffff;   // 18 channels

      caloroc_frame f;
      memset( &f, 0, sizeof(f) );

      f.mode        = mode;
      f.evt_counter = evt_counter;
      f.timestamp   = timestamp;
      f.hitmap      = hitmap;
      f.numhits     = numhits;
      f.truncated   = truncated;
      f.af_flag     = af_flag;

      // channel data words - one per bit set in the hitmap, ascending channel order
      for ( int ch = 0; ch < NUM_CHANNELS_PER_LINK && idx < frame_end - 1; ch++)
	{
	  if ( !((hitmap >> ch) & 0x1) ) continue;

	  unsigned int cw = payload[idx++];

	  f.gain[ch] = (cw >> 30) & 0x3;
	  f.adc1[ch] = (cw >> 20) & 0x3ff;
	  f.adc2[ch] = (cw >> 10) & 0x3ff;
	  f.toa [ch] =  cw        & 0x3ff;
	}

      // last word of the frame: CRC
      if ( idx < frame_end)
	{
	  f.crc = payload[idx++];
	}

      idx = frame_end;

      frames_by_link[link].push_back(f);
    }

  return 0;
}

int oncsSub_idcaloroc1b::decode()
{
  if (is_decoded) return 0;
  is_decoded = 1;

  const unsigned int *buffer = ( const unsigned int *) &SubeventHdr->data;

  unsigned int current_index_words = 0;
  unsigned int total_words = (unsigned int) getDataLength();

  while ( current_index_words + 2 <= total_words )
    {
      unsigned int marker = buffer[current_index_words];

      if ( marker != 0xaabbccdd)
	{
	  coutfl << "bad safety marker 0x" << hex << marker << dec
		 << " at word " << current_index_words << ", giving up" << endl;
	  _broken = 1;
	  break;
	}

      unsigned int received_bytes = buffer[current_index_words + 1];
      unsigned int payload_words  = received_bytes / 4;

      const unsigned int *payload = buffer + current_index_words + 2;

      if ( current_index_words + 2 + payload_words > total_words)
	{
	  payload_words = total_words - current_index_words - 2;   // truncated, decode what we have
	}

      decode_packet( payload, payload_words );

      current_index_words += 2 + payload_words;
    }

  groupWaveforms();

  return 0;
}

// groups each link's frame sequence into waveforms using a signed,
// wraparound-safe 16-bit timestamp diff against CONTIGUOUS_CLOCK, then
// derives link_mask / channels / nr_wf from the result.
void oncsSub_idcaloroc1b::groupWaveforms()
{
  link_mask = 0;
  nr_wf = 0;

  for ( int link = 0; link < NUM_LINKS; link++)
    {
      const std::vector<caloroc_frame> &frames = frames_by_link[link];
      if ( frames.empty()) continue;

      link_mask |= (1 << link);

      std::vector<caloroc_waveform> &waveforms = waveforms_by_link[link];

      for ( size_t k = 0; k < frames.size(); k++)
	{
	  bool startNew = waveforms.empty();

	  if ( !startNew)
	    {
	      unsigned int prev_ts = frames[k-1].timestamp;
	      unsigned int cur_ts  = frames[k].timestamp;
	      int16_t diff = (int16_t) (uint16_t) (cur_ts - prev_ts);   // wraparound-safe signed step

	      if ( diff > CONTIGUOUS_CLOCK || diff < -CONTIGUOUS_CLOCK)
		startNew = true;
	    }

	  if ( startNew)
	    waveforms.push_back( caloroc_waveform() );

	  waveforms.back().samples.push_back( frames[k] );
	}

      if ( (int) waveforms.size() > nr_wf) nr_wf = (int) waveforms.size();
    }

  // CHANNELS = NUM_CHANNELS_PER_LINK * (highest present link + 1) - same
  // spirit as oncsSub_idcaenv1742 sizing CHANNELS from the highest enabled
  // group, done the C++17-compatible way (manual bit scan).
  channels = 0;
  if ( link_mask)
    {
      const int b = 1;
      int highestBit = 0;
      for ( int i = 0; i < NUM_LINKS; i++)
	{
	  if ( (b<<i) & link_mask) highestBit = i;
	}
      channels = NUM_CHANNELS_PER_LINK * (highestBit+1);
    }
}

int oncsSub_idcaloroc1b::iValue(const int n, const char *what)
{
  decode();

  if ( strcmp(what,"CHANNELS") == 0 )
    {
      return channels;
    }

  if ( strcmp(what,"NR_WF") == 0 )
    {
      return nr_wf;
    }

  if ( strcmp(what,"LINKPRESENT") == 0 )
    {
      if ( n < 0 || n >= NUM_LINKS) return 0;
      return (link_mask >> n) & 1;
    }

  if ( strcmp(what,"SAMPLESIZE") == 0 )
    {
      if ( n < 0) return 0;
      unsigned int uwf = n;
      unsigned int size = 0;
      for ( int link = 0; link < NUM_LINKS; link++)
	{
	  if ( uwf < waveforms_by_link[link].size() )
	    {
	      unsigned int s = (unsigned int) waveforms_by_link[link][uwf].samples.size();
	      if ( s > size) size = s;
	    }
	}
      return (int) size;
    }

  return 0;
}

const oncsSub_idcaloroc1b::caloroc_frame *
oncsSub_idcaloroc1b::getFrame( const int wf, const int sample, const int ch, int &localch )
{
  if ( wf < 0 || sample < 0 || ch < 0 || ch >= channels) return 0;

  int link = ch / NUM_CHANNELS_PER_LINK;
  localch  = ch % NUM_CHANNELS_PER_LINK;

  unsigned int uwf = wf;
  if ( uwf >= waveforms_by_link[link].size() ) return 0;

  const caloroc_waveform &w = waveforms_by_link[link][uwf];

  unsigned int us = sample;
  if ( us >= w.samples.size() ) return 0;

  return &w.samples[us];
}

// most frequently called accessor - goes straight at ADC1, no string
// comparison (unlike the "what"-variant below, see getFrame()).
int oncsSub_idcaloroc1b::iValue(const int wf, const int sample, const int ch)
{
  decode();

  int localch;
  const caloroc_frame *f = getFrame(wf, sample, ch, localch);
  if ( !f) return 0;

  return f->adc1[localch];
}

int oncsSub_idcaloroc1b::iValue(const int wf, const int sample, const int ch, const char *what)
{
  decode();

  int localch;
  const caloroc_frame *f = getFrame(wf, sample, ch, localch);
  if ( !f) return 0;

  if      ( strcmp(what,"ADC1") == 0 ) return f->adc1[localch];
  else if ( strcmp(what,"ADC2") == 0 ) return f->adc2[localch];
  else if ( strcmp(what,"TOA")  == 0 ) return f->toa[localch];
  else if ( strcmp(what,"GAIN") == 0 ) return f->gain[localch];

  return 0;
}

void oncsSub_idcaloroc1b::dump(std::ostream &os)
{
  identify(os);

  if ( _broken)
    {
      os << " ** corrupt packet **" << endl;
    }

  // we only go through iValue() from here on, same as the rest of the
  // output - so ddump can never show something a user couldn't also get
  // via the public accessors.

  int nwf = iValue(0, "NR_WF");

  os << " Number of Channels:  " << iValue(0, "CHANNELS") << endl;
  os << " Number of Waveforms: " << nwf << endl;

  os << " Link present:        ";
  for ( int link = 0; link < NUM_LINKS; link++)
    os << iValue(link, "LINKPRESENT") << " ";
  os << endl << endl;

  for ( int w = 0; w < nwf; w++)
    {
      int ssize = iValue(w, "SAMPLESIZE");

      os << " ----- Waveform " << w << "  Samples: " << ssize << endl;

      for ( int ch = 0; ch < iValue(0,"CHANNELS"); ch++)
	{
	  int show_this = 0;
	  for ( int s = 0; s < ssize; s++)
	    {
	      if ( iValue(w,s,ch) || iValue(w,s,ch,"ADC2") || iValue(w,s,ch,"TOA") ) show_this = 1;
	    }
	  if ( !show_this) continue;

	  // ADC2's gain can change sample by sample, so it's shown inline as
	  // "value(gain)"; all three rows use the same left-justified,
	  // width-7 fields so the sample columns line up across them.
	  os << setw(4) << ch << " | ADC1 |   " << hex << left;
	  for ( int s = 0; s < ssize; s++) os << setw(7) << iValue(w,s,ch,"ADC1");
	  os << right << dec << endl;

	  os << setw(4) << ch << " | ADC2 |   " << left;
	  for ( int s = 0; s < ssize; s++)
	    {
	      ostringstream oss;
	      oss << hex << iValue(w,s,ch,"ADC2") << "(" << dec << iValue(w,s,ch,"GAIN") << ")";
	      os << setw(7) << oss.str();
	    }
	  os << right << endl;

	  os << setw(4) << ch << " | TOA  |   " << hex << left;
	  for ( int s = 0; s < ssize; s++) os << setw(7) << iValue(w,s,ch,"TOA");
	  os << right << dec << endl;

	  os << endl;
	}
    }
}
