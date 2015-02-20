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
HOST=$(hostname)
CORES=$(getconf _NPROCESSORS_ONLN)

if [ "$ARCH" = "armv7l" ]; then
    NPROC=1
    echo "ARM architecture detected, using $NPROC processors"
else
    MEM=1

    if [ `echo $OS | tr '[:upper:]' '[:lower:]'` = `echo "Darwin" | tr '[:upper:]' '[:lower:]'` ]; then
	MEM=$(sysctl -a | grep 'hw.memsize:'| awk '{print $2/(1024*1024*1024)}')
    else
        MEM=$(( $(free -m | grep 'Mem' | awk '{print int(($2/1024)+0.5)}') ))
    fi;

    echo "Your system \"$HOST\" has $CORES cpu cores and $MEM gigabytes of physical memory"
    if [ $MEM -ge $CORES ]; then
	NPROC=$CORES
    else
	NPROC=$MEM
    fi;
fi;
echo "Selected compilation concurrent level is: $NPROC"

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
    LMEM_VERSION=1.0.18
fi;

if [ ! -n "$ZMQ_VERSION" ]; then
    ZMQ_VERSION=zeromq4-x
fi;

if [ ! -n "$COUCHBASE_VERSION" ]; then
    COUCHBASE_VERSION=2.4.6
fi;

if [ ! -n "$CHAOS_LINK_LIBRARY" ]; then
    echo "Set the dafult chaos framework linking library"
    export CHAOS_LINK_LIBRARY="boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup memcached zmq uv dl"
    echo $CHAOS_LINK_LIBRARY
fi;


if [ -n "$1" ]; then
    PREFIX=$1/usr/local

else
    if [ ! -n "$CHAOS_PREFIX" ]; then
	PREFIX=$CHAOS_DIR/usr/local
    else
	PREFIX=$CHAOS_PREFIX
    fi
fi



CROSS_HOST_CONFIGURE=""
if [ -n "$CHAOS_CROSS_HOST" ]; then
    CROSS_HOST_CONFIGURE="--host=$CHAOS_CROSS_HOST"
fi

do_make() {
    make clean
    echo "* make $1 with "$NPROC" processors"
    if [ -n "$CHAOS_DEVELOPMENT" ]; then
	if !(make -j$NPROC VERBOSE=1); then
	    echo "## error compiling $1 in VERBOSE"
	    exit 1
	fi
    else
	if !(make -j$NPROC ); then
	    echo "## error compiling $1"
	    exit 1
	fi
    fi

     if !(make install); then
     	echo "## error installing $1"
     	exit 1
     fi
}



CHAOS_DIR=$SCRIPTPATH
BASE_EXTERNAL=$CHAOS_DIR/external


echo "Operating system version: $OS"
echo "Current architecture: $ARCH"
echo "Current kernel version: $KERNEL_VER"
echo "Current short kernel version: $KERNEL_SHORT_VER"
echo "Using $CHAOS_DIR as chaos folder"
echo "Using $BASE_EXTERNAL as external library folder"
echo "Using $PREFIX as prefix folder"
echo "Using BOOST version $BOOST_VERSION/$BOOST_VERSION_IN_PATH ($BOOST_NUMBER_VERSION)"
echo "Compilation type -> $CHAOS_COMP_TYPE"

if [ ! -d "$BASE_EXTERNAL" ]; then
    mkdir -p $BASE_EXTERNAL
fi

if [ ! -e "$PREFIX/include/zlib.h" ] || [ ! -e "$PREFIX/lib/libz.a" ]; then
    if [ ! -d "$BASE_EXTERNAL/zlib-$ZLIB_VERSION" ]; then
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

## json cpp
if [ ! -f $PREFIX/include/json/json.h ]; then
    if [ ! -d $BASE_EXTERNAL/jsoncpp ]; then
		if !(git clone https://github.com/bisegni/jsoncpp.git $BASE_EXTERNAL/jsoncpp) ; then
			echo "## cannot checkout jsoncpp"
			exit 1
		fi

    fi
    cd $BASE_EXTERNAL/jsoncpp
    git checkout pre-clang
    make clean
    rm CMakeCache.txt
    if [ -n "$CHAOS_STATIC" ]; then
	CXX=$CXX cmake $CHAOS_CMAKE_FLAGS -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF  
    else
	CXX=$CXX cmake $CHAOS_CMAKE_FLAGS -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DJSONCPP_LIB_BUILD_SHARED=ON 
    fi
    do_make "jsoncpp"
fi

## mongoose install
if [ ! -f $PREFIX/include/mongoose-cpp/mongoose.h ]; then
    if [ ! -f $BASE_EXTERNAL/mongoose-cpp/mongoose.h ]; then
		  if !(git clone https://github.com/bisegni/mongoose-cpp.git $BASE_EXTERNAL/mongoose-cpp) ; then
			  echo "## cannot checkout moongoose-cpp"
			  exit 1
		  else
        cd $BASE_EXTERNAL/mongoose-cpp
        git checkout -b chaos_master origin/chaos_master
      fi
    else
      cd $BASE_EXTERNAL/mongoose-cpp
      git pull
    fi
    rm CMakeCache.txt
    make clean
# -DHAS_JSONCPP=ON
   if [ -n "$CHAOS_STATIC" ]; then
       CXX=$CXX CC=$CC cmake $CHAOS_CMAKE_FLAGS -DHAS_JSONCPP=ON
   else
       CXX=$CXX CC=$CC cmake $CHAOS_CMAKE_FLAGS -DSHAREDLIB=ON -DHAS_JSONCPP=ON -DJSONCPP_DIR=$PREFIX
   fi

	do_make "mongoose-cpp"
fi

##

if [ ! -d "$PREFIX/include/boost" ]; then
    if [ ! -e $BASE_EXTERNAL/boost ]; then
	if [ ! -e "$BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz" ]; then
            echo "Download boost source"
            if !( wget --no-check-certificate -O $BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz "http://downloads.sourceforge.net/project/boost/boost/$BOOST_VERSION_IN_PATH/boost_$BOOST_VERSION.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F$BOOST_VERSION_IN_PATH%2F&ts=1350734344&use_mirror=freefr" ); then
	    echo "## cannot download boost_$BOOST_VERSION.tar.gz"
	    exit 1
	    fi

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

    if [ -n "$CHAOS_CROSS_HOST" ]; then
	echo "* Patching project-config.jam to cross compile for $CHAOS_CROSS_HOST"
	sed -i .bak -e "s/using gcc/using gcc : arm : $CXX/" project-config.jam
    fi


    cd $BASE_EXTERNAL/boost
    echo "Compile and install boost libraries into $PREFIX/"
    ./b2 --clean
    ./b2 $CHAOS_BOOST_FLAGS -j $NPROC

else
    echo "Boost Already present"
fi

### install libmodbus
if [ ! -d "$PREFIX/include/modbus" ] || [ ! -d "$BASE_EXTERNAL/libmodbus" ]; then
    echo "Setup libmodbus library"
    if [ ! -d "$BASE_EXTERNAL/libmodbus" ]; then
        echo "Install libmodbus"
        git clone https://github.com/stephane/libmodbus.git $BASE_EXTERNAL/libmodbus
        cd $BASE_EXTERNAL/libmodbus
	git checkout v3.0.5
    else
        echo "Update libmodbus"
        cd $BASE_EXTERNAL/libmodbus/
        git pull v3.05
    fi

    ./autogen.sh
    if [ -n "$CHAOS_STATIC" ]; then
	./configure --enable-static --prefix=$PREFIX $CROSS_HOST_CONFIGURE
    else
	./configure --enable-shared --prefix=$PREFIX $CROSS_HOST_CONFIGURE
    fi

    do_make "MODBUS"

    echo "libmodbus done"
fi

echo "Setup LIBEVENT :$LIB_EVENT_VERSION"
if [ ! -d "$PREFIX/include/event2" ]; then
    if [ ! -d "$BASE_EXTERNAL/libevent" ]; then
	echo "Installing LibEvent"
	if !(git clone https://github.com/libevent/libevent.git $BASE_EXTERNAL/libevent); then
	    echo "## cannot clone http://git.code.sf.net/p/levent/libevent"
	    exit 1
	fi
    fi
    cd $BASE_EXTERNAL/libevent
    git checkout $LIB_EVENT_VERSION
    git pull
    ./autogen.sh
    ./configure --prefix=$PREFIX $CROSS_HOST_CONFIGURE
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
	git checkout -b good_for_chaos 1552184
    else
	cd $BASE_EXTERNAL/libuv
	#git pull
    fi
    ./autogen.sh
    ./configure --prefix=$PREFIX $CROSS_HOST_CONFIGURE
    do_make "LIBUV"
    echo "LIBUV done"
fi

echo "Setup Couchbase sdk"
if [ ! -f "$PREFIX/include/libcouchbase/couchbase.h" ]; then
    if [ ! -d "$BASE_EXTERNAL/libcouchbase" ]; then
	echo "Download couchabse source"
	if !(git clone https://github.com/couchbase/libcouchbase.git $BASE_EXTERNAL/libcouchbase); then
	    echo "## cannot wget http://packages.couchbase.com/clients/c/libcouchbase-$COUCHBASE_VERSION.tar.gz"
	    exit 1
	fi
    cd $BASE_EXTERNAL/libcouchbase
    git checkout -b good_for_chaos $COUCHBASE_VERSION
    fi
    cd $BASE_EXTERNAL/libcouchbase
    if [ -n "$CHAOS_STATIC" ]; then
	cmake $CHAOS_CMAKE_FLAGS -DLCB_BUILD_STATIC=true .
    else
	cmake $CHAOS_CMAKE_FLAGS .
    fi
    do_make "COUCHBASE"
    echo "Couchbase done"
fi

echo "Setup MongoDB client"
if [ ! -f "$PREFIX/include/mongo/client/dbclient.h" ]; then
    if [ ! -d "$BASE_EXTERNAL/mongo" ]; then
	echo "Download mongodb client"
	if !(git clone https://github.com/mongodb/mongo-cxx-driver.git $BASE_EXTERNAL/mongo); then
	    echo "## cannnot git clone https://github.com/mongodb/mongo-cxx-driver.git"
	    exit 1
	fi
	cd $BASE_EXTERNAL/mongo
	git checkout $MONGO_VERSION
    else
	cd $BASE_EXTERNAL/mongo
	git pull $MONGO_VERSION
    fi
    ## centos6 does not detect correctly boost_thread becasue script fails linking boost_system that is required, force to be included in test
    #--use-system-boost --full removed on used mongodb version
    if !( scons --prefix=$PREFIX --libpath=$PREFIX/lib --cxx="$CXX" --cc="$CC" --cpppath=$PREFIX/include --extrapath=$PREFIX --extralib=boost_system  install-mongoclient); then
	echo "## error scons configuration of mongo failed, maybe you miss scons package"
	exit 1
    fi
    echo "Mongodb done"
fi

echo "Setup LIBMEMCACHED"
if [ ! -d "$PREFIX/include/libmemcached" ]; then
    echo "Install libmemcached into  $BASE_EXTERNAL/libmemcached"
    if [ ! -d "$BASE_EXTERNAL/libmemcached-$LMEM_VERSION" ]; then
	if !(wget --no-check-certificate -O $BASE_EXTERNAL/libmemcached.tar.gz https://launchpad.net/libmemcached/1.0/$LMEM_VERSION/+download/libmemcached-$LMEM_VERSION.tar.gz); then
	    echo "## cannot wget  https://launchpad.net/libmemcached/1.0/$LMEM_VERSION/+download/libmemcached-$LMEM_VERSION.tar.gz"
	    exit 1
	fi
	tar zxvf $BASE_EXTERNAL/libmemcached.tar.gz -C $BASE_EXTERNAL
    fi
    cd $BASE_EXTERNAL/libmemcached-$LMEM_VERSION

    if !(./configure --without-memcached --disable-sasl --prefix=$PREFIX $CROSS_HOST_CONFIGURE); then
	echo "Memcached configuration failed"
	exit 1
    fi
    ## use standard types instead cinttypes that generates troubles in ARM annd clang
    echo "patching memcached.h to use the correct cinttypes"
    sed -i .bak -e "s/include <cinttypes>/include <tr1\/cinttypes>/" libmemcached-1.0/memcached.h

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

    ./autogen.sh
    ./configure --prefix=$PREFIX $CROSS_HOST_CONFIGURE --with-gnu-ld
    do_make "ZMQ"
    echo "ZMQ done"
fi



echo "Compile !CHAOS"
cd $SCRIPTPATH
cmake $CHAOS_CMAKE_FLAGS .

do_make "!CHAOS"


# if [ -n "$CHAOS_DEVELOPMENT" ]; then
#     echo "Remove the installed header"
#     rm -rf $CHAOS_DIR/usr/local/include/chaos
#     echo "Link !CHAOS source root directory for include because we are in development mode"
#     ln -sf $CHAOS_DIR/chaos $CHAOS_DIR/usr/local/include/chaos
# fi
