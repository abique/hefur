cmake_minimum_required(VERSION 2.8)

project(hefur C CXX)

enable_testing()

find_package(GnuTLS REQUIRED)

file(STRINGS VERSION HEFUR_VERSION)
message("hefur version: ${HEFUR_VERSION}")

add_definitions(
  -std=c++11
  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE
  -Wall -Wextra -Werror=return-type
  -DHEFUR_VERSION=\"${HEFUR_VERSION}\"
  ${GNUTLS_DEFINITIONS})

add_subdirectory(mimosa)
include_directories(mimosa)
add_subdirectory(hefur)
