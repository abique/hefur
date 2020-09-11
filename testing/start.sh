#! /bin/bash

exec ../cmake-builds/code/hefur/hefurd \
  -torrent-dir torrents \
  -www-dir ../www \
  -control-socket control \
  -log-color \
  -log-level debug
