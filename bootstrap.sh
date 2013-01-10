#!/bin/bash

#####
#Dipendence: automake libtool subversion git-core bzr ruby1.8-full cmake zlib1g-dev libcloog-ppl0 wget g++
#####

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd -P`
popd > /dev/null

BOOST_VERSION=1_50_0
LMEM_VERSION=1.0.14
CHAOS_DIR=$SCRIPTPATH
BASE_EXTERNAL=$CHAOS_DIR/external
PREFIX=$CHAOS_DIR/usr/local

if [ -n "$1" ]; then
    PREFIX=$1/usr/local
else
    PREFIX=$CHAOS_DIR/usr/local
fi
if [ -n "$CHAOS32" ]; then
export CFLAGS="-m32 -arch i386"
export CXXFLAGS="-m32 -arch i386"
echo "Force 32 bit binaries"
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
    if [ ! -e "$BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz" ]; then
        echo "Download boost source"
        wget --no-check-certificate -O $BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz "http://downloads.sourceforge.net/project/boost/boost/1.50.0/boost_$BOOST_VERSION.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.51.0%2F&ts=1350734344&use_mirror=freefr"
        
    fi

    if [ ! -e $BASE_EXTERNAL/boost ]; then
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
if [ -n "$CHAOS32" ]; then
    	echo "INSTALLING BOOST X86 32"
    	./b2 cflags=-m32 cxxflags=-m32 architecture=x86 address-model=32 --prefix=$PREFIX link=shared --with-program_options --with-chrono --with-filesystem --with-log --with-regex --with-system --with-thread install
    else
    	./b2 --prefix=$PREFIX link=shared --with-program_options --with-chrono --with-filesystem --with-log --with-regex --with-system --with-thread install
    fi
else
    echo "Boost Already present"
fi


if [ ! -d "$BASE_EXTERNAL/msgpack-c" ]; then
    echo "Install msgpack-c"
    git clone https://github.com/msgpack/msgpack-c.git $BASE_EXTERNAL/msgpack-c
    cd $BASE_EXTERNAL/msgpack-c/
else
    echo "Update msgpack-c"
    cd $BASE_EXTERNAL/msgpack-c/
    git pull
fi
./bootstrap
./configure --prefix=$PREFIX 
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
./configure --prefix=$PREFIX
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
./configure --prefix=$PREFIX --with-mpio=$PREFIX --with-msgpack=$PREFIX
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
./configure --prefix=$PREFIX
make clean
make
make install

if [ ! -d "$PREFIX/include/libmemcached" ]; then
    echo "Install libmemcached into  $BASE_EXTERNAL/libmemcached-1.0.12"
    wget --no-check-certificate -O $BASE_EXTERNAL/libmemcached.tar.gz https://launchpad.net/libmemcached/1.0/$LMEM_VERSION/+download/libmemcached-$LMEM_VERSION.tar.gz
    tar zxvf $BASE_EXTERNAL/libmemcached.tar.gz -C $BASE_EXTERNAL
    cd $BASE_EXTERNAL/libmemcached-$LMEM_VERSION
    ./configure --disable-sasl --prefix=$PREFIX
    make clean
    make
    make install
fi

if [ ! -d "$BASE_EXTERNAL/zeromq3-x" ]; then
echo "Download zmq source"
git clone https://github.com/zeromq/zeromq3-x.git $BASE_EXTERNAL/zeromq3-x
else
echo "Update zmq source"
cd $BASE_EXTERNAL/zeromq3-x
git pull
fi
cd $BASE_EXTERNAL/zeromq3-x
./autogen.sh
./configure --prefix=$PREFIX
make
make install

echo "Compile !CHAOS"
cd $CHAOS_DIR
if [ -n "$CHAOS32" ]; then
cmake -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX -DBUILD_FORCE_32=true -DBUILD_PREFIX=$PREFIX .
else
cmake -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX -DBUILD_PREFIX=$PREFIX .
fi
make
make install
