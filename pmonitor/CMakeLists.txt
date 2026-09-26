cmake_minimum_required(VERSION 3.10)
project(pmonitor CXX)

# default to -O2 -g, matching Automake's default CXXFLAGS - without this,
# CMake builds with no optimization at all
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

# This coexists with the Automake build (autogen.sh/configure/make) -
# use one or the other, they don't interfere with each other as long
# as you build out-of-source here (cmake -S . -B build).
#
# Requires online_distribution/eventlibraries to already be built and
# installed (its own CMakeLists.txt), same as the Automake build needs
# $ONLINE_MAIN/lib/libEvent.so + libmessage.so.

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Werror -Wno-vexing-parse -Wno-unused-parameter)
endif()

find_package(ROOT REQUIRED)

# ----------------------------------------------------------------------
# eventlibraries (Event, NoRootEvent, message) - external, not a CMake
# package, same find_library/find_path pattern as everywhere else.
# ----------------------------------------------------------------------
find_library(EVENT_LIBRARY Event)
# looking for Event/Event.h (not Event.h with PATH_SUFFIXES Event) so
# EVENT_INCLUDE_DIR ends up as the parent of Event/, matching
# pmonitor.h's #include <Event/Event.h>
find_path(EVENT_INCLUDE_DIR Event/Event.h)
if(NOT EVENT_LIBRARY OR NOT EVENT_INCLUDE_DIR)
  message(FATAL_ERROR "Event (from online_distribution/eventlibraries) not found - "
                       "build and install it first")
endif()

# ----------------------------------------------------------------------
# FROG - optional, same pattern as RunControl's optional GL1/GTM: most
# sites don't have OFFLINE_MAIN's FROG.h, pmonitor.cc guards its use
# with #ifdef HAVE_FROG.
# ----------------------------------------------------------------------
find_path(FROG_INCLUDE_DIR FROG.h HINTS "$ENV{OFFLINE_MAIN}/include")
if(FROG_INCLUDE_DIR)
  message(STATUS "FROG.h found (${FROG_INCLUDE_DIR}) - building pmonitor with FROG support")
else()
  message(STATUS "FROG.h not found - building pmonitor without FROG support")
endif()

# ----------------------------------------------------------------------
# libpmonitor - libpmonitor_la_SOURCES in Makefile.am
# ----------------------------------------------------------------------
ROOT_GENERATE_DICTIONARY(pmonitor_Dict
  pmonitor.h
  pmonstate.h
  LINKDEF pmonitorLinkDef.h
  MODULE pmonitor)

add_library(pmonitor SHARED
  ${CMAKE_CURRENT_BINARY_DIR}/pmonitor_Dict.cxx
  pmonitor.cc
  pMutex.cc)

target_include_directories(pmonitor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE ${EVENT_INCLUDE_DIR})

target_link_libraries(pmonitor PUBLIC ${EVENT_LIBRARY} ROOT::Core ROOT::Thread pthread)
target_link_options(pmonitor PRIVATE -Wl,--no-as-needed)

# mirrors configure.ac's "test for root 6" -> AC_DEFINE(HAVE_ROOT6).
# pmonitor.cc's TThread-based live-update path is gated on this.
if(ROOT_VERSION VERSION_GREATER_EQUAL 6)
  target_compile_definitions(pmonitor PRIVATE HAVE_ROOT6)
endif()

if(FROG_INCLUDE_DIR)
  target_include_directories(pmonitor PRIVATE ${FROG_INCLUDE_DIR})
  target_compile_definitions(pmonitor PRIVATE HAVE_FROG)
endif()

install(TARGETS pmonitor LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/libpmonitor_rdict.pcm
  ${CMAKE_CURRENT_BINARY_DIR}/libpmonitor.rootmap
  DESTINATION ${CMAKE_INSTALL_LIBDIR})
# headers go under include/pmonitor/ (Automake's pkgincludedir, from
# AC_INIT(pmonitor,...)), matching #include <pmonitor/pmonitor.h> as used
# by writePmonProject.pl's template - same convention as eventlibraries'
# include/Event/
install(FILES pmonitor.h pmonstate.h pMutex.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pmonitor)
install(PROGRAMS writePmonProject.pl DESTINATION ${CMAKE_INSTALL_BINDIR})
