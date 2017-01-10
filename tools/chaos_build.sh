#!/bin/bash -e
arch="x86_64 i686 arm armhf"
build="dynamic static"
prefix="chaos_bundle"
branch="development"
buildtype=""

pushd `dirname $0` >& /dev/null
mydir=`pwd -P`
popd >& /dev/null

currdir=`pwd`
installdir="$currdir/chaos-build"
inputdir=""
nproc="4"
log="default"
unset CHAOS_TARGET
unset CHAOS_PREFIX
host=`hostname`
host_arch=`uname -m`
host_date=`date`


mail_obj="[CHAOS CHECK] ERROR on $host($host_arch) $host_date"
target=""
stage=""
deploy_mode=0

function send_error_mail(){
    to=$1
    obj="$mail_obj [$target] during:$stage"
    body=`cat $log|grep error`

    mail -s "$obj" -t "$to" < $body
EOF
}
while getopts j:b:o:a:t:r:sfhd:i: opt; do
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
	d) 
	    deploy_mode="$OPTARG"
	    ;;

	h)
	    echo -e "Usage is $0 [-s] [-d] [-i <chaos bundle input source dir>] [-t <branch to use> ] [-o <installdir>] [-a <architectures> [$arch]] [-b <build> [$build]] [-g <buildtype> [$buildtype]] [-j <#proc>]\n-d <deploy mode 0=build and deploy on server, 1=build, test and deploy package>: deploy fo\n-j <## proc> number of cpu used in compilation [$nproc]\n-f: force reconfiguration\n-s: from scratch, connects to repo and build\n-t <branch to use>: git branch to use [$branch]\n-r reuse dir, each time use the same directory to compile (needs rebuild all)\n-o <installdir> install into dir [$installdir]\n-a <arch>: compile for the fiven architectures[$arch]\n-b <dynamic|static>: make the binaries static and/or dynamic [$build]\n-g <release type>: choose the output configuration [$buildtype]\n"
	    exit 0;
	    ;;
    esac
done


if [ -z "$branch" ];then
    echo "## you should point to a valid branch, try $0 development"
    exit 1
fi

function printlog(){
    echo "$1"
    echo "$1" >> $log 2>&1
    
}
function initialize_bundle(){
    printlog "* initializing repo"

    if ! repo init -u ssh://git@opensource-stash.infn.it:7999/chaos/chaos_repo_bundle.git -b development >> $log 2>&1;then
	printlog "## repo initialization failed"
	return 1
    fi
    printlog "* synching repo"
    if ! repo sync >> $log 2>&1 ;then
	printlog "## repo synchronization failed"
	return 1
    fi
    return 0
}
function compile_bundle(){

    local dir=$1
    local arch=$2
    local build=$3
    log="$currdir"/compile_all-$arch-$build.log
    pushd $dir >& $log
    printlog "=========================================="
    printlog "==== DIR   :$dir"
    printlog "==== ARCH  :$arch"
    printlog "==== BUILD :$build"
    printlog "==== BRANCH:$branch"
    printlog "=========================================="

    printlog "* entering in $dir checking out \"$branch\""
    printlog "* log file \"$log\""
    stage="initilization $dir $arch $build $branch"
    target="DIR $dir, ARCH $arch, BUILD:$build, BRANCH:$branch"
    install_prefix="$installdir/chaos-distrib-$arch-$build-$branch"
    if ! mkdir -p $install_prefix;then
	printlog "## cannot create $install_prefix"
	exit 1
    fi
    cmake_params="-DCMAKE_INSTALL_PREFIX=$install_prefix";
    printlog "* synchronizing with branch \"$branch\"...."
    stage="initilization bundle"    
    if [ ! -d chaosframework ];then
	if ! initialize_bundle;then

	    popd >& /dev/null
	    exit 1
	fi
    fi
    stage="git access to \"$branch\""
    if ! chaosframework/tools/chaos_git.sh -c $branch >> $log 2>&1 ;then
	printlog "## error cannot checkout \"$branch\""
	popd > /dev/null
	exit 1;
    fi

    if ! grep "\+" $log >& /dev/null;then
	printlog "* no source change on \"$dir\""

    else
	printlog "* source changes detected on \"$dir\""
    fi

    if [ "$build" == "static" ];then
	cmake_params="$cmake_params -DCHAOS_STATIC=ON"
    fi
    case $arch in
	i686)	    
	    cmake_params="$cmake_params -DCHAOS_TARGET=i686-linux26"
	    export PATH=/usr/local/chaos/i686-nptl-linux-gnu/bin:$PATH
	    ;;

	armhf)
	    cmake_params="$cmake_params -DCHAOS_TARGET=armhf -DCHAOS_CDS=OFF"
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
	    enable_ccs=true
	    ;;
    esac
    stage="configuration ($cmake_params)"
    if [ -d "$inputdir" ];then
#	chaosframework/tools/chaos_clean.sh . >> $log 2>&1
	printlog "* configuring $dir cmake \"$cmake_params\"...."
	if ! cmake $cmake_params . >> $log 2>&1;then
	    printlog "## error during cmake configuration \"$cmake_params\""
	    popd >& /dev/null
	    return 1
	fi
       
    fi
    
    if [ ! -f CMakeCache.txt ]|| [ -n "$force_reconf" ];then
	printlog "* configuring $dir cmake \"$cmake_params\"...."
	rm -rf $install_prefix
	chaosframework/tools/chaos_clean.sh . >> $log 2>&1
	if ! cmake $cmake_params . >> $log 2>&1;then
	    printlog "## error during cmake configuration \"$cmake_params\""
	    popd >& /dev/null
	    return 1
	fi
	
    fi

    stage="compilation ($cmake_params)"
    printlog "* compiling $dir with \"$cmake_params\" ...."
    if ! make -j $nproc install  >> $log 2>&1 ;then
	printlog "## error during compilation"
    else
	printlog "* compilation ok"
	stage="CCS compilation"
	if [ -n "$enable_ccs" ];then
	    printlog "* compiling CCS ..."
	    if ! make -j $nproc ccs install  >> $log 2>&1 ;then
		printlog "## error compiling CCS"
	    else
		printlog "* CCS compilation ok"
	    fi
	fi
    fi

    if [ "$host_arch" == "$arch" ] && [ $deploy_mode -gt 0 ]; then
	printlog "* testing distribution on architecture \"$arch\""
	pushd $install_prefix >& /dev/null
	export LD_LIBRARY_PATH=$install_prefix/lib
	export CHAOS_PREFIX=$install_prefix
	export CHAOS_TOOLS=$CHAOS_PREFIX/tools
	stage="chaos test"
	if $mydir/chaos_test.sh -d tools/test >> $log 2>&1 ;then
	    printlog "* test OK"
	    popd >& /dev/null
	    printlog "* packaging distribution on architecture \"$arch\""
	    stage="chaos packaging ($install_prefix)"
	    if $mydir/chaos_debianizer.sh -i $install_prefix -t development -r >> $log 2>&1 ;then
		printlog "* packaging ok"
	    else
		printlog "## error during packaging"
	    fi
	else
	    printlog "## test FAILED"
	    popd >& /dev/null
	fi
	unset LD_LIBRARY_PATH
	unset CHAOS_PREFIX
	unset CHAOS_TOOLS
    fi
    popd >& /dev/null
}

if [ -n "$fromscratch" ];then
    printlog "* from scratch"
    if [ -z "$inputdir" ];then
	inputdir=$currdir"/build_chaos_bundle"
    fi
    if [ -d "$inputdir" ];then
	printlog "* removing $inputdir"
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
	    printlog "* checking for $dir in "`pwd`
	    if [ -d "$dir" ];then
		printlog "* compiling $dir branch:$branch"
		compile_bundle $dir $a $b 
	    fi
	    
	fi
	    
    done
done
