#!/bin/bash -e
arch="x86_64 i686 arm armhf"
build="dynamic static"
prefix="chaos_bundle"
branch="development"
buildtype=""
currdir=`pwd`
installdir="$currdir/chaos-build"
inputdir=""
nproc="4"
unset CHAOS_TARGET
unset CHAOS_PREFIX
while getopts j:b:o:a:t:r:sfh opt; do
    case $opt in
	t) 
	    branch=$OPTARG
	    ;;
	j) 
	    nproc=$OPTARG
	    ;;
	o)
	    installdir=$OPTARG
	    ;;
	a)
	    arch=$OPTARG
	    ;;
	b) 
	    build=$OPTARG
	    ;;
	g) 
	    buildtype=$OPTARG
	    ;;
	i) 
	    inputdir=$OPTARG
	    ;;
	s)
	    fromscratch="true"
	    ;;
	f) 
	    force_reconf="true"
	    ;;

	h)
	    echo -e "Usage is $0 [-s] [-i <chaos bundle input source dir>] [-t <branch to use> ] [-o <installdir>] [-a <architectures> [$arch]] [-b <build> [$build]] [-g <buildtype> [$buildtype]] [-j <#proc>]\n-j <## proc> number of cpu used in compilation [$nproc]\n-f: force reconfiguration\n-s: from scratch, connects to repo and build\n-t <branch to use>: git branch to use [$branch]\n-r reuse dir, each time use the same directory to compile (needs rebuild all)\n-o <installdir> install into dir [$installdir]\n-a <arch>: compile for the fiven architectures[$arch]\n-b <dynamic|static>: make the binaries static and/or dynamic [$build]\n-g <release type>: choose the output configuration [$buildtype]\n"
	    exit 0;
	    ;;
    esac
done


if [ -z "$branch" ];then
    echo "## you should point to a valid branch, try $0 development"
    exit 1
fi

function initialize_bundle(){
    echo "* initializing repo"
    if ! repo init -u ssh://git@opensource-stash.infn.it:7999/chaos/chaos_repo_bundle.git -b development >> $log 2>&1;then
	echo "## repo initialization failed"
	return 1
    fi
    echo "* synching repo"
    if ! repo sync >> $log 2>&1 ;then
	echo "## repo synchronization failed"
	return 1
    fi
    return 0
}
function compile_bundle(){

    local dir=$1
    local arch=$2
    local build=$3
    log="$currdir"/compile_all-$arch-$build.log
    echo "=========================================="
    echo "==== DIR   :$dir"
    echo "==== ARCH  :$arch"
    echo "==== BUILD :$build"
    echo "==== BRANCH:$branch"
    echo "=========================================="

    echo "* entering in $dir checking out \"$branch\""
    echo "* log file \"$log\""
    pushd $dir >& $log
    install_prefix="$installdir/chaos-distrib-$arch-$build"
    if ! mkdir -p $install_prefix;then
	echo "## cannot create $install_prefix"
	exit 1
    fi
    cmake_params="-DCMAKE_INSTALL_PREFIX=$install_prefix";
    echo "* synchronizing with branch \"$branch\"...."
    
    if [ ! -d chaosframework ];then
	if ! initialize_bundle;then
	    popd >& /dev/null
	    exit 1
	fi
    fi
    if ! chaosframework/tools/chaos_git.sh -c $branch >> $log 2>&1 ;then
	echo "## error cannot checkout \"$branch\""
	popd > /dev/null
	exit 1;
    fi

    if ! grep "\+" $log >& /dev/null;then
	echo "* no source change on \"$dir\""

    else
	echo "* source changes detected on \"$dir\""
    fi

    if [ "$build" == "static" ];then
	cmake_params="-DCHAOS_STATIC=ON"
    fi
    case $arch in
	i686)	    
	    cmake_params="$cmake_params -DCHAOS_TARGET=i686-linux26"
	    export PATH=/usr/local/chaos/i686-nptl-linux-gnu/bin:$PATH
	    ;;

	armhf)
	    cmake_params="$cmake_params -DCHAOS_TARGET=armhf"
	    ;;

	arm)
	    cmake_params="$cmake_params -DCHAOS_TARGET=arm-linux-2.6 -DCHAOS_CDS=OFF -DCHAOS_MDS=OFF -DCHAOS_WAN=OFF -DCHAOS_EXAMPLES=OFF"
	    export PATH=/usr/local/chaos/gcc-arm-infn-linux26/bin:$PATH
	    ;;
	crio90xx)
	    cmake_params="$cmake_params -DCHAOS_TARGET=crio90xx -DCHAOS_CDS=OFF -DCHAOS_MDS=OFF -DCHAOS_WAN=OFF -DCHAOS_EXAMPLES=OFF"
	    source /usr/local/chaos/oecore-x86-64/environment-setup-armv7-vfp-neon-nilrt-linux-gnueabi
	    ;;
	x86_64)
	    cmake_params="$cmake_params -DCHAOS_CCS=ON -DQMAKE_PATH=/usr/local/chaos/qt-5.6/bin/"
	    ;;
    esac

    if [ -d "$inputdir" ];then
	chaosframework/tools/chaos_clean.sh . >> $log 2>&1
	echo "* configuring $dir cmake \"$cmake_params\"...."
	if ! cmake $cmake_params . >> $log 2>&1;then
	    echo "## error during cmake configuration \"$cmake_params\""
	    popd >& /dev/null
	    return 1
	fi
       
    fi
    
    if [ ! -f CMakeCache.txt ]|| [ -n "$force_reconf" ];then
	echo "* configuring $dir cmake \"$cmake_params\"...."
	chaosframework/tools/chaos_clean.sh . >> $log 2>&1
	if ! cmake $cmake_params . >> $log 2>&1;then
	    echo "## error during cmake configuration \"$cmake_params\""
	    popd >& /dev/null
	    return 1
	fi
    fi


    echo "* compiling $dir with \"$cmake_params\" ...."
    if ! make -j $nproc install  >> $log 2>&1 ;then
	echo "## error during compilation"
	echo "## error during compilation" >> $log 2>&1
    else
	echo "* compilation ok"
    fi

    popd >& /dev/null
}

if [ -n "$fromscratch" ];then
    echo "* from scratch"
    if [ -z "$inputdir" ];then
	inputdir=$currdir"/build_chaos_bundle"
    fi
    if [ -d "$inputdir" ];then
	echo "* removing $inputdir"
	rm -rf $inputdir
    fi

    mkdir -p $inputdir
    
fi
for a in $arch;do
    for b in $build;do
	dir="$prefix-$a-$b"

	if [ -d "$inputdir" ];then
	    compile_bundle $inputdir $a $b 
	else
	    echo "* checking for $dir in "`pwd`
	    if [ -d "$dir" ];then
		echo "* compiling $dir branch:$branch"
		compile_bundle $dir $a $b 
	    fi
	    
	fi
	    
    done
done
