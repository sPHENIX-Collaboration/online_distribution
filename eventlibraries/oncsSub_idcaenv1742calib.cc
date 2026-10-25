#include "oncsSub_idcaenv1742calib.h"

#include <string.h>
#include <string>
#include <iomanip>

// the layout, see daq_device_CAENcalibration.h in the CAENdrs plugin:
// a header of HEADERLENGTH words, then for each speed the speed code and
// a status word, followed for each chip by NCHANNELS x NCELLS words (low 16
// bits the cell offset, high 16 bits the nsample correction, channel-major)
// and NCELLS words with the cell time as a float.

static const int HEADERLENGTH = 26;

oncsSub_idcaenv1742calib::oncsSub_idcaenv1742calib(subevtdata_ptr data)
  :oncsSubevent_w4 (data){}

int oncsSub_idcaenv1742calib::header(const int i) const
{
  return (&SubeventHdr->data)[i];
}

// number of words we have, and the dimensions in the header must fit
int oncsSub_idcaenv1742calib::valid() const
{
  int n = getLength() - SEVTHEADERLENGTH - getPadding();
  if ( n < HEADERLENGTH ) return 0;
  if ( header(0) != 1 ) return 0;  // format version
  int speeds = header(4);
  int chips = header(5);
  int channels = header(6);
  int cells = header(7);
  if ( speeds <= 0 || chips <= 0 || channels <= 0 || cells <= 0) return 0;
  if ( n < HEADERLENGTH + speeds * ( 2 + chips * (channels + 1) * cells) ) return 0;
  return 1;
}

static void chip_and_channel(const int ch, const int chips, int &chip, int &c)
{
  // 0-31 = chip*8 + channel, then one trigger channel per chip
  const int perchip = 8;
  if ( ch < chips * perchip )
    {
      chip = ch / perchip;
      c = ch % perchip;
    }
  else
    {
      chip = ch - chips * perchip;
      c = perchip;
    }
}

int *oncsSub_idcaenv1742calib::word(const int cell, const int ch, const int speed) const
{
  if ( !valid() ) return 0;
  int speeds = header(4);
  int chips = header(5);
  int channels = header(6);
  int cells = header(7);
  if ( speed < 0 || speed >= speeds || cell < 0 || cell >= cells || ch < 0 || ch >= chips * channels) return 0;

  int chip, c;
  chip_and_channel(ch, chips, chip, c);

  int *d = &SubeventHdr->data + HEADERLENGTH
    + speed * ( 2 + chips * (channels + 1) * cells)
    + 2
    + chip * (channels + 1) * cells;
  return d + c * cells + cell;
}

int *oncsSub_idcaenv1742calib::timeword(const int cell, const int ch, const int speed) const
{
  int *w = word(0, ch, speed);   // checks everything except the cell
  if ( !w || cell < 0 || cell >= header(7) ) return 0;
  int chip, c;
  chip_and_channel(ch, header(5), chip, c);
  int channels = header(6);
  int cells = header(7);
  // back to the start of this chip, then past its channels
  return w - c * cells + channels * cells + cell;
}

int oncsSub_idcaenv1742calib::iValue(const int cell, const int ch, const int speed, const char *what)
{
  int *w = word(cell, ch, speed);
  if ( !w ) return 0;
  if ( strcmp(what, "OFFSET") == 0) return (short) (*w & 0xffff);
  if ( strcmp(what, "NSAMPLE") == 0) return (short) ((*w >> 16) & 0xffff);
  return 0;
}

double oncsSub_idcaenv1742calib::dValue(const int cell, const int ch, const int speed)
{
  int *w = timeword(cell, ch, speed);
  if ( !w ) return 0;
  float t;
  memcpy(&t, w, sizeof(float));
  return t;
}

int oncsSub_idcaenv1742calib::iValue(const int n, const char *what)
{
  if ( !valid() ) return 0;

  if ( strcmp(what, "VERSION") == 0) return header(0);
  if ( strcmp(what, "SERIAL") == 0) return header(1);
  if ( strcmp(what, "READOUTID") == 0) return header(2);
  if ( strcmp(what, "MODEL") == 0) return header(3);
  if ( strcmp(what, "SPEEDS") == 0) return header(4);
  if ( strcmp(what, "CHANNELS") == 0) return header(5) * header(6);
  if ( strcmp(what, "CELLS") == 0) return header(7);

  if ( strcmp(what, "STATUS") == 0)
    {
      if ( n < 0 || n >= header(4) ) return -1;
      int chips = header(5);
      int channels = header(6);
      int cells = header(7);
      return header( HEADERLENGTH + n * ( 2 + chips * (channels + 1) * cells) + 1);
    }

  return 0;
}

std::string oncsSub_idcaenv1742calib::text(const int first, const int nwords) const
{
  const char *c = (const char *) (&SubeventHdr->data + first);
  return std::string(c, strnlen(c, nwords * 4));
}

void  oncsSub_idcaenv1742calib::dump ( OSTREAM& os )
{
  identify(os);

  if ( !valid() )
    {
      os << "  ** malformed calibration packet **" << std::endl;
      return;
    }

  os << "Version:       " << iValue(0,"VERSION") << std::endl;
  os << "Model:         " << text(8, 3) << " (" << iValue(0,"MODEL") << ")" << std::endl;
  os << "Serial Number: " << iValue(0,"SERIAL") << std::endl;
  os << "Firmware:      " << text(11, 5) << " / " << text(16, 10) << std::endl;
  os << "For packet:    " << iValue(0,"READOUTID") << std::endl;
  os << "Speeds:        " << iValue(0,"SPEEDS") << "  Channels: " << iValue(0,"CHANNELS")
     << "  Cells: " << iValue(0,"CELLS") << std::endl;

  const std::streamsize oldprecision = os.precision();
  const char *speedname[] = { "5GS", "2.5GS", "1GS" };
  const int chips = header(5);
  const int channels = header(6);

  for ( int s = 0; s < iValue(0,"SPEEDS"); s++)
    {
      os << std::endl;
      os << "Speed " << s << " (" << (s < 3 ? speedname[s] : "?") << ")  status " << iValue(s,"STATUS") << std::endl;

      // the same columns as the calib_<serial>_*.dat files: cell, the 9 cell
      // offsets of the chip, the cell time, the 9 nsample corrections
      for ( int chip = 0; chip < chips; chip++)
	{
	  os << " chip " << chip << std::endl;
	  for ( int cell = 0; cell < iValue(0,"CELLS"); cell++)
	    {
	      os << std::setw(4) << cell << "  ";
	      for ( int c = 0; c < channels; c++)
		{
		  int ch = ( c < 8 ) ? chip * 8 + c : chips * 8 + chip;
		  os << std::setw(6) << iValue(cell, ch, s, "OFFSET");
		}
	      os << "  " << std::setw(9) << std::fixed << std::setprecision(4)
		 << dValue(cell, chip * 8, s) << "  ";
	      os.unsetf(std::ios::fixed);
	      for ( int c = 0; c < channels; c++)
		{
		  int ch = ( c < 8 ) ? chip * 8 + c : chips * 8 + chip;
		  os << std::setw(6) << iValue(cell, ch, s, "NSAMPLE");
		}
	      os << std::endl;
	    }
	}
    }
  os.precision(oldprecision);
  os << std::endl;
}
