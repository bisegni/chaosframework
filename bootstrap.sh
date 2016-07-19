#!/bin/bash

#####
#Dipendence: automake libtool subversion git-core bzr ruby1.8-full cmake zlib1g-dev libcloog-ppl0 wget g++
#####
nmake=0

pushd `dirname $0` > /dev/null
SCRIPTPATH=`pwd -P`
popd > /dev/null

# export CXXFLAGS="$CXXFLAGS -fPIC"
# export CFLAGS="$CFLAGS -fPIC"
chaos_exclude(){
    name=$1
    for i in $CHAOS_EXCLUDE_DIR;do
	if [ "$name" == "$i" ]; then
	    return 0;
	fi
    done
    return 1
}


if [ -z "$CHAOS_BUNDLE" ];then
    CHAOS_BUNDLE=$SCRIPTPATH
    echo "* setting CHAOS_BUNDLE=$CHAOS_BUNDLE"
fi

if [ -z "$CHAOS_PREFIX" ];then
    mkdir -p $CHAOS_BUNDLE/usr/local
    CHAOS_PREFIX=$CHAOS_BUNDLE/usr/local
    echo "* setting CHAOS_PREFIX=$CHAOS_PREFIX"
fi


### DEFAULT VERSIONS
if [ -z "$MONGO_VERSION" ];then
    MONGO_VERSION=legacy-1.0.0-rc0
fi
if [ -z "$LIB_EVENT_VERSION" ];then
    LIB_EVENT_VERSION=release-2.1.4-alpha
fi


#if [ ! -n "$LUA_VERSION" ]; then
#    LUA_VERSION=5.3.2
#fi;

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

if [ -z "CXXFLAGS" ];then
    export CXXFLAGS="-DCHAOS -fPIC"
fi
if [ -z "CFLAGS" ];then
    export CFLAGS="-DCHAOS -fPIC"
fi

if [ -z "$CHAOS_BOOST_VERSION" ];then
    CHAOS_BOOST_VERSION=55
fi

#####
if [ -e $CHAOS_BUNDLE/tools/common_util.sh ];then
    source $CHAOS_BUNDLE/tools/common_util.sh
#    NO_MONGOOSE="TRUE"
else
    OS=`uname -s`
    ARCH=`uname -m`
    KERNEL_VER=$(uname -r)
    KERNEL_SHORT_VER=$(uname -r|cut -d\- -f1|tr -d '.'| tr -d '[A-Z][a-z]')
    PID=$$
    CHAOSTMP="/tmp/""$USER"
    TEST_INFO_NAME=$CHAOSTMP"/__chaos_test_info__"
    mkdir -p /tmp/$USER >& /dev/null
    if [ -z "$NPROC" ];then
	NPROC=$(getconf _NPROCESSORS_ONLN)
    fi
    CHAOS_DIR=$CHAOS_BUNDLE
    if [ `echo $OS | tr '[:upper:]' '[:lower:]'` = `echo "Darwin" | tr '[:upper:]' '[:lower:]'` ] && [ $KERNEL_SHORT_VER -ge 1300 ] && [ ! -n "$CROSS_HOST" ]; then
	echo "Use standard CLIB with clang"
	CHAOS_BOOST_VERSION=56
	export CC=clang
	export CXX="clang++"
	export CXXFLAGS="-stdlib=libstdc++"
	export LDFLAGS="-stdlib=libstdc++"
	export CHAOS_CMAKE_FLAGS="$CHAOS_CMAKE_FLAGS -DCMAKE_CXX_FLAGS=-stdlib=libstdc++ $CHAOS_COMP_TYPE -DCMAKE_INSTALL_PREFIX:PATH=$CHAOS_PREFIX"
	export LD=clang
	APPLE="true"
    ## 18, 16 doesnt compile
	export LMEM_VERSION=1.0.18
	export CHAOS_BOOST_FLAGS="$CHAOS_BOOST_FLAGS toolset=clang cxxflags=-stdlib=libstdc++ linkflags=-stdlib=libstdc++ link=static runtime-link=shared variant=release"

    fi

fi



if [ -z "$CHAOS_COMP_TYPE" ];then
    CHAOS_COMP_TYPE=" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS_DEBUG=-DDEBUG=1 "
fi



if [ -z "$CHAOS_BOOS_FLAGS" ];then
    CHAOS_BOOST_FLAGS="$CHAOS_BOOST_FLAGS --prefix=$CHAOS_PREFIX --with-program_options --with-chrono --with-filesystem --with-iostreams --with-log --with-regex --with-random --with-system --with-thread --with-atomic --with-timer install link=static runtime-link=shared variant=release cxxflags=-fPIC"
    echo "* setting CHAOS_BOOST_FLAGS=$CHAOS_BOOST_FLAGS"
fi

if [ -z "$CHAOS_CMAKE_FLAGS" ];then
    CHAOS_CMAKE_FLAGS="$CHAOS_CMAKE_FLAGS $CHAOS_COMP_TYPE -DCMAKE_INSTALL_PREFIX:PATH=$CHAOS_PREFIX -DCMAKE_CXX_FLAGS=-fPIC"
    echo "* setting CHAOS_CMAKE_FLAGS=$CHAOS_CMAKE_FLAGS"
fi

if [ -z "$CHAOS_ZMQ_CONFIGURE" ];then
    CHAOS_ZMQ_CONFIGURE="--prefix=$CHAOS_PREFIX $CROSS_HOST_CONFIGURE --with-gnu-ld --disable-eventfd"
    echo "* setting CHAOS_ZMQ_CONFIGURE=$CHAOS_ZMQ_CONFIGURE"
fi

if [ -z "$CHAOS_LIBEVENT_CONFIGURE" ];then 
    CHAOS_LIBEVENT_CONFIGURE="--disable-openssl --prefix=$CHAOS_PREFIX"
    echo "* setting CHAOS_LIBEVENT_CONFIGURE=$CHAOS_LIBEVENT_CONFIGURE"
fi
 
if [ -z "$CHAOS_LIBMEMCACHED_CONFIGURE" ];then
    CHAOS_LIBMEMCACHED_CONFIGURE="--without-memcached --with-pic --disable-memaslap --disable-sasl --prefix=$CHAOS_PREFIX $CROSS_HOST_CONFIGURE"
    echo "* setting CHAOS_LIBMEMCACHED_CONFIGURE=$CHAOS_LIBMEMCACHED_CONFIGURE"
fi
    


if [ -z "$CHAOS_CB_CONFIGURE" ];then
    CHAOS_CB_CONFIGURE="$CHAOS_CMAKE_FLAGS -DLCB_NO_SSL=true -DLCB_NO_TESTS=true -DLCB_NO_TOOLS=true -DLCB_NO_PLUGINS=true"
    echo "* setting CHAOS_CB_CONFIGURE=$CHAOS_CB_CONFIGURE"
fi


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

    echo "Your compilation system will use $NPROC cpu cores and $MEM gigabytes of physical memory"
    if [ "$MEM" -lt "$NPROC" ]; then
	NPROC=$MEM
    fi;
fi;
echo "Selected compilation concurrent level is: $NPROC"
PROF=""
if [ "$CHAOS_DEVELOPMENT" == "profile" ];then
    PROF="-pg"
    echo "*** ENABLE PROFILING"
fi

if [ -n "$CHAOS_BOOST_VERSION" ]; then
    BOOST_VERSION="1_"$CHAOS_BOOST_VERSION"_0"
    BOOST_VERSION_IN_PATH="1.$CHAOS_BOOST_VERSION.0"
else
    BOOST_VERSION=1_56_0
    BOOST_VERSION_IN_PATH=1.56.0
fi

BOOST_NUMBER_VERSION=$(echo $BOOST_VERSION_IN_PATH |sed "s/[^0-9]//g" )

if [ ! -n "$OS" ]; then
    OS=$(uname -s)
fi;


if [ ! -n "$CHAOS_LINK_LIBRARY" ]; then
    echo "Set the dafult chaos framework linking library"
    export CHAOS_LINK_LIBRARY="boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup zmq dl"
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




do_make() {
    # make clean
    opt=""
    if [ -n "$3" ];then
	opt="$3"
    fi
    if [ ! -z "$2" ]; then
	echo "* make clean"
	make clean
    fi
    echo "* make $1 with "$NPROC" processors,opt:\"$opt\""
    if [ -n "$CHAOS_DEVELOPMENT" ]; then
	if !(make -j$NPROC VERBOSE=1 $opt); then
	    echo "## error compiling $1 in VERBOSE"
	    if [ "$opt" != "-k" ];then
		echo " exiting"
		exit 1
	    fi
	fi
    else
	if !(make -j$NPROC $opt); then
	    echo "## error compiling $1"
	    if [ "$opt" != "-k" ];then
		echo " exiting"
		exit 1
	    fi

	fi
    fi

     if !(make install); then
     	echo "## error installing $1"
	if [ "$opt" != "-k" ];then
	    echo " exiting"
	    exit 1
	fi

     fi
     ((nmake++))
}



CHAOS_DIR=$CHAOS_BUNDLE
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
echo "Boost flags: \"$CHAOS_BOOST_FLAGS\""
if [ ! -d "$BASE_EXTERNAL" ]; then
    mkdir -p $BASE_EXTERNAL
fi
CMAKE_CHAOS_FRAMEWORK=""
if chaos_exclude "mongo";then
    CHAOS_NO_MONGO=true;
    CMAKE_CHAOS_FRAMEWORK="-DCHAOS_NO_MONGO=true"
    export CXXFLAGS="$CXXFLAGS -DCHAOS_NO_MONGO"
fi

if chaos_exclude "chaos_services";then
    CHAOS_NO_SERVICES=true;
    CMAKE_CHAOS_FRAMEWORK="$CMAKE_CHAOS_FRAMEWORK -DCHAOS_NO_SERVICES=true"
fi

if chaos_exclude "memcached";then
    CHAOS_NO_MEMCACHE=true
    export CXXFLAGS="$CXXFLAGS -DCHAOS_NO_MEMCACHED"

fi

if chaos_exclude "couchbase";then
    CHAOS_NO_COUCHBASE=true;
    export CXXFLAGS="$CXXFLAGS -DCHAOS_NO_COUCHBASE"
fi
if chaos_exclude "zmq";then
    CHAOS_NO_ZMQ=true;
    export CXXFLAGS="$CXXFLAGS -DCHAOS_NO_ZMQ"
fi

if chaos_exclude "libuv";then
    CHAOS_NO_LIBUV=true;
    export CXXFLAGS="$CXXFLAGS -DCHAOS_NO_LIBUV"
fi


# if [ ! -f "$PREFIX/include/sigar.h" ] || [ ! -f "$PREFIX/lib/libsigar.a" ]; then
#     echo "* need sigar"
#     if [ ! -d "$BASE_EXTERNAL/sigar" ]; then
# 	cd "$BASE_EXTERNAL"
# 	if !( git clone http://github.com/hyperic/sigar.git ); then
# 	echo "## cannot download libsigar from http://github.com/hyperic/sigar.git "
# 	exit 1;
# 	fi
#     fi
    
#     if [ -d "$BASE_EXTERNAL/sigar" ]; then
# 	cd $BASE_EXTERNAL/sigar
# 	echo "entering in $BASE_EXTERNAL/sigar"
# 	echo "using $CC and $CXX"
# 	./autogen.sh
# 	if [ -n "$CHAOS_STATIC" ]; then
# 	    ./configure --enable-maintainer-mode --enable-static --prefix=$PREFIX $CROSS_HOST_CONFIGURE
# 	else
# 	    ./configure --enable-maintainer-mode --enable-shared --prefix=$PREFIX $CROSS_HOST_CONFIGURE
# 	fi
	
# 	do_make "sigar" 1
#     else
# 	echo "$BASE_EXTERNAL/sigar not found"
# 	exit 1
#     fi
# fi
## json cpp
if [ ! -f $PREFIX/include/json/json.h ]; then
    echo "* need json"
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
    # if [ -n "$CHAOS_STATIC" ]; then
    # 	CXX=$CXX cmake $CHAOS_CMAKE_FLAGS -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF
    # else
    # 	CXX=$CXX cmake $CHAOS_CMAKE_FLAGS -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DJSONCPP_LIB_BUILD_SHARED=ON
    # fi
    echo "---> $CHAOS_CMAKE_FLAGS"
    cmake $CHAOS_CMAKE_FLAGS -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF
    do_make "jsoncpp" 1
fi

## mongoose install
if [ -z "$NO_MONGOOSE" ];then
    if [ ! -f $PREFIX/include/mongoose.h ]; then
	echo "* need mongoose"
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
	cmake $CHAOS_CMAKE_FLAGS -DHAS_JSONCPP=ON -DENABLE_PIC=ON
	# -DHAS_JSONCPP=ON
	# if [ -n "$CHAOS_STATIC" ]; then
	#     CXX=$CXX CC=$CC cmake $CHAOS_CMAKE_FLAGS -DHAS_JSONCPP=ON
	# else
   #     CXX=$CXX CC=$CC cmake $CHAOS_CMAKE_FLAGS -DSHAREDLIB=ON -DHAS_JSONCPP=ON -DJSONCPP_DIR=$PREFIX
	# fi
	
	do_make "mongoose-cpp" 1
    fi
fi
##

#if [ -z "$CHAOS_NO_MEMCACHE" ];then
#echo "Setup LIBMEMCACHED"
#if [ ! -d "$PREFIX/include/libmemcached" ]; then
#    echo "* need memcached"
#
#    if [ ! -d "$BASE_EXTERNAL/libmemcached-$LMEM_VERSION" ]; then
#	if !(wget --no-check-certificate -O $BASE_EXTERNAL/libmemcached.tar.gz https://launchpad.net/libmemcached/1.0/$LMEM_VERSION/+download/libmemcached-$LMEM_VERSION.tar.gz); then
#	    echo "## cannot wget  https://launchpad.net/libmemcached/1.0/$LMEM_VERSION/+download/libmemcached-$LMEM_VERSION.tar.gz"
#	    exit 1
#	fi
#	tar zxvf $BASE_EXTERNAL/libmemcached.tar.gz -C $BASE_EXTERNAL
#    fi
#    cd $BASE_EXTERNAL/libmemcached-$LMEM_VERSION
#
#    if !(./configure $CHAOS_LIBMEMCACHED_CONFIGURE ); then
#	echo "Memcached configuration failed"
#	exit 1
#    fi
#    ## use standard types instead cinttypes that generates troubles in ARM annd clang
#    echo "patching memcached.h to use the correct cinttypes"
#    sed -i .bak -e "s/include <cinttypes>/include <tr1\/cinttypes>/" libmemcached-1.0/memcached.h
#
#    do_make "LIBMEMCACHED" 1 -k
#fi
#echo "Libmemcached done"
#else
#echo "skipping libmemcached"
#fi


export CXXFLAGS="$CXXFLAGS $PROF"
if [ ! -f "$PREFIX/include/zlib.h" ] || [ ! -f "$PREFIX/lib/libz.a" ]; then
    echo "* need zlib"
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
	do_make "ZLIB" 1
    else
	echo "$BASE_EXTERNAL/zlib-$ZLIB_VERSION not found"
	exit 1
    fi
fi


if [ ! -d "$PREFIX/include/boost" ]; then
    echo "* need boost"
    if [ ! -e "$BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz" ]; then
        echo "Download boost $BOOST_VERSION source"
        if !( wget --no-check-certificate -O $BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz "http://download.sourceforge.net/project/boost/boost/$BOOST_VERSION_IN_PATH/boost_$BOOST_VERSION.tar.gz" ); then
	    echo "## cannot download boost_$BOOST_VERSION.tar.gz"
	    exit 1
	fi
	    
    fi
    
    if [ ! -e $BASE_EXTERNAL/boost ]; then
        tar zxvf $BASE_EXTERNAL/boost_$BOOST_VERSION.tar.gz -C $BASE_EXTERNAL
        mv $BASE_EXTERNAL/boost_$BOOST_VERSION $BASE_EXTERNAL/boost
	if [ -e $CHAOS_BUNDLE/tools/patches/boost-1.55.0-atomic-check_lock_free_flag.patch ];then
	    cp $CHAOS_BUNDLE/tools/patches/boost-1.55.0-atomic-check_lock_free_flag.patch $BASE_EXTERNAL/boost
	fi
	
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
    if [ -e boost-1.55.0-atomic-check_lock_free_flag.patch ];then
	patch -p1 < boost-1.55.0-atomic-check_lock_free_flag.patch >& /dev/null
    fi
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
    echo "using zlib : $ZLIB_VERSION : $CHAOS_PREFIX ;" > user-config.jam
    ./b2 $CHAOS_BOOST_FLAGS -j $NPROC
    
else
    echo "Boost Already present"
fi

# #install lua language
# if [ ! -f "$PREFIX/include/lua.h" ]; then
#     echo "* need lua"
#     if [ ! -e "$BASE_EXTERNAL/lua-$LUA_VERSION.tar.gz" ]; then
#         echo "Download lua source to $BASE_EXTERNAL/lua-$LUA_VERSION.tar.gz"
#         if !( curl -R -o $BASE_EXTERNAL/lua-$LUA_VERSION.tar.gz http://www.lua.org/ftp/lua-$LUA_VERSION.tar.gz ); then
#             echo "## cannot download lua-$LUA_VERSION.tar.gz"
#             exit 1
#         fi

#     fi

#     if [ ! -e $BASE_EXTERNAL/lua-$LUA_VERSION ]; then
#         tar zxvf $BASE_EXTERNAL/lua-$LUA_VERSION.tar.gz -C $BASE_EXTERNAL
#     fi


#     echo "Compiling lua"
#     cd $BASE_EXTERNAL/lua-$LUA_VERSION
#     if [ $OS = "Darwin" ]; then
#         LUA_PLAT="macosx"
#     elif [ $OS = "Linux" ]; then
#         LUA_PLAT="linux"
#     else
#         echo "Invalid lua platform detected"
#         exit 1
#     fi

# #do_make "lua" 0 "$LUA_PLAT local"
#     #compile lua according to detected platform
#     if !( make $LUA_PLAT local ); then
#         echo "## Error compiling lua"
#         exit 1
#     fi
#     echo "Installing lua"

#     if !( cp -Rf install/bin/* $PREFIX/bin ); then
#         echo "## Error installing lua into bin"
#         exit 1
#     fi

#     if !( cp -R -f install/include/* $PREFIX/include ); then
#         echo "## Error installing lua into include"
#         exit 1
#     fi

#     if !( cp -R -f install/lib/* $PREFIX/lib ); then
#         echo "## Error installing lua into lib"
#         exit 1
#     fi

#     if !( cp -R -f install/man/* $PREFIX/man ); then
#         echo "## Error installing lua into man"
#         exit 1
#     fi

#     if !( cp -R -f install/share/* $PREFIX/share ); then
#         echo "## Error installing lua into share"
#         exit 1
#     fi

#     echo "Lua installed"
# else
#     echo "Lua already installed"
# fi

echo "Setup LIBEVENT  $CHAOS_LIBEVENT_CONFIGURE :$LIB_EVENT_VERSION"
if [ ! -d "$PREFIX/include/event2" ]; then
    echo "* need libevent"
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
   
    ./configure $CHAOS_LIBEVENT_CONFIGURE
    do_make "LIBEVENT" 1
    echo "LIBEVENT done"
fi

if [ -z "$CHAOS_NO_COUCHBASE" ]; then
echo "Setup Couchbase sdk, $CHAOS_CB_CONFIGURE"
if [ ! -f "$PREFIX/include/libcouchbase/couchbase.h" ]; then
    echo "* need couchbase"
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
    cmake $CHAOS_CB_CONFIGURE .

    do_make "COUCHBASE" 1
    echo "Couchbase done"
fi
else
echo "skipping COUCHBASE"
fi

if [ -z "$CHAOS_NO_MONGO" ]; then
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
    if !( scons --prefix=$PREFIX --libpath=$PREFIX/lib --cxx="$CXX" --cc="$CC" --disable-warnings-as-errors --cpppath=$PREFIX/include --extrapath=$PREFIX --extralib=boost_system  install-mongoclient); then
	echo "## error scons configuration of mongo failed, maybe you miss scons package"
	exit 1
    fi
    echo "Mongodb done"
fi
else
echo "skipping MONGO"
fi


if [ -z "$CHAOS_NO_ZMQ" ]; then
    echo "Setup ZMQ $CHAOS_ZMQ_CONFIGURE"
    if [ ! -f "$PREFIX/include/zmq.h" ]; then
	echo "* need zmq"
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
    ./configure $CHAOS_ZMQ_CONFIGURE
    do_make "ZMQ" 1
    echo "ZMQ done"
    fi

else
    echo "skipping ZMQ"
fi


echo "Compile !CHAOS"
cd $SCRIPTPATH
if [ $nmake -gt 0 ];then
    cmake $CHAOS_CMAKE_FLAGS $CMAKE_CHAOS_FRAMEWORK .
else
    echo "* nothing changed"
fi

do_make "!CHAOS"
exit 0

# if [ -n "$CHAOS_DEVELOPMENT" ]; then
#     echo "Remove the installed header"
#     rm -rf $CHAOS_DIR/usr/local/include/chaos
#     echo "Link !CHAOS source root directory for include because we are in development mode"
#     ln -sf $CHAOS_DIR/chaos $CHAOS_DIR/usr/local/include/chaos
# fi
