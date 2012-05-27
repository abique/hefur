add_executable(hefurd
  announce-handler.cc
  announce-request.cc
  fs-tree-white-list.cc
  hefur.cc
  http-server.cc
  log.cc
  main.cc
  options.cc
  peer.cc
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
  gnutls
  pthread
  rt
)

install(TARGETS hefurd
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
