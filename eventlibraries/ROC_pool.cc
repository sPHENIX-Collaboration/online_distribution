#include "ROC_pool.h"

#include <cstring>
#include <iomanip>
#include "packet.h"
#include "oncsSubConstants.h"
#include "h2gcroc3_10G_pool.h"
#include "caloroc1b_pool.h"

using namespace std;

ROC_pool::ROC_pool(const int hitformat, const char *format_name,
		   const unsigned int low_mark, const unsigned int high_mark)
  : _hitformat(hitformat)
  , _format_name(format_name)
{
  _packet_id = -1;
  _low_mark = low_mark;
  _high_mark = high_mark;
  if ( _high_mark <= _low_mark)
    {
      _high_mark = _low_mark + 1;
      cout << " ROC_pool: high mark must be above the low mark, set to " << _high_mark << endl;
    }
  _draining = false;
  _is_decoded = false;
  _datagrams_seen = 0;
  _bad_markers = 0;
  _truncated = 0;
  _rejected = 0;
}

ROC_pool *ROC_pool::instance(Packet *p, const unsigned int low_mark, const unsigned int high_mark)
{
  if ( !p) return NULL;

  ROC_pool *pool;
  switch ( p->getHitFormat() )
    {
    case IDH2GCROC3_10G:
      pool = new h2gcroc3_10G_pool(low_mark, high_mark);
      break;
    case IDCALOROC1B:
      pool = new caloroc1b_pool(low_mark, high_mark);
      break;
    default:
      return NULL;
    }
  pool->_packet_id = p->getIdentifier();
  return pool;
}

void ROC_pool::identify(std::ostream &os) const
{
  os << "ROC_pool for Packet ";
  if ( _packet_id < 0) os << "(none yet)";
  else os << setw(5) << _packet_id;
  os << " " << setw(3) << _hitformat << " (" << _format_name << ")"
     << "  depth " << _datagrams.size() << " datagrams (low/high mark "
     << _low_mark << "/" << _high_mark << ")  seen " << _datagrams_seen;
  if ( _bad_markers) os << "  bad markers " << _bad_markers;
  if ( _truncated)   os << "  truncated " << _truncated;
  if ( _rejected)    os << "  refused Packets " << _rejected;
  if ( _draining)    os << "  draining";
  os << endl;
}

int ROC_pool::addPacket(Packet *p)
{
  if ( !p) return 0;

  // the first Packet locks the id, unless instance() already did
  if ( _packet_id < 0 && p->getHitFormat() == _hitformat) _packet_id = p->getIdentifier();

  if ( p->getIdentifier() != _packet_id || p->getHitFormat() != _hitformat)
    {
      _rejected++;
      // complain loudly, but don't flood the screen if it happens every event
      if ( _rejected <= 5)
	{
	  cerr << " ROC_pool for Packet ";
	  if ( _packet_id < 0) cerr << "(none yet)";
	  else cerr << _packet_id;
	  cerr << " (" << _format_name << "): refusing Packet " << p->getIdentifier() << " format " << p->getHitFormat();
	  if ( _rejected == 5) cerr << " - further ones are only counted";
	  cerr << endl;
	}
      return -1;
    }

  int l = p->getDataLength();
  if ( l <= 0) return 0;

  vector<int> buffer(l);
  int nw = 0;
  p->fillIntArray(buffer.data(), l, &nw, "DATA");

  const uint8_t *b = (const uint8_t *) buffer.data();
  const size_t nbytes = 4 * (size_t) nw;
  size_t pos = 0;
  int n = 0;

  // the datagrams are packed back to back on byte boundaries (a 302
  // datagram is not a multiple of 4 bytes long)
  while ( pos + 8 <= nbytes)
    {
      uint32_t marker, length;
      memcpy(&marker, b + pos, 4);
      memcpy(&length, b + pos + 4, 4);

      if ( marker != 0xaabbccdd)
	{
	  // the zeroes at the end are padding; anything else is not
	  for ( size_t i = pos; i < nbytes; i++)
	    {
	      if ( b[i])
		{
		  _bad_markers++;
		  break;
		}
	    }
	  break;
	}

      pos += 8;
      if ( pos + length > nbytes)
	{
	  length = nbytes - pos;
	  _truncated++;
	}

      datagram dg;
      dg.nbytes = length;
      dg.w.resize( (length + 3) / 4, 0);
      memcpy(dg.w.data(), b + pos, length);
      _datagrams.push_back(std::move(dg));

      pos += length;
      n++;
    }

  _datagrams_seen += n;
  return n;
}

bool ROC_pool::depth_ok() const
{
  if ( _draining) return ( !_datagrams.empty() || has_pending() );
  return ( _datagrams.size() >= _high_mark);
}

int ROC_pool::decode()
{
  if ( _is_decoded) return 0;
  _is_decoded = true;

  const unsigned int keep = _draining ? 0 : _low_mark;

  while ( _datagrams.size() > keep)
    {
      const datagram &dg = _datagrams.front();
      decode_datagram(dg.w.data(), dg.nbytes);
      _datagrams.pop_front();
    }

  if ( _draining) close_open();

  update_offer();
  return 0;
}

int ROC_pool::next()
{
  decode();   // in case nobody looked
  drop_offer();
  _is_decoded = false;
  return 0;
}

void ROC_pool::drain()
{
  _draining = true;
  _is_decoded = false;
}
