if(HEFUR_CONTROL_INTERFACE)
  mimosa_protoc(hefur)

  add_library(hefur-proto STATIC hefur.pb.cc)

  add_executable(hefurctl hefurctl.cc)

  target_link_libraries(hefurctl
    hefur-proto
    mimosa-bittorrent
    mimosa-rpc
    mimosa
    ${GNUTLS_LIBRARY}
    pthread
    ${RT_LIB})

  install(TARGETS hefurctl
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

  set(HEFURD_CONTROL_SOURCES
    control-server.cc
    control-server.hh

    control-service.cc
    control-service.hh
    )

endif(HEFUR_CONTROL_INTERFACE)

add_executable(hefurd
  address.cc
  address.hh

  announce-handler.cc
  announce-handler.hh

  announce-request.cc
  announce-request.hh

  announce-response.hh

  crypto-support.hh

  file-handler.cc
  file-handler.hh

  fs-tree-white-list.cc
  fs-tree-white-list.hh

  hefur.cc
  hefur.hh

  http-server.cc
  http-server.hh

  info-hash.hh
  info-hash.cc

  log-handler.cc
  log-handler.hh

  log.cc
  log.hh

  main.cc

  namespace-helper.hh

  options.cc
  options.hh

  peer.hh

  peers-handler.cc
  peers-handler.hh

  scrape-handler.cc
  scrape-handler.hh

  scrape-request.hh

  scrape-response.hh

  stat-handler.cc
  stat-handler.hh

  template-factory.cc
  template-factory.hh

  torrent-db.cc
  torrent-db.hh

  torrent.cc
  torrent.hh

  udp-server.cc
  udp-server.hh

  ${HEFURD_CONTROL_SOURCES})

if(HEFUR_CONTROL_INTERFACE)
  target_link_libraries(hefurd hefur-proto mimosa-rpc)
endif()

target_link_libraries(hefurd
  mimosa-tpl
  mimosa-http
  mimosa-fs
  mimosa-bittorrent
  mimosa-bencode
  mimosa-json
  mimosa
  ${GNUTLS_LIBRARY}
  pthread
  ${RT_LIB})

install(TARGETS hefurd
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hefur)
