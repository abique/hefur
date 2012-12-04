add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/namespace-helper.hh)

mimosa_protoc(hefur)

add_library(hefur-proto hefur.pb.cc)

add_executable(hefurd
  address.cc
  announce-handler.cc
  announce-request.cc
  control-server.cc
  control-service.cc
  file-handler.cc
  fs-tree-white-list.cc
  hefur.cc
  http-server.cc
  log.cc
  main.cc
  options.cc
  peer.cc
  peers-handler.cc
  scrape-handler.cc
  stat-handler.cc
  template-factory.cc
  torrent.cc
  torrent-db.cc
  udp-server.cc
)

target_link_libraries(hefurd
  hefur-proto
  mimosa-rpc
  mimosa-tpl
  mimosa-http
  mimosa-fs
  mimosa-bittorrent
  mimosa-bencode
  mimosa
  ${GNUTLS_LIBRARY}
  pthread
  rt
)

add_executable(hefurctl
  hefurctl.cc)

target_link_libraries(hefurctl
  hefur-proto
  mimosa-bittorrent
  mimosa-rpc
  mimosa
  ${GNUTLS_LIBRARY}
  pthread
  rt
)

install(TARGETS hefurd hefurctl
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
