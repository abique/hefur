add_executable(hefurd
  announce-handler.cc
  announce-request.cc
  fs-tree-white-list.cc
  hefur.cc
  http-server.cc
  log.cc
  main.cc
  peer.cc
  scrape-handler.cc
  torrent-db.cc
  udp-server.cc
)

target_link_libraries(hefurd
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
