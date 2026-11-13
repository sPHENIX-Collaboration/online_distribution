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

The ePIC version is on the branch `ePIC`:

~~~
git clone -b ePIC https://github.com/sPHENIX-Collaboration/online_distribution.git
~~~

## Building and installing

Everything is installed under one prefix, by convention `$ONLINE_MAIN`:

~~~
export ONLINE_MAIN=/path/to/install
~~~

The headers go to `$ONLINE_MAIN/include/Event/` and
`$ONLINE_MAIN/include/pmonitor/`, the libraries to `$ONLINE_MAIN/lib`, the
programs to `$ONLINE_MAIN/bin`.

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

~~~
export PATH=$ONLINE_MAIN/bin:$PATH
export LD_LIBRARY_PATH=$ONLINE_MAIN/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$ONLINE_MAIN/include:$ONLINE_MAIN/include/Event:$ONLINE_MAIN/include/pmonitor
~~~

Code includes the headers with their directory, e.g. `#include <Event/Event.h>`
and `#include <pmonitor/pmonitor.h>`. `writePmonProject.sh <name>` creates a new
pmonitor project.

For the UDP-based ROC formats (302, 310), whose waveforms are split across
Packets, eventlibraries has pool classes that put them back together; see
[eventlibraries/ROC_pool.md](eventlibraries/ROC_pool.md).
