#include <iostream>
#include <iomanip>

#include "caen_calibration.h"
#include "packet.h"
#include "oncsSubConstants.h"

using namespace std;

// the time of one turn of the 1024-cell DRS4 ring, in ns, for each speed
static const double ring_period[] = { 1024 * 0.2, 1024 * 0.4, 1024 * 1.0 };

caen_calibration::caen_calibration(Packet *calib)
{
  _broken = 0;
  _serial = 0;
  _readout_id = 0;
  _warned_id = 0;
  _samples = 0;
  _speed = 0;
  for ( int s = 0; s < NSPEEDS; s++) _status[s] = -1;

  _offset.assign(NSPEEDS * NCELLS * NCHANNELS, 0);
  _nsample.assign(NSPEEDS * NCELLS * NCHANNELS, 0);
  _timevec.assign(NSPEEDS * NCELLS * NCHIPS, 0);
  _current_wave.assign(NCELLS * NCHANNELS, 0);
  _current_time.assign(NCELLS * NCHIPS, 0);

  if ( !calib || calib->getHitFormat() != IDCAENV1742CALIB )
    {
      cout << "caen_calibration: no CAEN calibration packet given" << endl;
      _broken = 1;
      return;
    }

  if ( calib->iValue(0,"CELLS") != NCELLS || calib->iValue(0,"CHANNELS") != NCHANNELS
       || calib->iValue(0,"SPEEDS") != NSPEEDS )
    {
      cout << "caen_calibration: unexpected calibration packet layout" << endl;
      _broken = 1;
      return;
    }

  _serial = calib->iValue(0,"SERIAL");
  _readout_id = calib->iValue(0,"READOUTID");

  for ( int s = 0; s < NSPEEDS; s++)
    {
      _status[s] = calib->iValue(s,"STATUS");
      for ( int cell = 0; cell < NCELLS; cell++)
	{
	  for ( int ch = 0; ch < NCHANNELS; ch++)
	    {
	      _offset[ (s*NCELLS + cell) * NCHANNELS + ch] = calib->iValue(cell, ch, s, "OFFSET");
	      // this one goes by sample number, the index is not a cell here
	      _nsample[ (s*NCELLS + cell) * NCHANNELS + ch] = calib->iValue(cell, ch, s, "NSAMPLE");
	    }
	  for ( int chip = 0; chip < NCHIPS; chip++)
	    {
	      _timevec[ (s*NCELLS + cell) * NCHIPS + chip] = calib->dValue(cell, chip*8, s);
	    }
	}
    }
}

// the same corrections as caen_correction::init
int caen_calibration::init (Packet *p)
{
  if ( _broken || !p ) return -1;

  if ( _readout_id && p->getIdentifier() != _readout_id && !_warned_id )
    {
      cout << "caen_calibration: warning - the calibration of unit " << _serial
	   << " was taken for packet " << _readout_id
	   << ", but is used for packet " << p->getIdentifier() << endl;
      _warned_id = 1;
    }

  const int speed = p->iValue(0,"FREQUENCY");
  if ( speed < 0 || speed >= NSPEEDS || _status[speed] )
    {
      cout << "caen_calibration: no calibration for speed " << speed << endl;
      return -1;
    }
  _speed = speed;

  for ( int chip = 0; chip < NCHIPS; chip++)
    {
      _samples = p->iValue(chip,"SAMPLES");
      if ( _samples > NCELLS ) _samples = NCELLS;
      const int cell = p->iValue(chip,"INDEXCELL");
      if ( cell < 0 || cell >= NCELLS ) return -1;

      // the time for each chip, as CAEN's ApplyDataCorrection: past cell
      // 1023 the readout wraps around to cell 0, one full turn of the ring later
      const double tstart = timevec(speed, cell, chip);

      for ( int i = 0; i < _samples; i++)
	{
	  if ( i+cell < NCELLS)
	    {
	      _current_time[i*NCHIPS + chip] = timevec(speed, i+cell, chip) - tstart;
	    }
	  else
	    {
	      _current_time[i*NCHIPS + chip] = timevec(speed, i+cell - NCELLS, chip) + ring_period[speed] - tstart;
	    }
	}

      // the adc samples of the 8 channels, and the trigger channel of this chip
      for ( int c = 0; c < 9; c++)
	{
	  const int ch = ( c < 8 ) ? chip*8 + c : 32 + chip;
	  int idx = cell;
	  for ( int i = 0; i < _samples; i++)
	    {
	      _current_wave[i*NCHANNELS + ch] = p->iValue(i, ch) - offset(speed, idx, ch) - nsample(speed, i, ch);
	      idx++;
	      if (idx >= NCELLS) idx = 0;
	    }
	}
    }
  return 0;
}

float caen_calibration::caen_corrected(const int sample, const int channel) const
{
  if ( sample < 0 || sample >= _samples || channel < 0 || channel >= NCHANNELS) return 0;
  return _current_wave[sample*NCHANNELS + channel];
}

float caen_calibration::caen_time(const int sample, const int channel) const
{
  if ( sample < 0 || sample >= _samples || channel < 0 || channel >= NCHANNELS) return 0;
  return _current_time[sample*NCHIPS + (channel < 32 ? channel/8 : channel-32)];
}

float caen_calibration::caen_rollover(const int channel) const
{
  // as in caen_correction, the time of the last cell, at the speed of the last data packet
  if ( channel < 0 || channel >= NCHANNELS ) return 0;
  return timevec(_speed, NCELLS-1, channel < 32 ? channel/8 : channel-32);
}

void caen_calibration::identify(std::ostream& os) const
{
  os << "caen_calibration for unit " << _serial << ", taken for packet " << _readout_id;
  if ( _broken ) os << " ** not usable **";
  os << endl;
}
