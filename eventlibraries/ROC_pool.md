# ROC_pool

Pool classes that put back together the waveforms which the UDP-based ROC
readout formats split across Packets (and so across Events).

Formats covered:

| hit format | name             | pool class          | per-Packet decoder        |
|-----------:|------------------|---------------------|---------------------------|
| 302        | IDH2GCROC3_10G   | `h2gcroc3_10G_pool` | `Packet_idh2gcroc3_10G`  |
| 310        | IDCALOROC1B      | `caloroc1b_pool`    | `Packet_idcaloroc1b`     |

Format 301 is deprecated and not supported.

The pools are part of eventlibraries: the classes are in `libNoRootEvent`
(with the ROOT dictionary in `libEvent`), the headers are installed with
the other Event headers (`#include <Event/ROC_pool.h>`), and `pool_validate`
is installed in `bin/` with the other tools. Nothing extra needs to be
linked or added to `ROOT_INCLUDE_PATH`.

## Why

The data area of these Packets is a list of the UDP datagrams as they were
received, each one starting with the marker `0xaabbccdd` and the received
length in bytes. RCDAQ cuts the stream into Packets at datagram boundaries,
without regard to the waveforms. The first and last waveform of a Packet are
therefore often only pieces, and a per-Packet decoder cannot do anything
about that.

A pool collects the datagrams of consecutive Packets of one packet id and
decodes them as one continuous stream. To the user it looks like a very
large Packet of the same format, with the same `iValue` calls.

## Usage

In a pmonitor project:

    #include <Event/ROC_pool.h>

    static ROC_pool *pool = 0;

    int process_event (Event *e)
    {
      Packet *p = e->getPacket(12001);
      if ( !p ) return 0;

      if ( !pool )
        {
          pool = ROC_pool::instance(p);
          if ( !pool )
            {
              cout << "packet " << p->getIdentifier() << " has format "
                   << p->getHitFormat() << " - no pool for that" << endl;
              delete p;
              return 1;    // non-zero stops the pmonitor event loop
            }
        }

      pool->addPacket(p);
      delete p;

      while ( pool->depth_ok() )
        {
          int nwf = pool->iValue(0, "NR_WF");
          for ( int wf = 0; wf < nwf; wf++ )
            {
              int ns = pool->iValue(wf, "SAMPLESIZE");
              // ... pool->iValue(wf, sample, ch) etc.
            }
          pool->next();
        }
      return 0;
    }

    // at the end of the file or stream, called after prun():
    pool->drain();
    // ... the same while ( pool->depth_ok() ) loop once more

- `ROC_pool::instance(p [, low_mark, high_mark])` makes the right pool for
  the Packet's hit format, or returns NULL if there is none. The pool
  classes can also be constructed directly.
- `addPacket(p)` copies the Packet's datagrams into the pool. The Packet
  can be deleted right after.
- `depth_ok()` is true while there are waveforms on offer.
- `iValue(0, "NR_WF")` is the number of complete waveforms on offer, and
  `wf` runs from 0 to NR_WF-1.
- `next()` drops the waveforms on offer and moves on.
- `drain()` says the stream has ended: everything left, including the last
  waveform, is offered.
- `identify()` prints one line about the pool: packet id, format, fill
  level, datagrams seen, and any problems counted.

### Locked to one packet id

A pool takes Packets of one packet id and one format only. `instance()`
locks it to the Packet it was made from; a directly constructed pool locks
to the first Packet of its format. Any other Packet is refused:
`addPacket()` returns -1 and the pool stays as it was. The first five
refusals are printed, after that they are counted (`rejected_packets()`,
`identify()`). This catches e.g. feeding packet 12002 into the pool for
12001.

### Water marks

The pool collects datagrams until it holds `high_mark` of them (default
600), then decodes from the oldest down to `low_mark` (default 200) and
offers the waveforms that are complete by then. A waveform still open at
that point is kept and continued with the next datagrams. The marks only
set how much is decoded in one go: the waveforms that come out are the same
for any choice of marks (`pool_validate` checks this).

## The calls

All pools:

| call                              | returns                              |
|-----------------------------------|--------------------------------------|
| `iValue(0, "NR_WF")`              | number of waveforms on offer         |
| `iValue(0, "CHANNELS")`           | number of channels                   |
| `iValue(wf, "SAMPLESIZE")`        | samples in waveform wf               |
| `iValue(wf, sample, ch)`          | the ADC value                        |
| `iValue(wf, sample, ch, "what")`  | other per-sample values, see below   |
| `dump(os)`                        | the waveforms on offer, as text      |

Format 302 (`h2gcroc3_10G_pool`), as in the Packet:

- `iValue(wf, sample, ch, what)` with "TOT", "TOA", "TRIGGER_IN",
  "TRIGGER_OUT", "EVENT_COUNTER", and "ERROR": the 3 hamming error bits
  H1 H2 H3 from the ROC header of the half-chip that `ch` is in (H1 = 4,
  header; H2 = 2, CM and channels 0-17; H3 = 1, Calib and channels 18-35;
  0 = no error)
- `lValue(wf, sample, ch)`: the sample's timestamp (all pools have this call)
- sample-indexed calls, counting the samples of all waveforms on offer:
  `iValue(n, what)` with "SAMPLES", "TIMESTAMP", "CMN", "CALIB", and
  `iValue(sample, ch [, "TOT" / "TOA"])`

A new waveform starts when the timestamp jumps by more than 164 clocks.

Format 310 (`caloroc1b_pool`), as in the Packet:

- `iValue(wf, sample, ch, what)` with "ADC1", "ADC2", "TOA", "GAIN"
  (plain `iValue(wf, sample, ch)` is ADC1)
- `lValue(wf, sample, ch)`: the 48-bit global timestamp of the sample (0 if
  the data have none)
- `iValue(link, "LINKPRESENT")`, links 0-11
- channels of all links in one range, link = ch / 18
- `iValue(wf, "SAMPLESIZE")` is the longest of the links' waveforms wf;
  a shorter link reads back zeros past its end

As in the Packet, waveform `wf` is the wf-th waveform of each link. So that
this pairing holds when the links run ahead of each other, a waveform is
only offered once every link seen so far has completed it.

## pool_validate

    pool_validate <file> <packet id>

checks the pools against the per-Packet decoders on a real data file:

1. All waveforms that lie fully inside a Packet (everything but the first
   and last of each Packet) must come out of the pool unchanged and in the
   same order. They are compared through a checksum over everything the
   calls above return.
2. The pool must give identical results for the water marks 200/600,
   0/1, 1/2 and 5000/20000.

It ends with PASSED or FAILED. For 302 it also prints how many waveforms
have each length, Packet by Packet and from the pool.

## Adding a format

Derive from `ROC_pool` and implement the `iValue` calls and `dump()`, plus
the format-specific part:

- `decode_datagram(d, nbytes)`: decode one datagram
- `close_open()`: at `drain()`, the open waveforms are complete
- `update_offer()`: after decoding, decide what is on offer
- `drop_offer()`: at `next()`, forget what was on offer
- `has_pending()`: decoded data that is not yet dropped

Pass the hit format and its name to the `ROC_pool` constructor, add a
`case` for it in `ROC_pool::instance()`, and add the new files to both
`Makefile.am` and `CMakeLists.txt` (sources, installed headers, and the
`event_Dict` header list), and the class to `eventLinkDef.h`.
