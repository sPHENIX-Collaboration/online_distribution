cmake_minimum_required(VERSION 3.14)
project(online_distribution CXX)

# Builds and installs the eventlibraries and pmonitor in one go:
#
#   cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$ONLINE_MAIN
#   cmake --build build -j8
#   cmake --install build
#
# Each of the two can still be built on its own with CMake (from its own
# directory), or with Automake.

add_subdirectory(eventlibraries)
add_subdirectory(pmonitor)
