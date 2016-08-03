#!/bin/bash

separator='-'
pushd `dirname $0` > /dev/null
dir=`pwd -P`
popd > /dev/null

source $dir/common_util.sh
err=0

prefix_build=chaos
outbase=$PWD
create_deb_ver=""
remove_working="false"
log="$0.log"
exclude_dir=()
compile_target=( "$ARCH" )

if arm-linux-gnueabihf-g++-4.8 -v 2>&1 | grep version >& /dev/null; then
    compile_target+=("armhf")
fi

if arm-infn-linux-gnueabi-g++ -v 2>&1 | grep version >& /dev/null;then
    compile_target+=("arm-linux-2.6")
fi

if i686-nptl-linux-gnu-g++ -v 2>&1 | grep version >& /dev/null;then
    compile_target+=("i686-linux26")
fi

if arm-nilrt-linux-gnueabi-g++ -v 2>&1 | grep version >& /dev/null;then
      compile_target+=("crio90xx")
fi

compile_build=("release" "debug" "profile")

if [ "$OS" == "Linux" ]; then
    compile_type=("dynamic static");
else
    compile_type=("dynamic");
fi

type=${compile_type[0]}
target=${compile_target[0]}
build=${compile_build[0]}

while getopts t:o:w:b:p:hd:rsc:kx:n:fei: opt; do
    case $opt in
	i) DEPLOY_SERVER="$OPTARG"
	    info_mesg "deploy server " "$DEPLOY_SERVER"
	    ;;
	t)
	    if [[ ${compile_target[@]} =~ $OPTARG ]]; then 
		compile_target=($OPTARG);
		info_mesg "setting target to " "$compile_target";
	    else
		error_mesg "compile targets one of: ${compile_target[@]}"
		exit 1
	    fi
	    ;;
	e)
	    STRIP_SYMBOLS=ON
	    info_mesg "stripping symbols"
	    ;;
	o) 
	    if [[ ${compile_type[@]} =~ $OPTARG ]]; then 
		compile_type=($OPTARG);
		info_mesg "setting type to " "$compile_type";
	    else
		error_mesg "compile type one of: ${compile_type[@]}"
		exit 1
	    fi
	    ;;
	b) 
	    if [[ ${compile_build[@]} =~ $OPTARG ]]; then 
		compile_build=($OPTARG);
		info_mesg "setting build to " "$compile_build";
	    else
		error_mesg "compile build one of ${compile_build[@]}"
		exit 1
	    fi
	    ;;
	r) 
	    remove_working=true;
	    info_mesg "remove working as done";
	    ;;

	w) 
	    if [ -d "$OPTARG" ]; then
		outbase=$OPTARG
		info_mesg "* Using working directory " "$outbase";
	    else
		error_mesg "bad working directory " "$OPTARG"
		exit -1
	    fi
	    ;;
	p)
	    prefix_build="$OPTARG"
	    info_mesg "prefix " "$prefix_build"
	    ;;
	k)
	    perform_test=true
	    info_mesg "execute test after build"
	    ;;
	d)
	    create_deb_ver="$OPTARG"
	    info_mesg "create debian package version " "$create_deb_ver"
	    ;;
	s)
	    switch_env=true
	    info_mesg "switching environment";
	    ;;
	c)
	    config="$OPTARG"
	    if [ ! -d "$config" ]; then
		error_mesg "directory $config is invalid"
		exit 1
	    else
		info_mesg "configuring environment in " "$config";
	    fi
	    ;;
	x)
	    exclude_dir+=("$OPTARG")
	    ;;
	f)
	    force_rebuild="true"
	    info_mesg "force rebuild" " true"
	    ;;

	n) export NPROC=$OPTARG
	    info_mesg "force to use " "$NPROC processors"
	    ;;
	h)
	    echo -e "Usage is $0 [-w <work directory>] [-k] [-s] [-e] [-t <armhf|$ARCH>] [-o <static|dynamic> [-b <debug|release>] [-p <build prefix>] [-d <deb version>] [-r] [-c <>] [-i <dst copy of compilation tree>]\n-w <work directory>: where directories are generated [$outbase]\n-t <target>: cross compilation target [${compile_target[@]}]\n-o <static|dynamic>: enable static or dynamic compilations [${compile_type[@]}]\n-b <build type> build type [${compile_build[@]}]\n-p <build prefix>: prefix to add to working directory [$prefix_build]\n-d <version>: create a deb package of the specified version\n-r: remove working directory after compilation\n-s:switch environment to precompiled one (skip compilation) [$tgt]\n-c <dir>:configure installation directory (etc,env,tools)\n-k:perform test suite after build\n-x <exclude dir>: exclude the specified directory/library from compilation\n-f:force rebuild all\n-e:strip all symbols\n-i <dstcopy>: generate a tar of the distrib and copy on dstcopy";
	    exit 0;
	    ;;
    esac
done

type=${compile_type[0]}
target=${compile_target[0]}
build=${compile_build[0]}
export CHAOS_EXCLUDE_DIR="${exclude_dir[@]}"
TEMP_PREFIX="/tmp/""$USER/""$target""$separator""$type""$separator""$build"
init_tgt_vars(){
    tgt="$prefix_build""$separator""$target""$separator""$type""$separator""$build"

    PREFIX=`echo "$outbase/$tgt"|sed 's/\/\{2,\}/\//g' | sed 's/\/\w*\/\.\{2\}//g'`
    log=$outbase/$tgt.log
}


init_tgt_vars;




function compile(){
    info_mesg "log on " "$log"

    if [ ! -d "$PREFIX" ] || [ ! -z "$force_rebuild" ];then
	info_mesg "new configuration $PREFIX " "clean all" 
	$dir/chaos_clean.sh
	rm -rf $PREFIX
	info_mesg "compiling " "$tgt ...."

	echo -e '\n\n' | $dir/init_bundle.sh >& $log;
    else
	echo -e '\n\n' | $dir/init_bundle.sh 1 >& $log;

    fi
    grep "error:" $log
    return 0
}


if [ -n "$switch_env" ]; then

    setEnv $type $target $build $PREFIX 
    exit 0
fi

if [ -n "$config" ]; then
    PREFIX=$config

    if ! setEnv $type $target $build $PREFIX; then
	error_mesg "error setting environment check " "$log"
	exit 1
    fi
    chaos_configure
    exit 0
fi



for target in ${compile_target[@]} ; do
    for type in ${compile_type[@]} ; do
	for build in ${compile_build[@]} ; do
	    error=0
	    init_tgt_vars;

	    mkdir -p $PREFIX
	    unSetEnv

	    if ! setEnv $type $target $build $PREFIX; then
		error_mesg "error setting environment check " "$log"
		exit 1
	    fi
	    start_profile_time
	    compile $tgt;
	    if [ $? -ne 0 ]; then 
		((err++))
		error_mesg "Error $err compiling $tgt"
		error=1
	    else
		## configure
		chaos_configure
		if [ "$OS" == "Linux" ]; then
		info_mesg "generating " "Unit Server.."
		echo "==== GENERATING UNIT SERVER ====" >> $log 2>&1 
		if $dir/chaos_generate_us.sh -i $CHAOS_BUNDLE/driver -o $PREFIX -n UnitServer >> $log 2>&1 ; then
		    pushd $PREFIX/UnitServer > /dev/null
		    if cmake . >> $log ; then
			
			if  make install >> $log 2>&1 ; then
			    info_mesg "UnitServer " "successfully installed"
			else
			    error_mesg "error compiling UnitServer \"$log\" for details" 
			    ((err++))
			    error=1
			fi
		    else
			error_mesg "error during Unit Server makefile generation"
			((err++))
			error=1
		    fi
		    popd > /dev/null
		else
		    error_mesg "error during generation of Unit Server"
		    ((err++))
		    error=1
		fi
		else
		    info_mesg "skipping UnitServer on $OS"
		fi

		for i in sccu rtcu common driver;do
		    info_mesg "testing template " "$i"
		    rm -rf $TEMP_PREFIX/_prova_"$i"_
		    echo "==== TESTING TEMPLATE $i ====" >> $log 2>&1 
		    if  $dir/chaos_create_template.sh -o "$TEMP_PREFIX" -n _prova_"$i"_ $i >> $log 2>&1 ;then
			mkdir -p "$TEMP_PREFIX""/_prova_$i"_
			pushd "$TEMP_PREFIX"/_prova_"$i"_ >/dev/null
			if cmake .  >> $log 2>&1 ; then
			    if ! make -j $NPROC >> $log 2>&1 ;then 
				error_mesg "error during compiling $i template"
				((err++))
				error=1
			    else
				ok_mesg "template $i"

			    fi
			else
			    error_mesg "error  generating makefile for $i"
			    ((err++))
			    error=1
			fi
			popd >/dev/null
			rm -rf "$TEMP_PREFIX""/_prova_$i"_
		    else
			error_mesg "error during generation of $i template"
			((err++))
			error=1
			
		    fi

		done
	    fi

	    if (($error == 0)); then
		tt=$(end_profile_time)
		info_mesg "compilation ($tt s) " "$tgt OK"
		if [ -n "$CHAOS_CROSS_HOST" ]; then
		    STRIP_CMD=$CHAOS_CROSS_HOST-strip
		else
		    STRIP_CMD=strip
		fi
		if [ -n "$STRIP_SYMBOLS" ];then
		    info_mesg "stripping " " $STRIP_CMD $PREFIX/bin"
		    $STRIP_CMD $PREFIX/bin/* >& /dev/null
		    info_mesg "stripping " " $STRIP_CMD $PREFIX/lib"
		    $STRIP_CMD $PREFIX/lib/* >& /dev/null
		fi
		if [ -n "$perform_test" ];then
		    if [ "$ARCH" == "$target" ];then
			info_mesg "Starting chaos testsuite (it takes some time), test report file " "test-$tgt.csv"
			start_profile_time
			echo "===== TESTING ====" >> $log 2>&1 
			if [ "$build" == "debug" ];then
			      $PREFIX/tools/chaos_test.sh -g -r test-$tgt.csv >> $log 2>&1 
			else
			    $PREFIX/tools/chaos_test.sh -r test-$tgt.csv >> $log 2>&1 
			fi
			status=$?
			tt=$(end_profile_time)
			if [ $status -eq 0 ];then
			    ok_mesg "TEST RUN ($tt s)"
			else
			    nok_mesg "TEST RUN ($tt s)"
			fi
		    else
			info_mesg "test skipped on cross compilation"
		    fi
		fi
		if [ -n "$create_deb_ver" ]; then
		    nameok=`echo $tgt | sed s/_/-/g`
		    extra="-i $PREFIX -v $create_deb_ver -t $target"
		    if [ -z "$CHAOS_STATIC" ];then
			extra="$extra -d"
		    fi
		    info_mesg "creating debian package " "client"
		    if $dir/chaos_debianizer.sh $extra  >> $log 2>&1; then
			ok_mesg "debian package generated"
		    fi
		    info_mesg "creating debian package " "server"
		    if $dir/chaos_debianizer.sh $extra  -s >> $log 2>&1; then
			ok_mesg "debian package generated"
		    fi
		    info_mesg "creating debian package " "devel"
		    if $dir/chaos_debianizer.sh $extra  -a >> $log 2>&1; then
			ok_mesg "debian package generated"
		    fi
		else
		    if [ -n "$DEPLOY_SERVER" ];then
			str=`date +%H-%M-%Y-%h-%d`
			info_mesg "creating tar " "$PREFIX.$str.tar.gz"
			if tar cfz $PREFIX.$str.tar.gz $PREFIX;then
			    if scp $PREFIX.$str.tar.gz "$DEPLOY_SERVER";then
				ok_mesg "copied to " "$DEPLOY_SERVER"
			    else
				nok_mesg "copied to " "$DEPLOY_SERVER"
			    fi
			else
			    nok_mesg "creating tar " "$PREFIX.$str.tar.gz"
			fi
		    fi
		fi
	    fi
	
	    if [ "$remove_working" == "true" ]; then
		info_mesg "removing " "$nameok"
		rm -rf $PREFIX
	    fi
	    echo 
	done
    done
done;

if [ $err -gt 0 ]; then
    error_mesg "Number of errors $err"
    exit $err
fi 
ok_mesg "building"
exit 0
