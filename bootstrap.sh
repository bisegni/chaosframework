#!/bin/bash

#####
#Dipendence: automake libtool subversion git-core bzr ruby1.8-full cmake zlib1g-dev libcloog-ppl0 wget g++
#####

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd -P`
popd > /dev/null

BOOST_VERSION=1_50_0
LMEM_VERSION=1.0.12
CHAOS_DIR=$SCRIPTPATH
BASE_EXTERNAL=$CHAOS_DIR/external
PREFIX=$CHAOS_DIR/usr/local

if [ -n "$1" ]; then
    PREFIX=$1/usr/local
else
    PREFIX=$CHAOS_DIR/usr/local
fi

echo "Using $CHAOS_DIR as chaos folder"
echo "Using $BASE_EXTERNAL as external library folder"
echo "Using $PREFIX as prefix folder"

if [ ! -d "$BASE_EXTERNAL/boost-log" ]; then
 echo "Download boost-log source"
 svn co https://boost-log.svn.sourceforge.net/svnroot/boost-log/branches/v1 $BASE_EXTERNAL/boost-log
else
 echo "Update boost-log source"
 cd $BASE_EXTERNAL/boost-log
 svn update
fi

if [ ! -d "$PREFIX/include/boost" ]; then
    if [ ! -d "$BASE_EXTERNAL/boost" ]; then
        echo "Download boost source"
        wget --no-check-certificate -O $BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz "http://downloads.sourceforge.net/project/boost/boost/1.50.0/boost_$BOOST_VERSION.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.51.0%2F&ts=1350734344&use_mirror=freefr"
        tar zxvf $BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz -C $BASE_EXTERNAL
        mv $BASE_EXTERNAL/boost_$BOOST_VERSION $BASE_EXTERNAL/boost
    fi


    if [ ! -L "$BASE_EXTERNAL/boost/boost/log" ]; then
        echo "link boost/log into boost source"
        ln -s $BASE_EXTERNAL/boost-log/boost/log $BASE_EXTERNAL/boost/boost/
        if [[ $? -ne 0 ]] ; then
            exit 1
        fi
    fi

    if [ ! -L "$BASE_EXTERNAL/boost/libs/log" ]; then
        echo "link libs/log into boost source"
        ln -s $BASE_EXTERNAL/boost-log/libs/log $BASE_EXTERNAL/boost/libs/
        if [[ $? -ne 0 ]] ; then
            exit 1
        fi
    fi

    if [ ! -f "$BASE_EXTERNAL/boost/b2" ]; then
        echo "Boostrapping boost"
        cd $BASE_EXTERNAL/boost
        ./bootstrap.sh
    fi

    cd $BASE_EXTERNAL/boost
    echo "Compile and install boost libraries into $PREFIX/"
    ./b2 --prefix=$PREFIX link=shared --with-program_options --with-chrono --with-filesystem --with-log --with-regex --with-system --with-thread install
else
    echo "Boost Already present"
fi


if [ ! -d "$BASE_EXTERNAL/msgpack" ]; then
    echo "Install masgpack"
    git clone https://github.com/msgpack/msgpack.git $BASE_EXTERNAL/msgpack
    cd $BASE_EXTERNAL/msgpack/cpp
else
    echo "Update masgpack"
    cd $BASE_EXTERNAL/msgpack/cpp
    git pull
fi
./bootstrap
./configure --prefix=$PREFIX --enable-static=NO --enable-shared=YES
make clean
make
make install

if [ ! -d "$BASE_EXTERNAL/mpio" ]; then
    echo "Install mpio"
    git clone https://github.com/frsyuki/mpio.git $BASE_EXTERNAL/mpio
    cd $BASE_EXTERNAL/mpio
else
    echo "Update mpio"
    cd $BASE_EXTERNAL/mpio
    git pull
fi
./bootstrap
./configure --prefix=$PREFIX --enable-static=NO --enable-shared=YES
make clean
make
make install

if [ ! -d "$BASE_EXTERNAL/msgpack-rpc" ]; then
    echo "Install msgpack-rpc"
    git clone https://github.com/msgpack/msgpack-rpc.git $BASE_EXTERNAL/msgpack-rpc
    cd $BASE_EXTERNAL/msgpack-rpc/cpp
else
    echo "Update msgpack-rpc"
    cd $BASE_EXTERNAL/msgpack-rpc/
    git pull
    cd $BASE_EXTERNAL/msgpack-rpc/cpp
fi
./bootstrap
./configure --prefix=$PREFIX --with-mpio=$PREFIX --with-msgpack=$PREFIX --enable-static=NO --enable-shared=YES
make clean
make
make install


if [ ! -f "$BASE_EXTERNAL/libevent" ]; then
    echo "Installing LibEvent"
    git clone git://levent.git.sourceforge.net/gitroot/levent/libevent $BASE_EXTERNAL/libevent
    cd $BASE_EXTERNAL/libevent
else
    cd $BASE_EXTERNAL/libevent
    git pull
fi
./autogen.sh
./configure --prefix=$PREFIX --enable-static=NO --enable-shared=YES
make clean
make
make install

if [ ! -d "$PREFIX/include/libmemcached" ]; then
    echo "Install libmemcached into  $BASE_EXTERNAL/libmemcached-1.0.12"
    wget --no-check-certificate -O $BASE_EXTERNAL/libmemcached.tar.gz https://launchpad.net/libmemcached/1.0/$LMEM_VERSION/+download/libmemcached-$LMEM_VERSION.tar.gz
    tar zxvf $BASE_EXTERNAL/libmemcached.tar.gz -C $BASE_EXTERNAL
    cd $BASE_EXTERNAL/libmemcached-$LMEM_VERSION
    ./configure --disable-sasl --prefix=$PREFIX --enable-static=NO --enable-shared=YES
    make clean
    make
    make install
fi

cd
echo "Compile !CHOAS"
cd $CHAOS_DIR
cmake -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX .
make
make install
