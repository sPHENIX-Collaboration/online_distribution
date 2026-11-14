#include "h2gcroc3_10G_pool.h"

#include <cstring>
#include "oncsSubConstants.h"
#include <iomanip>

using namespace std;

h2gcroc3_10G_pool::h2gcroc3_10G_pool(const unsigned int low_mark, const unsigned int high_mark)
  : ROC_pool(IDH2GCROC3_10G, "IDH2GCROC3_10G", low_mark, high_mark)
{
  _sample_base = 0;
  _have_open_wf = false;
  _open_wf.first = 0;
  _open_wf.length = 0;
  _last_closed_ts = 0;
  memset(&_open_sample, 0, sizeof(sample));
  _have_open_sample = false;
  _old_timestamp = 0;
  _nr_offered = 0;
  _nr_offered_samples = 0;
  _nr_channels = 0;
}

unsigned int h2gcroc3_10G_pool::u4frombyte(const uint8_t *v)
{
  return (v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3];
}

uint64_t h2gcroc3_10G_pool::u8frombyte(const uint8_t *v)
{
  uint64_t r = 0;
  for ( int i = 0; i < 8; i++)
    {
      r = (r << 8) | v[i];
    }
  return r;
}

// one datagram, starting after the marker/length: 14 header bytes, then
// 192-byte lines, the last 4 bytes are not part of any line (same
// boundaries as Packet_idh2gcroc3_10G::decode())
int h2gcroc3_10G_pool::decode_datagram(const uint32_t *d, const unsigned int nbytes)
{
  const uint8_t *b = (const uint8_t *) d;
  const unsigned int line_bytes = 4*48;

  for ( unsigned int pos = 14; pos + 4 < nbytes && pos + line_bytes <= nbytes; pos += line_bytes)
    {
      decode_line(b + pos);
    }
  return 0;
}

// same as Packet_idh2gcroc3_10G::decode_line()
int h2gcroc3_10G_pool::decode_line(const uint8_t *d)
{
  if ( d[0] != 0xaa && d[1] != 0x5a) return 0;

  unsigned int ichip = d[2] & 0xf;
  int group = d[3] - 36;

  uint32_t trigger_in  = u4frombyte(&d[4]);
  uint32_t trigger_out = u4frombyte(&d[8]);
  uint32_t evt_ctr     = u4frombyte(&d[12]);
  uint64_t timestamp   = u8frombyte(&d[16]);

  uint32_t ch[36];

  uint32_t ROC_HDR = u4frombyte(&d[32 + 0]);
  uint32_t CMN = u4frombyte(&d[32 + 4]);
  for ( int n = 0; n < 18; n++)
    {
      ch[n] = u4frombyte(&d[32 + 8 + n*4]);
    }
  uint32_t CALIB = u4frombyte(&d[112]);
  for ( int n = 0; n < 18; n++)
    {
      ch[n+18] = u4frombyte(&d[112 + 4 + n*4]);
    }
  uint32_t CRC = u4frombyte(&d[188]);

  if ( ichip >= MAX_CHIPS) return 0;

  if ( (int) (ichip+1) * 72 > _nr_channels) _nr_channels = (ichip+1) * 72;

  if ( !_have_open_sample)
    {
      memset(&_open_sample, 0, sizeof(sample));
      _open_sample.timestamp = timestamp;
      _old_timestamp = timestamp;
      _have_open_sample = true;
    }

  // the timestamp moved on - that sample is done
  if ( ( timestamp >= _old_timestamp && timestamp - _old_timestamp > 5 ) ||
       ( timestamp <  _old_timestamp && _old_timestamp - timestamp > 5 ) )
    {
      close_sample();
      memset(&_open_sample, 0, sizeof(sample));
      _open_sample.timestamp = timestamp;
      _have_open_sample = true;
    }
  _old_timestamp = timestamp;

  int chip_offset = ichip * 72;
  int group_offset = group ? 36 : 0;

  for ( int c = 0; c < 36; c++)
    {
      _open_sample.ADC[chip_offset + group_offset + c] = (ch[c] >> 20) & 0x3ff;
      _open_sample.tot[chip_offset + group_offset + c] = (ch[c] >> 10) & 0x3ff;
      _open_sample.toa[chip_offset + group_offset + c] =  ch[c]        & 0x3ff;
    }

  // the ROC header is 0101 + 12b Bx + 6b Event + 3b Orbit + H1 H2 H3 + 0101
  // (H2GCROC3 datasheet p. 30); H1 = error in the header, H2 in CM and
  // channels 0-17, H3 in Calib and channels 18-35 of this half-chip
  _open_sample.error[(chip_offset + group_offset) / 36] = (ROC_HDR >> 4) & 0x7;

  _open_sample.trigger_in    = trigger_in;
  _open_sample.trigger_out   = trigger_out;
  _open_sample.event_counter = evt_ctr;
  _open_sample.cmn           = CMN;
  _open_sample.calib         = CALIB;
  _open_sample.crc32         = CRC;

  return 0;
}

// the open sample is complete; append it to the open waveform, or close
// that and start a new one (same rule as Packet_idh2gcroc3_10G::parse_timeline())
void h2gcroc3_10G_pool::close_sample()
{
  if ( !_have_open_sample) return;

  const unsigned long long number = _sample_base + _samples.size();
  const uint64_t ts = _open_sample.timestamp;

  _samples.push_back(_open_sample);
  _have_open_sample = false;

  if ( _have_open_wf && ts - _last_closed_ts > CONTIGUOUS_CLOCK)
    {
      _waveforms.push_back(_open_wf);
      _have_open_wf = false;
    }

  if ( !_have_open_wf)
    {
      _open_wf.first = number;
      _open_wf.length = 0;
      _have_open_wf = true;
    }
  _open_wf.length++;
  _last_closed_ts = ts;
}

void h2gcroc3_10G_pool::close_open()
{
  close_sample();
  if ( _have_open_wf)
    {
      _waveforms.push_back(_open_wf);
      _have_open_wf = false;
    }
}

void h2gcroc3_10G_pool::update_offer()
{
  _nr_offered = _waveforms.size();
  _nr_offered_samples = 0;
  for ( unsigned int i = 0; i < _nr_offered; i++)
    {
      _nr_offered_samples += _waveforms[i].length;
    }
}

void h2gcroc3_10G_pool::drop_offer()
{
  if ( !_nr_offered) return;

  // the offered waveforms are the oldest samples - the end of the last
  // one is where the samples still needed begin
  const wf_bounds &last = _waveforms[_nr_offered - 1];
  const unsigned long long keep_from = last.first + last.length;

  while ( _sample_base < keep_from && !_samples.empty())
    {
      _samples.pop_front();
      _sample_base++;
    }
  _waveforms.erase(_waveforms.begin(), _waveforms.begin() + _nr_offered);

  _nr_offered = 0;
  _nr_offered_samples = 0;
}

bool h2gcroc3_10G_pool::has_pending() const
{
  return ( _have_open_sample || _have_open_wf || !_waveforms.empty() );
}

const h2gcroc3_10G_pool::sample *h2gcroc3_10G_pool::getSample(const int wf, const int s) const
{
  if ( wf < 0 || s < 0) return 0;
  if ( (unsigned int) wf >= _nr_offered) return 0;
  const wf_bounds &b = _waveforms[wf];
  if ( (unsigned int) s >= b.length) return 0;
  return &_samples[b.first + s - _sample_base];
}

// samples counted across the waveforms on offer
const h2gcroc3_10G_pool::sample *h2gcroc3_10G_pool::getSample(const int s) const
{
  if ( s < 0 || (unsigned long long) s >= _nr_offered_samples) return 0;
  return &_samples[s];
}

int h2gcroc3_10G_pool::iValue(const int n, const char *what)
{
  decode();

  if ( strcmp(what, "CHANNELS") == 0)
    {
      return _nr_channels;
    }

  if ( strcmp(what, "SAMPLES") == 0)
    {
      return (int) _nr_offered_samples;
    }

  if ( strcmp(what, "NR_WF") == 0)
    {
      return (int) _nr_offered;
    }

  if ( strcmp(what, "SAMPLESIZE") == 0)
    {
      if ( n < 0 || (unsigned int) n >= _nr_offered) return 0;
      return _waveforms[n].length;
    }

  const sample *s = getSample(n);
  if ( !s) return 0;

  if ( strcmp(what, "TIMESTAMP") == 0) return s->timestamp;
  if ( strcmp(what, "CMN") == 0)       return s->cmn;
  if ( strcmp(what, "CALIB") == 0)     return s->calib;

  return 0;
}

int h2gcroc3_10G_pool::iValue(const int sample, const int ch)
{
  decode();
  if ( ch < 0 || ch >= _nr_channels) return 0;
  const h2gcroc3_10G_pool::sample *s = getSample(sample);
  if ( !s) return 0;
  return s->ADC[ch];
}

int h2gcroc3_10G_pool::iValue(const int sample, const int ch, const char *what)
{
  decode();
  if ( ch < 0 || ch >= _nr_channels) return 0;
  const h2gcroc3_10G_pool::sample *s = getSample(sample);
  if ( !s) return 0;

  if ( strcmp(what, "TOT") == 0) return s->tot[ch];
  if ( strcmp(what, "TOA") == 0) return s->toa[ch];
  return 0;
}

int h2gcroc3_10G_pool::iValue(const int wf, const int sample, const int ch)
{
  decode();
  if ( ch < 0 || ch >= _nr_channels) return 0;
  const h2gcroc3_10G_pool::sample *s = getSample(wf, sample);
  if ( !s) return 0;
  return s->ADC[ch];
}

int h2gcroc3_10G_pool::iValue(const int wf, const int sample, const int ch, const char *what)
{
  decode();
  if ( ch < 0 || ch >= _nr_channels) return 0;
  const h2gcroc3_10G_pool::sample *s = getSample(wf, sample);
  if ( !s) return 0;

  if ( strcmp(what, "TOT") == 0)           return s->tot[ch];
  if ( strcmp(what, "TOA") == 0)           return s->toa[ch];
  if ( strcmp(what, "ERROR") == 0)         return s->error[ch / 36];
  if ( strcmp(what, "TRIGGER_IN") == 0)    return s->trigger_in;
  if ( strcmp(what, "TRIGGER_OUT") == 0)   return s->trigger_out;
  if ( strcmp(what, "EVENT_COUNTER") == 0) return s->event_counter;
  return 0;
}

long long h2gcroc3_10G_pool::lValue(const int wf, const int sample, const int ch)
{
  decode();
  if ( ch < 0 || ch >= _nr_channels) return 0;
  const h2gcroc3_10G_pool::sample *s = getSample(wf, sample);
  if ( !s) return 0;
  return (long long) s->timestamp;
}

void h2gcroc3_10G_pool::dump(std::ostream &os)
{
  int e = iValue(0, "NR_WF");
  os << " h2gcroc3_10G_pool  depth: " << depth() << " datagrams" << endl;
  os << " Number of Channels:  " << iValue(0, "CHANNELS") << endl;
  os << " Number of Waveforms: " << e << endl << endl;

  for ( int n = 0; n < e; n++)
    {
      int ss = iValue(n, "SAMPLESIZE");
      os << " ----- Waveform  " << n << " Samples: " << ss
	 << "  Timestamp: 0x" << hex << lValue(n, 0, 0) << "  " << dec << lValue(n, 0, 0) << endl;

      for ( int ic = 0; ic < iValue(0, "CHANNELS"); ic++)
	{
	  int show_this = 0;
	  for ( int is = 0; is < ss; is++)
	    {
	      if ( iValue(n, is, ic) || iValue(n, is, ic, "TOT") || iValue(n, is, ic, "TOA") ) show_this = 1;
	    }
	  if ( !show_this) continue;

	  os << setw(4) << ic << " | ADC | " << hex;
	  for ( int is = 0; is < ss; is++) os << " " << setw(4) << iValue(n, is, ic);
	  os << dec << endl;

	  os << setw(4) << ic << " | TOT | " << hex;
	  for ( int is = 0; is < ss; is++) os << " " << setw(4) << iValue(n, is, ic, "TOT");
	  os << dec << endl;

	  os << setw(4) << ic << " | TOA | " << hex;
	  for ( int is = 0; is < ss; is++) os << " " << setw(4) << iValue(n, is, ic, "TOA");
	  os << dec << endl;

	  // the hamming error bits (H1 H2 H3), only if there are any
	  int has_error = 0;
	  for ( int is = 0; is < ss; is++)
	    {
	      if ( iValue(n, is, ic, "ERROR") ) has_error = 1;
	    }
	  if ( has_error)
	    {
	      os << setw(4) << ic << " | ERR | ";
	      for ( int is = 0; is < ss; is++) os << " " << setw(4) << iValue(n, is, ic, "ERROR");
	      os << endl;
	    }
	  os << endl;
	}
    }
}
