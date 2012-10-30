add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/namespace-helper.hh)

add_executable(hefurd
  address.cc
  announce-handler.cc
  announce-request.cc
  control-server.cc
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
  mimosa-tpl
  mimosa-http
  mimosa-fs
  mimosa-bencode
  mimosa
  ${GNUTLS_LIBRARY}
  pthread
  rt
)

install(TARGETS hefurd
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
