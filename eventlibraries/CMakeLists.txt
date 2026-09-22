cmake_minimum_required(VERSION 3.10)
project(eventlibraries CXX)

# This coexists with the Automake build (autogen.sh/configure/make) -
# use one or the other, they don't interfere with each other as long
# as you build out-of-source here (cmake -S . -B build).
#
# This builds the non-ROOT event/packet library (NoRootEvent), its
# message-handling dependency (message), and the bin_PROGRAMS command
# line tools from Makefile.am. The ROOT-dictionary libraries (Event,
# Rootmessage) are not built here; they still build fine via autotools
# if needed.
#
# Known gap carried over from Makefile.am: oncsSub_idmvtxv0-3.cc/h and
# mvtx_decoder/ are not part of allsources there either, so they are
# not built here.

include(GNUInstallDirs)
include(CheckCXXSourceCompiles)
include(CheckIncludeFileCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # mirrors configure.ac's `CPPFLAGS="$CPPFLAGS -DLinux"` for the
  # *-*-linux* host case
  add_compile_definitions(Linux)
endif()

# mirrors configure.ac's AC_CHECK_HEADERS(getopt.h) - under Automake
# this ends up in $(DEFS) and applies project-wide, which is what
# eventClient.cc's #ifdef HAVE_GETOPT_H (no #else) relies on.
check_include_file_cxx(getopt.h HAVE_GETOPT_H)
if(HAVE_GETOPT_H)
  add_compile_definitions(HAVE_GETOPT_H)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(
    -Wall -Werror
    -Wno-overloaded-virtual
    -Wno-class-memaccess
    -Wno-unused-parameter
    -Wno-type-limits
    -Wno-unused-but-set-parameter
    -Wno-unused-but-set-variable
    -Wno-deprecated-declarations)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wno-unknown-warning-option)
endif()

# ----------------------------------------------------------------------
# generated header - mirrors the Makefile.am NEWIO branch, which is
# what any modern (non-Windows) compiler takes: event_io.h relies on
# this to define STREAMBUF_NEW_IOSTREAM and pick the <iostream> path
# instead of the ancient <iostream.h>.
# ----------------------------------------------------------------------
set(IOSELECT_H ${CMAKE_CURRENT_BINARY_DIR}/ioselect.h)
file(WRITE ${IOSELECT_H}
"// *** this is a generated file. Do not commit, do not edit\n\
#ifndef __IOSELECT_H__\n\
#define __IOSELECT_H__\n\
\n\
#define STREAMBUF_NEW_IOSTREAM\n\
\n\
#endif\n")

# ----------------------------------------------------------------------
# third-party compression libs used by gzbuffer/lzobuffer/bz2buffer
# ----------------------------------------------------------------------
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

find_path(LZO_INCLUDE_DIR lzo/lzoutil.h)
find_library(LZO_LIBRARY lzo2)
if(NOT LZO_INCLUDE_DIR OR NOT LZO_LIBRARY)
  message(FATAL_ERROR "liblzo2 (lzo/lzoutil.h + liblzo2) not found")
endif()

# ----------------------------------------------------------------------
# mirrors configure.ac's "whether cout accepts a streambuf arg" check -
# filter_msg_buffer.cc/remote_msg_buffer.cc use this to pick between
# COUT.rdbuf(x) (works) and COUT = (STREAMBUF*)x (removed from modern
# libstdc++'s std::ostream::operator=).
# ----------------------------------------------------------------------
check_cxx_source_compiles("
#include <iostream>
int main() {
  std::streambuf *x = std::cout.rdbuf();
  std::cout.rdbuf(x);
}" RDBUF_ACCEPTS_STREAMBUF)

# ----------------------------------------------------------------------
# libmessage - message_la_SOURCES in Makefile.am
# ----------------------------------------------------------------------
add_library(message SHARED
  date_filter_msg_buffer.cc
  filter_msg_buffer.cc
  remote_msg_buffer.cc
  msg_buffer.cc
  msg_control.cc
  strnstr.cc)
target_include_directories(message PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(RDBUF_ACCEPTS_STREAMBUF)
  target_compile_definitions(message PRIVATE RDBUF_ACCEPTS_STREAMBUF)
endif()

# ----------------------------------------------------------------------
# libNoRootEvent - allsources in Makefile.am
# ----------------------------------------------------------------------
add_library(NoRootEvent SHARED
  Cframe.C
  CframeV1.C
  Cpacket.C
  CpacketV1.C
  dataBlock.C
  errorBlock.C
  fakeFrame.C
  formatError.C
  formatIO.C
  framePackets.C
  frameRoutines.C
  packetRoutines.C
  A_Event.cc
  buffer.cc
  prdfBuffer.cc
  decoding_routines.cc
  evt_mnemonic.cc
  simpleRandom.cc
  eventReceiverClient.cc
  testEventiterator.cc
  fileEventiterator.cc
  listEventiterator.cc
  md5.cc
  PHmd5Utils.cc
  PHmd5Value.cc
  gzbuffer.cc
  lzobuffer.cc
  bz2buffer.cc
  oEvent.cc
  ospEvent.cc
  oBuffer.cc
  ophBuffer.cc
  ogzBuffer.cc
  olzoBuffer.cc
  ospBuffer.cc
  oamlBuffer.cc
  oncsBuffer.cc
  oncsEvent.cc
  oncsEventiterator.cc
  rcdaqEventiterator.cc
  oncsSub_idcstr.cc
  oncsSub_id2evt.cc
  oncsSub_id4evt.cc
  oncsSub_idrtclock.cc
  oncsSub_idsis3300.cc
  oncsSub_idsis3300r.cc
  oncsSub_idcaenv792.cc
  oncsSub_idcaenv1742.cc
  oncsSub_idbspetdata.cc
  oncsSub_idrcpetdata.cc
  oncsSub_iduppetdata.cc
  oncsSub_iduppetdata_v104.cc
  oncsSub_iduppetparams.cc
  oncsSub_idsrs_v01.cc
  oncsSub_idfnalmwpc.cc
  oncsSub_idfnalmwpcv2.cc
  oncsSub_iddrs4v1.cc
  oncsSub_idgl1v0.cc
  oncsSub_idgl1v1.cc
  oncsSub_idh2gcroc3.cc
  oncsSub_idh2gcroc3_10G.cc
  oncsSub_idastrov1.cc
  oncsSubevent.cc
  oncs_mnemonic.cc
  packet_A.cc
  packet_gl1.cc
  packet_gl1p.cc
  packet_gl1_evclocks.cc
  packet_gl1psum.cc
  packet_idcstr.cc
  packet_id2evt.cc
  packet_id4evt.cc
  packet_id4scaler.cc
  packet_iddigitizerv2.cc
  packet_iddigitizerv3.cc
  packet_idll1v1.cc
  packet_idll1v2.cc
  packet_mnemonic.cc
  packet_w124.cc
  packet_cdevpolarimeter.cc
  packet_cdevir.cc
  packet_cdevwcm.cc
  packet_cdevbpm.cc
  packet_cdevmadch.cc
  packet_cdevpoltarget.cc
  packet_starscaler.cc
  packet_hbd_fpga.cc
  packet_hbd_fpgashort.cc
  packet_fvtx_dcm0.cc
  packet_cdevring.cc
  packet_cdevringnopol.cc
  packet_cdevbuckets.cc
  packet_cdevdvm.cc
  caen_correction.cc
  ${IOSELECT_H})

target_include_directories(NoRootEvent PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE ${LZO_INCLUDE_DIR})

target_link_libraries(NoRootEvent PUBLIC message
  PRIVATE ZLIB::ZLIB BZip2::BZip2 ${LZO_LIBRARY})

# ----------------------------------------------------------------------
# bin_PROGRAMS in Makefile.am
# ----------------------------------------------------------------------
find_package(Boost REQUIRED)  # header-only use: algorithm/string.hpp, lexical_cast.hpp

function(add_eventlib_tool name)
  add_executable(${name} ${name}.cc)
  target_link_libraries(${name} PRIVATE NoRootEvent dl)
endfunction()

add_eventlib_tool(dpipe)
add_eventlib_tool(dlist)
add_eventlib_tool(changeid)
add_eventlib_tool(changehitformat)
add_eventlib_tool(eventcombiner)
add_eventlib_tool(prdf2prdf)
add_eventlib_tool(prdfcheck)
add_eventlib_tool(prdfsplit)
add_eventlib_tool(eventClient)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_options(dpipe PRIVATE -Wl,-export-dynamic)
endif()

add_executable(ddump ddump.cc)
target_link_libraries(ddump PRIVATE NoRootEvent dl Boost::boost)

add_executable(lastEvent lastEvent.cc)
target_link_libraries(lastEvent PRIVATE NoRootEvent dl Boost::boost)

add_executable(eventServer eventServer.cc)
target_link_libraries(eventServer PRIVATE NoRootEvent dl pthread)

add_executable(eventServer_classic eventServer_classic.cc)
target_link_libraries(eventServer_classic PRIVATE NoRootEvent dl pthread)

set(EVENTLIB_TOOLS
  dpipe dlist changeid changehitformat eventcombiner prdf2prdf
  prdfcheck prdfsplit eventClient ddump lastEvent eventServer
  eventServer_classic)

# ----------------------------------------------------------------------
# install - mirrors pkginclude_HEADERS (installedheaders + msgheaders)
# ----------------------------------------------------------------------
set(EVENTLIB_INSTALL_HEADERS
  generalDefinitions.h event_io.h Event.h A_Event.h oncsEvent.h
  oncsEvtConstants.h oncsEvtStructures.h oncsStructures.h
  oncsSubConstants.h oncsSubevent.h EventTypes.h Eventiterator.h
  EvtConstants.h EvtStructures.h BufferConstants.h phenixTypes.h
  phenixOnline.h testEventiterator.h fileEventiterator.h
  listEventiterator.h dpipe_filter.h buffer.h simpleRandom.h
  eventReceiverClient.h gzbuffer.h lzobuffer.h bz2buffer.h prdfBuffer.h
  oBuffer.h ophBuffer.h ospBuffer.h oEvent.h ospEvent.h ogzBuffer.h
  olzoBuffer.h oamlBuffer.h oncsBuffer.h oncsEventiterator.h
  rcdaqEventiterator.h packet.h packetConstants.h
  packet_hbd_fpgashort.h packet_fvtx_dcm0.h packet_w124.h packet_A.h
  packetPublic.h packetV1Public.h errorBlock.h packetRoutines.h
  decoding_routines.h packetHeaders.h Cpacket.h packetHdr.h
  packetHdrV1.h formatError.h dataBlock.h framePackets.h framePublic.h
  frameV1Public.h caen_correction.h md5.h
  msg_control.h msg_profile.h msg_buffer.h date_filter_msg_buffer.h
  filter_msg_buffer.h remote_msg_buffer.h strnstr.h)

install(TARGETS NoRootEvent message ${EVENTLIB_TOOLS}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${EVENTLIB_INSTALL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${IOSELECT_H} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
