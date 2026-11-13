// Self-check for the pool classes on a real data file.
//
//   pool_validate <file> <packet id>
//
// 1) Reference: every Packet decoded on its own, with the per-Packet
//    decoder. The first and last waveform of each Packet may be split
//    pieces; all others ("interior") must show up unchanged in the pool.
// 2) The pool must give identical results for very different water marks
//    (they only set how much is decoded at a time).
//
// Waveforms are compared through a hash over everything the public API
// returns, read the same way from the Packet and from the pool.

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

#include "Event.h"
#include "fileEventiterator.h"
#include "packet.h"

#include "h2gcroc3_10G_pool.h"
#include "caloroc1b_pool.h"

using namespace std;

struct hasher
{
  uint64_t h = 1469598103934665603ULL;
  void add(const long long v)
  {
    for ( int i = 0; i < 8; i++)
      {
	h ^= (v >> (8*i)) & 0xff;
	h *= 1099511628211ULL;
      }
  }
};

// ---- format 302: one hash per waveform, plus its sample count
struct wf302
{
  uint64_t hash;
  int samples;
  long long first_ts;
};

template <class T>
wf302 hash302(T *src, const int wf)
{
  hasher h;
  wf302 r;
  r.samples = src->iValue(wf, "SAMPLESIZE");
  r.first_ts = src->lValue(wf, 0, 0);
  h.add(r.samples);
  for ( int s = 0; s < r.samples; s++)
    {
      h.add(src->lValue(wf, s, 0));
      h.add(src->iValue(wf, s, 0, "TRIGGER_IN"));
      h.add(src->iValue(wf, s, 0, "TRIGGER_OUT"));
      h.add(src->iValue(wf, s, 0, "EVENT_COUNTER"));
      for ( int ch = 0; ch < 72 * 4; ch++)
	{
	  h.add(src->iValue(wf, s, ch));
	  h.add(src->iValue(wf, s, ch, "TOT"));
	  h.add(src->iValue(wf, s, ch, "TOA"));
	  h.add(src->iValue(wf, s, ch, "ERROR"));
	}
    }
  r.hash = h.h;
  return r;
}

// ---- format 310: one hash per link and waveform. SAMPLESIZE is the
// longest link's count, so the shorter links read back trailing zeros;
// those are trimmed (identically on both sides) to get the link's own
// waveform. Returns samples = 0 if the link has nothing at this wf.
struct wf310
{
  uint64_t hash;
  int samples;
};

template <class T>
wf310 hash310(T *src, const int wf, const int link)
{
  int ss = src->iValue(wf, "SAMPLESIZE");
  vector<vector<int>> v(ss);
  int last = -1;
  for ( int s = 0; s < ss; s++)
    {
      for ( int c = 0; c < 18; c++)
	{
	  int ch = link * 18 + c;
	  v[s].push_back(src->iValue(wf, s, ch, "ADC1"));
	  v[s].push_back(src->iValue(wf, s, ch, "ADC2"));
	  v[s].push_back(src->iValue(wf, s, ch, "TOA"));
	  v[s].push_back(src->iValue(wf, s, ch, "GAIN"));
	}
      // the global timestamp (one per link and sample), split into two ints
      long long ts = src->lValue(wf, s, link * 18);
      v[s].push_back((int) (ts >> 32));
      v[s].push_back((int) (ts & 0xffffffff));
      for ( int x : v[s]) if ( x) last = s;
    }
  hasher h;
  wf310 r;
  r.samples = last + 1;
  h.add(link);
  h.add(r.samples);
  for ( int s = 0; s <= last; s++)
    {
      for ( int x : v[s]) h.add(x);
    }
  r.hash = h.h;
  return r;
}

// reference interior waveforms in order, must be found in the pool's list
// in the same order
template <class W>
void match(const vector<W> &ref, const vector<W> &pool, long &found, long &missing)
{
  size_t j = 0;
  for ( const W &r : ref)
    {
      size_t k = j;
      while ( k < pool.size() && pool[k].hash != r.hash) k++;
      if ( k < pool.size())
	{
	  found++;
	  j = k + 1;
	}
      else
	{
	  missing++;
	}
    }
}

struct result
{
  vector<wf302> wf302s;
  vector<wf310> wf310s[caloroc1b_pool::NUM_LINKS];
  long datagrams = 0;
  long bad_markers = 0;
  long truncated = 0;
  long max_offer = 0;
  long batches = 0;
};

// runs the pool over the whole file
result run_pool(const char *file, const int id, const int format,
		const unsigned int low, const unsigned int high)
{
  result r;
  ROC_pool *pool = 0;
  h2gcroc3_10G_pool *p302 = 0;
  caloroc1b_pool *p310 = 0;

  auto harvest = [&]()
  {
    while ( pool->depth_ok())
      {
	if ( p302)
	  {
	    int n = p302->iValue(0, "NR_WF");
	    if ( n > r.max_offer) r.max_offer = n;
	    for ( int wf = 0; wf < n; wf++) r.wf302s.push_back(hash302(p302, wf));
	  }
	else
	  {
	    int n = p310->iValue(0, "NR_WF");
	    if ( n > r.max_offer) r.max_offer = n;
	    for ( int wf = 0; wf < n; wf++)
	      {
		for ( int link = 0; link < caloroc1b_pool::NUM_LINKS; link++)
		  {
		    wf310 w = hash310(p310, wf, link);
		    if ( w.samples) r.wf310s[link].push_back(w);
		  }
	      }
	  }
	r.batches++;
	pool->next();
      }
  };

  int status;
  fileEventiterator it(file, status);
  Event *e;
  while ( (e = it.getNextEvent()) )
    {
      Packet *p = e->getPacket(id);
      if ( p)
	{
	  if ( !pool)
	    {
	      pool = ROC_pool::instance(p, low, high);
	      p302 = dynamic_cast<h2gcroc3_10G_pool *>(pool);
	      p310 = dynamic_cast<caloroc1b_pool *>(pool);
	    }
	  pool->addPacket(p);
	  delete p;
	  harvest();
	}
      delete e;
    }
  pool->drain();
  harvest();

  r.datagrams = pool->datagrams_seen();
  r.bad_markers = pool->bad_markers();
  r.truncated = pool->truncated_datagrams();
  delete pool;
  return r;
}

int main(int argc, char *argv[])
{
  if ( argc < 3)
    {
      cerr << "usage: " << argv[0] << " <file> <packet id>" << endl;
      return 2;
    }
  const char *file = argv[1];
  const int id = atoi(argv[2]);

  // ---- reference pass, Packet by Packet
  int status;
  fileEventiterator *it = new fileEventiterator(file, status);
  if ( status)
    {
      cerr << "cannot open " << file << endl;
      return 2;
    }

  int format = 0;
  long packets = 0;
  long ref_wf = 0;
  long ref_samples = 0;
  vector<wf302> ref302;
  map<int,long> ref_sizes;
  vector<wf310> ref310[caloroc1b_pool::NUM_LINKS];

  Event *e;
  while ( (e = it->getNextEvent()) )
    {
      Packet *p = e->getPacket(id);
      if ( p)
	{
	  if ( !format) format = p->getHitFormat();
	  if ( p->getHitFormat() == format)
	    {
	      packets++;
	      int n = p->iValue(0, "NR_WF");
	      ref_wf += n;
	      if ( format == 302)
		{
		  for ( int wf = 0; wf < n; wf++)
		    {
		      ref_samples += p->iValue(wf, "SAMPLESIZE");
		      ref_sizes[p->iValue(wf, "SAMPLESIZE")]++;
		      if ( wf > 0 && wf < n - 1) ref302.push_back(hash302(p, wf));
		    }
		}
	      else if ( format == 310)
		{
		  for ( int link = 0; link < caloroc1b_pool::NUM_LINKS; link++)
		    {
		      vector<wf310> l;
		      for ( int wf = 0; wf < n; wf++)
			{
			  wf310 w = hash310(p, wf, link);
			  if ( w.samples) l.push_back(w);
			}
		      for ( size_t k = 0; k < l.size(); k++) ref_samples += l[k].samples;
		      for ( size_t k = 1; k + 1 < l.size(); k++) ref310[link].push_back(l[k]);
		    }
		}
	    }
	  delete p;
	}
      delete e;
    }
  delete it;

  if ( format != 302 && format != 310)
    {
      cerr << "packet " << id << ": format " << format << " - no pool for that" << endl;
      return 2;
    }

  cout << "file " << file << "  packet " << id << "  format " << format << endl;
  cout << "Packets: " << packets << "   waveforms Packet by Packet: " << ref_wf
       << "   samples: " << ref_samples << endl;

  // ---- the pool, with very different water marks
  const unsigned int marks[][2] = { {200, 600}, {0, 1}, {1, 2}, {5000, 20000} };
  const int nmarks = sizeof(marks) / sizeof(marks[0]);

  result first;
  int failures = 0;

  for ( int m = 0; m < nmarks; m++)
    {
      result r = run_pool(file, id, format, marks[m][0], marks[m][1]);

      long found = 0, missing = 0, wfs = 0, samples = 0, interior = 0;
      if ( format == 302)
	{
	  match(ref302, r.wf302s, found, missing);
	  wfs = r.wf302s.size();
	  for ( auto &w : r.wf302s) samples += w.samples;
	  interior = ref302.size();
	}
      else
	{
	  for ( int link = 0; link < caloroc1b_pool::NUM_LINKS; link++)
	    {
	      match(ref310[link], r.wf310s[link], found, missing);
	      wfs += r.wf310s[link].size();
	      for ( auto &w : r.wf310s[link]) samples += w.samples;
	      interior += ref310[link].size();
	    }
	}

      cout << endl << "pool low/high mark " << marks[m][0] << "/" << marks[m][1] << ":" << endl;
      cout << "  datagrams " << r.datagrams << "  bad markers " << r.bad_markers
	   << "  truncated " << r.truncated << "  batches " << r.batches
	   << "  max waveforms on offer " << r.max_offer << endl;
      cout << "  waveforms " << wfs << (format == 310 ? " (link-waveforms)" : "")
	   << "  samples " << samples << endl;
      cout << "  interior Packet waveforms found in pool: " << found << " of " << interior
	   << "  missing: " << missing << endl;

      if ( missing) failures++;

      if ( m == 0)
	{
	  first = r;
	  if ( format == 302)
	    {
	      map<int,long> pool_sizes;
	      for ( auto &w : r.wf302s) pool_sizes[w.samples]++;
	      cout << "  waveform length (samples): count Packet by Packet / pool" << endl;
	      map<int,long> all = ref_sizes;
	      for ( auto &x : pool_sizes) all[x.first] += 0;
	      for ( auto &x : all)
		cout << "    " << x.first << ": " << ref_sizes[x.first] << " / " << pool_sizes[x.first] << endl;
	      // where in the stream are the pool waveforms of unusual length
	      int common = 0;
	      for ( auto &x : pool_sizes) if ( x.second > pool_sizes[common]) common = x.first;
	      for ( size_t k = 0; k < r.wf302s.size(); k++)
		if ( r.wf302s[k].samples != common)
		  cout << "    pool waveform " << k << " of " << r.wf302s.size() << " has " << r.wf302s[k].samples << " samples" << endl;
	    }
	}
      else
	{
	  bool same = true;
	  if ( format == 302)
	    {
	      same = (r.wf302s.size() == first.wf302s.size());
	      for ( size_t k = 0; same && k < r.wf302s.size(); k++)
		same = (r.wf302s[k].hash == first.wf302s[k].hash);
	    }
	  else
	    {
	      for ( int link = 0; link < caloroc1b_pool::NUM_LINKS; link++)
		{
		  same = same && (r.wf310s[link].size() == first.wf310s[link].size());
		  for ( size_t k = 0; same && k < r.wf310s[link].size(); k++)
		    same = (r.wf310s[link][k].hash == first.wf310s[link][k].hash);
		}
	    }
	  cout << "  identical to the 200/600 result: " << (same ? "yes" : "NO") << endl;
	  if ( !same) failures++;
	}
    }

  cout << endl << (failures ? "FAILED" : "PASSED") << endl;
  return failures ? 1 : 0;
}
