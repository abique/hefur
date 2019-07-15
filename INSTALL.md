# Prequisite

Prequisite for Debian 9 :

    apt install libgnutls28-dev libarchive-dev libprotoc-dev protobuf-compiler liblzma-dev bison flex

# Build and install from source

To install hefur from source, do:

    tar xf hefur-${version}.tar.xz &&
    cd hefur-${version} &&
    mkdir build &&
    cd build &&
    CC=gcc CXX=g++ cmake -DCMAKE_INSTALL_PREFIX=/usr ..
    
    # do a test installation into tmp-root/
    DESTDIR=tmp-root make install
    
    # do the real install
    make install

# Build and install from git source

To install hefur from git repo, do:

    git clone https://github.com/abique/hefur.git &&
    cd hefur &&
    git submodule init &&
    git submodule update &&
    mkdir build &&
    cd build &&
    CC=gcc CXX=g++ cmake -DCMAKE_INSTALL_PREFIX=/usr ..

    # do a test installation into tmp-root/
    DESTDIR=tmp-root make install

    # do the real install
    make install

