#ifndef __ROC_POOL_H__
#define __ROC_POOL_H__

#include <cstdint>
#include <deque>
#include <vector>
#include <iostream>

class Packet;

// Base class for "pool" classes of the UDP-datagram based ROC formats
// (302 IDH2GCROC3_10G, 310 IDCALOROC1B, and whatever comes next).
//
// The data area of such a Packet is a concatenation of received UDP
// datagrams, each one prefixed with the 0xaabbccdd marker and the received
// length in bytes. Waveforms are routinely split across Packet (and so
// Event) boundaries. A pool collects the datagrams from consecutive
// Packets and stitches the waveforms back together. To the user it acts
// like a super-sized Packet of the same format, with the same iValue API.
//
// The datagrams are kept in a std::deque, one std::vector per datagram:
// each datagram is contiguous in memory (which is all a format decoder
// needs, since lines and frames never cross a datagram boundary), and
// removing finished datagrams from the head is cheap.
//
// Usage, e.g. in a pmonitor process_event():
//
//   pool.addPacket(p);
//   delete p;
//   while ( pool.depth_ok() )
//     {
//       int nwf = pool.iValue(0, "NR_WF");
//       ... iValue(wf, sample, ch) etc for wf < nwf ...
//       pool.next();
//     }
//
// and at the end of the stream, pool.drain() followed by the same loop.
//
// The water marks count datagrams. Datagrams are collected until the
// high mark is reached, then decoded from the head down to the low mark;
// waveforms that are complete by then are on offer until next(). The
// waveforms still open (no later data seen yet) are kept and continued
// with the next datagrams, so the result does not depend on the marks -
// they only set how many datagrams are decoded in one go.
//
// A pool is locked to one packet id (and its format), by instance() or
// else by the first addPacket(). A Packet with a different id or format
// is refused, so mixing up e.g. 12001 and 12002 cannot go unnoticed.

class ROC_pool
{
public:
  ROC_pool(const int hitformat, const char *format_name,
	   const unsigned int low_mark = 200, const unsigned int high_mark = 600);
  virtual ~ROC_pool() {}

  // the right pool for this Packet's format, locked to its packet id;
  // NULL if there is no pool for that format
  static ROC_pool *instance (Packet *p,
			     const unsigned int low_mark = 200, const unsigned int high_mark = 600);
  
  // returns the number of datagrams taken, -1 if the Packet was refused
  virtual int addPacket(Packet *p);

  // true when there is something to read
  virtual bool depth_ok() const;

  // done with the waveforms on offer, move on
  virtual int next();

  // end of the stream: decode everything and close the open waveforms
  virtual void drain();

  unsigned int depth() const { return _datagrams.size(); }
  unsigned int low_mark() const { return _low_mark; }
  unsigned int high_mark() const { return _high_mark; }

  // bookkeeping
  unsigned long long datagrams_seen() const { return _datagrams_seen; }
  unsigned long long bad_markers() const { return _bad_markers; }
  unsigned long long truncated_datagrams() const { return _truncated; }
  unsigned long long rejected_packets() const { return _rejected; }

  // the packet id this pool is locked to, -1 if not yet
  int getIdentifier() const { return _packet_id; }
  int getHitFormat() const { return _hitformat; }

  // short identification message, like Event and Packet have
  virtual void identify(std::ostream &os = std::cout) const;

  // the calls all formats have, so code can work with any pool
  virtual int iValue(const int n, const char *what) = 0;
  virtual int iValue(const int wf, const int sample, const int ch) = 0;
  virtual int iValue(const int wf, const int sample, const int ch, const char *what) = 0;
  virtual long long lValue(const int wf, const int sample, const int ch) = 0;   // the sample's timestamp

  virtual void dump(std::ostream &os = std::cout) = 0;

protected:

  int decode();

  // format-specific part
  virtual int decode_datagram(const uint32_t *d, const unsigned int nbytes) = 0;
  virtual void close_open() = 0;          // drain: the open waveforms are complete
  virtual void update_offer() = 0;        // after decoding: what is on offer
  virtual void drop_offer() = 0;          // next(): forget what was on offer
  virtual bool has_pending() const = 0;   // decoded data not yet dropped

  struct datagram
  {
    std::vector<uint32_t> w;
    unsigned int nbytes;
  };

  std::deque<datagram> _datagrams;

  int _packet_id;
  const int _hitformat;
  const char *_format_name;

  unsigned int _low_mark;
  unsigned int _high_mark;
  bool _draining;
  bool _is_decoded;

  unsigned long long _datagrams_seen;
  unsigned long long _bad_markers;
  unsigned long long _truncated;
  unsigned long long _rejected;
};

#endif
