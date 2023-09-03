cmake_minimum_required(VERSION 3.16)
enable_testing()

project(hefur C CXX)
set(CMAKE_CXX_STANDARD 17)

option(HEFUR_CONTROL_INTERFACE "enables Hefur's control interface and hefurctl" ON)

find_package(GnuTLS REQUIRED)

file(STRINGS VERSION HEFUR_VERSION)

message("hefur version: ${HEFUR_VERSION}")
message("control interface: ${HEFUR_CONTROL_INTERFACE}")

add_definitions(
  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE
  -Wall -Wextra -Werror=return-type
  -DHEFUR_VERSION=\"${HEFUR_VERSION}\"
  ${GNUTLS_DEFINITIONS})

if(HEFUR_CONTROL_INTERFACE)
  add_definitions(-DHEFUR_CONTROL_INTERFACE)
endif()

include(GNUInstallDirs)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(RT_LIB "rt")
else()
  set(RT_LIB "")
endif()

add_subdirectory(mimosa EXCLUDE_FROM_ALL)
include_directories(mimosa)
add_subdirectory(hefur)

install(
  DIRECTORY www/
  DESTINATION ${CMAKE_INSTALL_DATADIR}/hefur/www)

install(
  FILES manual/manual.md
  DESTINATION ${CMAKE_INSTALL_DOCDIR}/hefur/)
