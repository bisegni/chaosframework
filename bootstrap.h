#!/bin/bash
export BASE_EXTERNAL=$1
echo "Using $BASE_EXTERNAL as external library folder"
export PREFIX=$BASE_EXTERNAL/usr/local
if [ ! -d "$PREFIX/include/msgpack" ]; then

    echo "Install masgpack"
    git clone https://github.com/msgpack/msgpack.git $BASE_EXTERNAL/msgpack
    cd $BASE_EXTERNAL/msgpack/cpp
    
    ./bootstrap
    ./configure --prefix=$PREFIX
    make
    make install
fi

if [ ! -d "$PREFIX/include/mp" ]; then

     echo "Install mpio"
    git clone https://github.com/frsyuki/mpio.git $BASE_EXTERNAL/mpio
    cd $BASE_EXTERNAL/mpio
    
    ./bootstrap 
    ./configure --prefix=$PREFIX
    make 
    make install
fi

if [ ! -d "$PREFIX/include/msgpack/rpc" ]; then
    echo "Install msgpack-rpc"
    git clone https://github.com/msgpack/msgpack-rpc.git $BASE_EXTERNAL/msgpack-rpc
    cd $BASE_EXTERNAL/msgpack-rpc/cpp
    ./bootstrap
    ./configure --prefix=$PREFIX --with-mpio=$PREFIX
    make
    make install
fi

if [ ! -d "$PREFIX/include/event.h" ]; then
    echo "Installing LibEvent"
    git clone git://levent.git.sourceforge.net/gitroot/levent/libevent $BASE_EXTERNAL/libevent
    cd $BASE_EXTERNAL/libevent
    ./autogen.sh
    ./configure --prefix=$PREFIX
    make
    make install
fi

    echo "Install libmemcached"
    bzr branch lp:libmemcached  $BASE_EXTERNAL/libmemcached
    cd $BASE_EXTERNAL/libmemcached
    ./bootstrap.sh
    make
    make install
