cmake_minimum_required(VERSION 2.8)

project(hefur C CXX)

enable_testing()

find_package(GnuTLS REQUIRED)

file(STRINGS VERSION HEFUR_VERSION)
message("hefur version: ${HEFUR_VERSION}")

add_definitions(
  -std=c++0x -fsplit-stack
  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE
  -Wall -Wextra -Werror=return-type
  -DHEFUR_VERSION=\"${HEFUR_VERSION}\"
  ${GNUTLS_DEFINITIONS})

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(hefur)
