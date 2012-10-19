#!/bin/bash

#####
#Dipendence: autotools, automake, libtool, git-core, bzr, ruby, cmake, zlib1g-dev
#####

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd -P`
popd > /dev/null


CHAOS_DIR=$SCRIPTPATH
BASE_EXTERNAL=$CHAOS_DIR/external
PREFIX=$CHAOS_DIR/usr/local

echo "Using $BASE_EXTERNAL as external library folder"
echo "Using $CHAOS_DIR as chaos folder"


if [ ! -d "$BASE_EXTERNAL/boost-log" ]; then
    echo "Download boost-log source"
    svn co https://boost-log.svn.sourceforge.net/svnroot/boost-log/branches/v1 $BASE_EXTERNAL/boost-log
else
    echo "Update boost-log source"
    cd $BASE_EXTERNAL/boost-log
    svn update
fi

if [[ $? -ne 0 ]] ; then
    exit 1
fi

if [ ! -d "$BASE_EXTERNAL/boost" ]; then
    echo "Download boost source"
    svn co http://svn.boost.org/svn/boost/trunk/ $BASE_EXTERNAL/boost
else
    echo "Update boost source"
    cd $BASE_EXTERNAL/boost
    svn update
fi

if [[ $? -ne 0 ]] ; then
    exit 1
fi

if [ ! -L "$BASE_EXTERNAL/boost/boost/log" ]; then
    echo "link boost/log into boos source"
    ln -s $BASE_EXTERNAL/boost-log/boost/log $BASE_EXTERNAL/boost/boost/
    if [[ $? -ne 0 ]] ; then
        exit 1
    fi
fi

if [ ! -L "$BASE_EXTERNAL/boost/libs/log" ]; then
    echo "link libs/log into boos source"
    ln -s $BASE_EXTERNAL/boost-log/libs/log $BASE_EXTERNAL/boost/libs/
    if [[ $? -ne 0 ]] ; then
        exit 1
    fi
fi

if [ ! -f "$BASE_EXTERNAL/boost/b2" ]; then
    echo "Boostrapping boost"
    cd $BASE_EXTERNAL/boost
    ./bootstrap.sh

    if [[ $? -ne 0 ]] ; then
        exit 1
    fi

fi

echo "Compile and isntall boost libraries into $BASE_EXTERNAL"
./b2 --prefix=$PREFIX --with-atomic --with-chrono --with-filesystem --with-log --with-regex --with-system --with-thread install

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

if [ ! -f "$PREFIX/include/event.h" ]; then
    echo "Installing LibEvent"
    git clone git://levent.git.sourceforge.net/gitroot/levent/libevent $BASE_EXTERNAL/libevent
    cd $BASE_EXTERNAL/libevent
    ./autogen.sh
    ./configure --prefix=$PREFIX
    make
    make install
fi

if [ ! -d "$PREFIX/include/libmemcached" ]; then
    echo "Install libmemcached into  $BASE_EXTERNAL/libmemcached-1.0.12"
    wget --no-check-certificate -O $BASE_EXTERNAL/libmemcached.tar.gz https://launchpad.net/libmemcached/1.0/1.0.12/+download/libmemcached-1.0.12.tar.gz
    tar zxvf $BASE_EXTERNAL/libmemcached.tar.gz -C $BASE_EXTERNAL
    cd $BASE_EXTERNAL/libmemcached-1.0.12
    ./configure --without-memcached --prefix=$PREFIX
    make
    make install
fi

cd
echo "Compile !CHOAS"
cd $CHAOS_DIR
cmake -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX .
make
make install