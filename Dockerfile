FROM debian:11-slim

ARG DEBIAN_FRONTEND=noninteractive
RUN \
  apt-get update && \
  apt-get install -y \
    bison \
    build-essential \
    cmake \
    flex \
    git \
    protobuf-compiler \
    libgnutls28-dev \
    libarchive-dev \
    libprotobuf-dev \
    libprotoc-dev \
    liblzma-dev \
    zlib1g-dev \
  && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/

COPY . /src
WORKDIR /src
RUN git submodule init && \
    git submodule update && \
    mkdir build && \
    cd build && \
    CC=gcc CXX=g++ cmake -DCMAKE_INSTALL_PREFIX=/usr .. && \
    DESTDIR=build-root make install



FROM debian:11-slim

ARG DEBIAN_FRONTEND=noninteractive
RUN \
  apt-get update && \
  apt-get install -y libprotobuf23 && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/

COPY --from=0 /src/build/build-root/ /

EXPOSE 6969
