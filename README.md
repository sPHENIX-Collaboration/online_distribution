# online_distribution

Software to read and analyze RCDAQ data files:

- `eventlibraries` - the Event/Packet libraries that read RCDAQ data
   and decode the packets, plus the command-line tools `ddump`,
  `dlist`, `dpipe` and others
- `pmonitor` - the ROOT-based analysis and monitoring framework built on them
  (see the [RCDAQ home page](https://www.phenix.bnl.gov/~purschke/rcdaq/) for
  the manuals and example files)

## Requirements

- a C++20 compiler (gcc or clang)
- ROOT, built with C++20
- zlib, bzip2 and LZO (`liblzo2`) with their development headers

## Getting the code

~~~
git clone https://github.com/eic/online_distribution.git
~~~

This is the ePIC fork of online_distribution. It started out as a branch
of the sPHENIX version,
[sPHENIX-Collaboration/online_distribution](https://github.com/sPHENIX-Collaboration/online_distribution),
and keeps its full history. From now on the two are developed separately
and are not merged: they support different devices.

## Building and installing

Everything is installed under one prefix. For a private install area, the
convention is to keep that prefix in `$ONLINE_MAIN`:

~~~
export ONLINE_MAIN=/path/to/install
~~~

The build itself does not need `ONLINE_MAIN`; it only uses the prefix you
give it, so a system-wide install works the same way with `/usr/local` as
the prefix.

The headers go to `<prefix>/include/Event/` and
`<prefix>/include/pmonitor/`, the libraries to `<prefix>/lib`, the
programs to `<prefix>/bin`.

You have the choice of using automake or cmake for the installation.
All of them install the same files.

### 1. CMake, both packages at once

From the top-level directory:

~~~
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$ONLINE_MAIN
cmake --build build -j8
cmake --install build
~~~

### 2. CMake, each package on its own

The eventlibraries first, then pmonitor, which finds them through
`CMAKE_PREFIX_PATH`:

~~~
cmake -S eventlibraries -B build-eventlibraries -DCMAKE_INSTALL_PREFIX=$ONLINE_MAIN
cmake --build build-eventlibraries -j8
cmake --install build-eventlibraries

cmake -S pmonitor -B build-pmonitor -DCMAKE_PREFIX_PATH=$ONLINE_MAIN -DCMAKE_INSTALL_PREFIX=$ONLINE_MAIN
cmake --build build-pmonitor -j8
cmake --install build-pmonitor
~~~

### 3. Automake

As in the RCDAQ manual ([rcdaq_doc.pdf](https://www.phenix.bnl.gov/~purschke/rcdaq/rcdaq_doc.pdf),
"Installation"): the sources and a separate build tree next to each other,
e.g. in `$HOME/softwarerepo`, and `autogen.sh` run from the build directory
(it passes its arguments on to `configure`). The eventlibraries first, then
pmonitor, into the same prefix:

~~~
cd $HOME/softwarerepo
mkdir -p build/online_distribution/{eventlibraries,pmonitor}

cd build/online_distribution/eventlibraries
../../../online_distribution/eventlibraries/autogen.sh --prefix=$ONLINE_MAIN
make install

cd ../pmonitor
../../../online_distribution/pmonitor/autogen.sh --prefix=$ONLINE_MAIN
make install
~~~

CMake builds default to `-O2 -g` (`RelWithDebInfo`), the same as Automake.

## Using the installation

For a private install area:

~~~
export PATH=$ONLINE_MAIN/bin:$PATH
export LD_LIBRARY_PATH=$ONLINE_MAIN/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$ONLINE_MAIN/include:$ONLINE_MAIN/include/Event:$ONLINE_MAIN/include/pmonitor
~~~

For an install in `/usr/local`, the compiler and linker find the headers
and libraries by themselves. Run `ldconfig` (as root) once after installing,
so the programs find the libraries without `LD_LIBRARY_PATH` (Debian and
Ubuntu search `/usr/local/lib`; other distributions may need an entry in
`/etc/ld.so.conf.d`). ROOT still needs to be told where the headers are:

~~~
export ROOT_INCLUDE_PATH=/usr/local/include/Event:/usr/local/include/pmonitor
~~~

Code includes the headers with their directory, e.g. `#include <Event/Event.h>`
and `#include <pmonitor/pmonitor.h>`. `writePmonProject.sh <name>` creates a new
pmonitor project.

For the UDP-based ROC formats (302, 310), whose waveforms are split across
Packets, eventlibraries has pool classes that put them back together; see
[eventlibraries/ROC_pool.md](eventlibraries/ROC_pool.md).

## Changes from the sPHENIX version

- The decoders for the PHENIX and sPHENIX detectors (GL1, LL1, the
  digitizers, MVTX, INTT, TPC and others) are gone.
- The legacy PHENIX data format (PRDF) can no longer be read or written;
  a PRDF file gives a message that the format is not supported. RCDAQ
  writes the RCDAQ format by default.
- The event and packet classes have proper names: `Event_A` (was
  `oncsEvent`), `Packet_A` (was `oncsSubevent`), and `Packet_idXXX` for
  the packet formats (was `oncsSub_idXXX`). Code that uses `Event`,
  `Packet` and the iterators, as user code normally does, is not affected,
  and programs built against the earlier version run without a rebuild.
- `dpipe` always writes the RCDAQ format; its options `-o`, `-z` and `-l`
  are gone for now (compressed writing will come back). `prdf2prdf` is
  gone, and `changeid` and `changehitformat` are not built until they are
  rewritten for the RCDAQ format.
- New: the 64-bit format ID8EVT, the V1742 calibration packet and the
  correction class `caen_calibration`, and the pool classes for the
  formats 302 and 310.
