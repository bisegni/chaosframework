#!/bin/bash

#####
#Dipendence: automake libtool subversion git-core bzr ruby1.8-full cmake zlib1g-dev libcloog-ppl0 wget g++
#####

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd -P`
popd > /dev/null

OS=$(uname -s)
ARCH=$(uname -m)
KERNEL_VER=$(uname -r)
KERNEL_SHORT_VER=$(uname -r|cut -d\- -f1|tr -d '.'| tr -d '[A-Z][a-z]')

if [ -n "$CHAOS_BOOST_VERSION" ]; then
	BOOST_VERSION="1_"$CHAOS_BOOST_VERSION"_0"
	BOOST_VERSION_IN_PATH="1.$CHAOS_BOOST_VERSION.0"
else
	BOOST_VERSION=1_53_0
	BOOST_VERSION_IN_PATH=1.53.0
fi

BOOST_NUMBER_VERSION=$(echo $BOOST_VERSION_IN_PATH |sed "s/[^0-9]//g" )

if [ ! -n "$ZLIB_VERSION" ]; then
    ZLIB_VERSION=1.2.8
fi;

if [ ! -n "$LMEM_VERSION" ]; then
    LMEM_VERSION=1.0.16
fi;
if [ ! -n "$ZMQ_VERSION" ]; then
    ZMQ_VERSION=zeromq4-x
fi;

if [ ! -n "$COUCHBASE_VERSION" ]; then
    COUCHBASE_VERSION=2.3.0
fi;


if [ -n "$1" ]; then
    PREFIX=$1/usr/local
else
    PREFIX=$CHAOS_DIR/usr/local
fi
CROSS_HOST_CONFIGURE=""
if [ -n "$CROSS_HOST" ]; then
CROSS_HOST_CONFIGURE="-host=$CROSS_HOST"
fi

do_make() {
   if !(make -j$NPROC); then
       echo "## error compiling $1"
       exit 1
   fi
   if !(make install); then
       echo "## error installing $1"
       exit 1
   fi

}
if [ -n "$CHAOS32" ]; then
    export CFLAGS="-m32"
    export CXXFLAGS="-m32"
    echo "Force 32 bit binaries"
fi

if [ -n "$CHAOS_DEVELOPMENT" ]; then
	export COMP_TYPE=" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS_DEBUG=-DDEBUG=1 "
#CHAOS_DIR=$SCRIPTPATH/../
	echo "Setup for chaos development folder structure"
#echo "Shared libray prefix -> $CHAOS_DIR"
else
	export COMP_TYPE=" -DCMAKE_BUILD_TYPE=Release "
fi

if [ `echo $OS | tr '[:upper:]' '[:lower:]'` = `echo "Darwin" | tr '[:upper:]' '[:lower:]'` ] && [ $KERNEL_SHORT_VER -ge 1300 ]; then
    export CC=clang
    export CXXFLAGS="-stdlib=libstdc++"
    ## for msgpack
    export CXX="clang++ $CXXFLAGS"

    export LD=clang
    
#    LMEM_VERSION=1.0.16
fi

CHAOS_DIR=$SCRIPTPATH
BASE_EXTERNAL=$CHAOS_DIR/external
PREFIX=$CHAOS_DIR/usr/local

echo "Operating system version: $OS"
echo "Current architecture: $ARCH"
echo "Current kernel version: $KERNEL_VER"
echo "Current short kernel version: $KERNEL_SHORT_VER"
echo "Using $CHAOS_DIR as chaos folder"
echo "Using $BASE_EXTERNAL as external library folder"
echo "Using $PREFIX as prefix folder"
echo "Using BOOST version $BOOST_VERSION/$BOOST_VERSION_IN_PATH ($BOOST_NUMBER_VERSION)"
echo "Compilation type -> $COMP_TYPE"

if [ ! -d "$BASE_EXTERNAL" ]; then
    mkdir -p $BASE_EXTERNAL
fi

if [ ! -e "$PREFIX/include/zlib.h" ] || [ ! -e "$PREFIX/lib/libz.a" ]; then

	if !( wget --no-check-certificate -O "$BASE_EXTERNAL/zlib-$ZLIB_VERSION.tar.gz" "http://zlib.net/zlib-$ZLIB_VERSION.tar.gz" ); then
	    echo "## cannot download http://zlib.net/zlib-$ZLIB_VERSION.tar.gz, aborting "
	    exit 1;
	fi
	if [ -e "$BASE_EXTERNAL/zlib-$ZLIB_VERSION.tar.gz" ]; then
	    filetar="$BASE_EXTERNAL/zlib-$ZLIB_VERSION.tar.gz";
	    if !( tar xvfz $filetar -C "$BASE_EXTERNAL" > /dev/null ) then
		echo "## cannot extract $filetar, aborting "
		exit 1
	    fi
	else
	    echo "## cannot compile $BASE_EXTERNAL/zlib-$ZLIB_VERSION.tar.gz, aborting"
	    exit 1
	fi

if [ -d "$BASE_EXTERNAL/zlib-$ZLIB_VERSION" ]; then
	    cd $BASE_EXTERNAL/zlib-$ZLIB_VERSION
	    echo "entering in $BASE_EXTERNAL/zlib-$ZLIB_VERSION"
	    echo "using $CC and $CXX"
	    ./configure --prefix=$PREFIX 
	    do_make "ZLIB"
else
    echo "$BASE_EXTERNAL/zlib-$ZLIB_VERSION not found"
    exit 1
fi
fi


if [ ! -d "$PREFIX/include/boost" ]; then
    if [ ! -e "$BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz" ]; then
        echo "Download boost source"
        if !( wget --no-check-certificate -O $BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz "http://downloads.sourceforge.net/project/boost/boost/$BOOST_VERSION_IN_PATH/boost_$BOOST_VERSION.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F$BOOST_VERSION_IN_PATH%2F&ts=1350734344&use_mirror=freefr" ); then
	    echo "## cannot download boost_$BOOST_VERSION.tar.gz"
	    exit 1
	 fi
        
    fi

    if [ ! -e $BASE_EXTERNAL/boost ]; then
        tar zxvf $BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz -C $BASE_EXTERNAL
        mv $BASE_EXTERNAL/boost_$BOOST_VERSION $BASE_EXTERNAL/boost
    fi
    
#install old version of boost log
    if [ $BOOST_NUMBER_VERSION -le 1530 ] && [ ! -d "$BASE_EXTERNAL/boost_log" ]; then
	
	if !(git clone https://cvs.lnf.infn.it/boost_log $BASE_EXTERNAL/boost_log); then
	    echo "## cannot git clone  https://cvs.lnf.infn.it/boost_log"
	    exit 1
	fi
	
	if [ ! -d "$BASE_EXTERNAL/boost/boost/log" ]; then
	    echo "link $BASE_EXTERNAL/boost/boost/log -> $BASE_EXTERNAL/boost_log/boost/log"
	    ln -s $BASE_EXTERNAL/boost_log/boost/log $BASE_EXTERNAL/boost/boost/log
	fi
	
	if [ ! -d "$BASE_EXTERNAL/boost/libs/log" ]; then
	    echo "link $BASE_EXTERNAL/boost/libs/log -> $BASE_EXTERNAL/boost_log/libs/log"
	    ln -s $BASE_EXTERNAL/boost_log/libs/log $BASE_EXTERNAL/boost/libs/log
	fi
    fi


    echo "Boostrapping boost"
    cd $BASE_EXTERNAL/boost
    if !( ./bootstrap.sh ); then
	echo "## cannot bootstrap boost"
	exit 1;
    fi

    echo "-> $CROSS_HOST" 
    if [ -n "$CROSS_HOST" ]; then
	echo "* Patching project-config.jam to cross compile for $CROSS_HOST"
	sed -i .bak -e "s/using gcc/using gcc : arm : $CROSS_HOST-g++/" project-config.jam 
    fi
    

    cd $BASE_EXTERNAL/boost
    echo "Compile and install boost libraries into $PREFIX/"

    if [ -n "$CHAOS32" ]; then
    	echo "INSTALLING BOOST 32"
    	./b2 link=shared cflags=-m32 cxxflags=-m32 address-model=32 --prefix=$PREFIX --with-iostreams --with-program_options --with-chrono --with-random --with-filesystem --with-log --with-regex --with-system --with-thread --with-atomic --with-timer install
    else
        if [ `echo $OS | tr [:upper:] [:lower:]` = `echo "Darwin" | tr [:upper:] [:lower:]` ] && [ $KERNEL_SHORT_VER -ge 1300 ]; then
            ./b2  toolset=clang cxxflags=-stdlib=libstdc++ linkflags=-stdlib=libstdc++ link=shared --prefix=$PREFIX --with-program_options --with-random --with-chrono --with-filesystem --with-iostreams --with-log --with-regex --with-system --with-thread --with-atomic --with-timer install
        else
            ./b2  link=shared --prefix=$PREFIX --with-program_options --with-chrono --with-filesystem --with-iostreams --with-log --with-regex --with-random --with-system --with-thread --with-atomic --with-timer install
        fi
    fi
else
    echo "Boost Already present"
fi

### install libmodbus
if [ ! -d "$PREFIX/include/modbus" ] || [ ! -d "$BASE_EXTERNAL/libmodbus" ]; then
        echo "Setup libmodbus library"
        if [ ! -d "$BASE_EXTERNAL/libmodbus" ]; then
                echo "Install libmodbus"
                git clone -b v3.0.5 https://github.com/stephane/libmodbus.git $BASE_EXTERNAL/libmodbus
                cd $BASE_EXTERNAL/libmodbus
        else
                echo "Update libmodbus"
                cd $BASE_EXTERNAL/libmodbus/
                git pull
        fi

./autogen.sh
./configure --prefix=$PREFIX $CROSS_HOST_CONFIGURE


make clean
do_make "MODBUS"

echo "libmodbus done"
fi

if [ ! -d "$PREFIX/include/msgpack" ]; then
	echo "Setup MSGPACK"
	if [ ! -d "$BASE_EXTERNAL/msgpack-c" ]; then
		echo "Install msgpack-c"
		if !(git clone https://github.com/msgpack/msgpack-c.git $BASE_EXTERNAL/msgpack-c); then
		    echo "## cannot clone https://github.com/msgpack/msgpack-c.git"
		    exit 1
		fi
	
	else
		echo "Update msgpack-c"
		cd $BASE_EXTERNAL/msgpack-c/
		git pull
	fi
cd $BASE_EXTERNAL/msgpack-c/
./bootstrap
./configure $CROSS_HOST_CONFIGURE --prefix=$PREFIX 

make clean
do_make "MSG PACK"
echo "MSGPACK done"
fi

if [ ! -d "$PREFIX/include/mp" ]; then
	echo "Setup MPIO"
	if [ ! -d "$BASE_EXTERNAL/mpio" ]; then
		echo "Install mpio"
	#    git clone https://github.com/frsyuki/mpio.git $BASE_EXTERNAL/mpio
		if !(git clone https://github.com/bisegni/mpio.git $BASE_EXTERNAL/mpio); then
		    echo "## cannot git clone https://github.com/bisegni/mpio.git "
		    exit 1
		fi
		cd $BASE_EXTERNAL/mpio
	else
		echo "Update mpio"
		cd $BASE_EXTERNAL/mpio
		git pull
	fi
	./bootstrap
	if [ `echo $OS | tr "[:upper:]" "[:lower:]"` = `echo "linux" | tr "[:upper:]" "[:lower:]"` ] && [ $KERNEL_SHORT_VER -le 2625 ]; then
         ./configure --prefix=$PREFIX $CROSS_HOST_CONFIGURE
	else
         ./configure --disable-timerfd --disable-signalfd --prefix=$PREFIX $CROSS_HOST_CONFIGURE
	fi
	make clean
	do_make "MPIO"
	echo "MPIO done"
fi

if [ ! -d "$PREFIX/include/msgpack/rpc" ]; then
	echo "Setup MSGPACK-RPC"
	if [ ! -d "$BASE_EXTERNAL/msgpack-rpc" ]; then
		echo "Install msgpack-rpc"
#git clone https://github.com/msgpack-rpc/msgpack-rpc-cpp.git $BASE_EXTERNAL/msgpack-rpc
		if !(git clone https://github.com/bisegni/msgpack-rpc-cpp.git $BASE_EXTERNAL/msgpack-rpc); then
		    echo "## cannot git clone  https://github.com/bisegni/msgpack-rpc-cpp.git"
		    exit 1
   		fi
		cd $BASE_EXTERNAL/msgpack-rpc
	else
		echo "Update msgpack-rpc"
		cd $BASE_EXTERNAL/msgpack-rpc/
		git pull
		cd $BASE_EXTERNAL/msgpack-rpc/
	fi
	./bootstrap
	./configure --prefix=$PREFIX --with-mpio=$PREFIX --with-msgpack=$PREFIX $CROSS_HOST_CONFIGURE
	make clean
	do_make "MSGPACK-RPC"

	echo "MSGPACK-RPC done"
fi

echo "Setup LIBEVENT"
if [ ! -d "$PREFIX/include/event2" ]; then
	if [ ! -f "$BASE_EXTERNAL/libevent" ]; then
		echo "Installing LibEvent"
	#    git clone git://levent.git.sourceforge.net/gitroot/levent/libevent $BASE_EXTERNAL/libevent
		if !(git clone http://git.code.sf.net/p/levent/libevent $BASE_EXTERNAL/libevent); then
		    echo "## cannot clone http://git.code.sf.net/p/levent/libevent"
		    exit 1
		fi
		cd $BASE_EXTERNAL/libevent
	else
		cd $BASE_EXTERNAL/libevent
		git pull
	fi
	./autogen.sh
	./configure --prefix=$PREFIX $CROSS_HOST_CONFIGURE
	make clean
	do_make "LIBEVENT"
	echo "LIBEVENT done"
fi

if [ ! -f "$PREFIX/include/uv.h" ]; then
	echo "Setup LIBUV"
	if [ ! -d "$BASE_EXTERNAL/libuv" ]; then
		echo "Installing LIBUV"
		if !(git clone https://github.com/joyent/libuv.git $BASE_EXTERNAL/libuv); then
		    echo "## cannot git clone https://github.com/joyent/libuv.git"
		    exit 1
		fi
		cd $BASE_EXTERNAL/libuv
	else
		cd $BASE_EXTERNAL/libuv
		git pull
	fi
./autogen.sh
./configure --prefix=$PREFIX $CROSS_HOST_CONFIGURE
make clean
do_make "LIBUV"
echo "LIBUV done"
fi

echo "Setup Couchbase sdk"
if [ ! -f "$PREFIX/include/libcouchbase/couchbase.h" ]; then
if [ ! -f "$BASE_EXTERNAL/libcouchbase-$COUCHBASE_VERSION" ]; then
	echo "Download couchabse source"
	if !(wget --no-check-certificate -O $BASE_EXTERNAL/libcouchbase-$COUCHBASE_VERSION.tar.gz http://packages.couchbase.com/clients/c/libcouchbase-$COUCHBASE_VERSION.tar.gz); then
	    echo "## cannot wget http://packages.couchbase.com/clients/c/libcouchbase-$COUCHBASE_VERSION.tar.gz"
	    exit 1
	fi
	tar zxvf $BASE_EXTERNAL/libcouchbase-$COUCHBASE_VERSION.tar.gz -C $BASE_EXTERNAL
fi
cd $BASE_EXTERNAL/libcouchbase-$COUCHBASE_VERSION
./configure --prefix=$PREFIX --disable-couchbasemock --disable-plugins $CROSS_HOST_CONFIGURE
do_make "COUCHBASE"
echo "Couchbase done"
fi

echo "Setup MongoDB client"
if [ ! -f "$PREFIX/include/mongo/client/dbclient.h" ]; then
	if [ ! -f "$BASE_EXTERNAL/mongo" ]; then
		echo "Download mongodb client"
		if !(git clone https://github.com/mongodb/mongo-cxx-driver.git $BASE_EXTERNAL/mongo); then
		    echo "## cannnot git clone https://github.com/mongodb/mongo-cxx-driver.git"
		    exit 1
		fi
		cd $BASE_EXTERNAL/mongo
	else
		cd $BASE_EXTERNAL/mongo
		git pull
	fi

if !(scons --prefix=$PREFIX --libpath=$PREFIX/lib --cxx="$CXX" --cc="$CC" --cpppath=$PREFIX/include --extrapath=$PREFIX install-mongoclient); then
    echo "## error scons configuration of mongo failed, maybe you miss scons package"
    exit 1
fi
echo "Mongodb done"
fi

echo "Setup LIBMEMCACHED"
if [ ! -d "$PREFIX/include/libmemcached" ]; then
    echo "Install libmemcached into  $BASE_EXTERNAL/libmemcached"
    if !(wget --no-check-certificate -O $BASE_EXTERNAL/libmemcached.tar.gz https://launchpad.net/libmemcached/1.0/$LMEM_VERSION/+download/libmemcached-$LMEM_VERSION.tar.gz); then
      echo "## cannot wget  https://launchpad.net/libmemcached/1.0/$LMEM_VERSION/+download/libmemcached-$LMEM_VERSION.tar.gz"
      exit 1
    fi

    tar zxvf $BASE_EXTERNAL/libmemcached.tar.gz -C $BASE_EXTERNAL
    cd $BASE_EXTERNAL/libmemcached-$LMEM_VERSION
    ./configure --without-memcached --disable-sasl --prefix=$PREFIX $CROSS_HOST_CONFIGURE
    ## use standard types instead cinttypes that generates troubles in ARM annd clang
    echo "patchin memcached.h to use the correct cinttypes"
    sed -i .bak -e "s/include <cinttypes>/include <tr1\/cinttypes>/" libmemcached-1.0/memcached.h

    make clean
    do_make "LIBMEMCACHED"
fi
echo "Libmemcached done"

echo "Setup ZMQ"
if [ ! -f "$PREFIX/include/zmq.h" ]; then
	if [ ! -d "$BASE_EXTERNAL/$ZMQ_VERSION" ]; then
	echo "Download zmq source"

	if !(git clone https://github.com/zeromq/$ZMQ_VERSION.git $BASE_EXTERNAL/$ZMQ_VERSION); then
	    echo "## cannot git clone  https://github.com/zeromq/$ZMQ_VERSION.git"
	    exit 1
	fi
	else
	echo "Update zmq source"
	cd $BASE_EXTERNAL/$ZMQ_VERSION
	git pull
	fi
	cd $BASE_EXTERNAL/$ZMQ_VERSION

#if [ -d "cmake-make" ]; then
#        rm -rf "cmake-make"
#    fi
#    rm -rf "CMakeFiles"

#    mkdir "cmake-make"
#    cd "cmake-make"

#    if [ `echo $OS | tr '[:upper:]' '[:lower:]'` = `echo "Darwin" | tr '[:upper:]' '[:lower:]'` ] && [ $KERNEL_SHORT_VER -ge 1300 ]; then
#        echo "Use standard CLIB with clang"
#        cmake -DZMQ_BUILD_FRAMEWORK=NO -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="-stdlib=libstdc++" $COMP_TYPE -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX -DBUILD_PREFIX=$PREFIX ..
#    else
#        cmake $COMP_TYPE -DZMQ_BUILD_FRAMEWORK=NO -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX -DBUILD_PREFIX=$PREFIX ..
#    fi
#    if [ -n "$CHAOS_DEVELOPMENT" ]; then
#        make -j4  VERBOSE=1
#    else
#        make -j4
#    fi
    ./autogen.sh
	./configure --prefix=$PREFIX $CROSS_HOST_CONFIGURE
	do_make "ZMQ"

#	./autogen.sh
#	./configure --prefix=$PREFIX
#make
#	make install
	echo "ZMQ done"
fi

echo "Compile !CHAOS"
cd $SCRIPTPATH

if [ -n "$CHAOS32" ]; then
    cmake $COMP_TYPE -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX -DBUILD_FORCE_32=true -DBUILD_PREFIX=$PREFIX $SCRIPTPATH/.
else
    if [ `echo $OS | tr '[:upper:]' '[:lower:]'` = `echo "Darwin" | tr '[:upper:]' '[:lower:]'` ] && [ $KERNEL_SHORT_VER -ge 1300 ]; then
        echo "Use standard CLIB with clang"
        cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="-stdlib=libstdc++" $COMP_TYPE -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX -DBUILD_PREFIX=$PREFIX $SCRIPTPATH/.
    else
        cmake $COMP_TYPE -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX -DCMAKE_CXX_COMPILER=$CXX  -DCMAKE_C_COMPILER=$CC -DBUILD_PREFIX=$PREFIX $SCRIPTPATH/.
    fi

fi

NPROC=4

if [ "$ARCH" = "armv7l" ]; then
    NPROC=1
    echo "ARM architecture using $NPROC processors"
fi

if [ -n "$CHAOS_DEVELOPMENT" ]; then
    if !(make -j$NPROC  VERBOSE=1); then
	echo "## error compiling !CHAOS DEVELOPMENT"
	exit 1
    fi
else
    if !(make -j$NPROC); then
	echo "## error compiling !CHAOS"
	exit 1
    fi
fi
make install

if [ -n "$CHAOS_DEVELOPMENT" ]; then
	echo "Remove the installed header"
	rm -rf $CHAOS_DIR/usr/local/include/chaos
	echo "Link !CHAOS source root directory for include because we are in development mode"
	ln -sf $CHAOS_DIR/chaos $CHAOS_DIR/usr/local/include/chaos
fi
