
OS=`uname -s`
ARCH=`uname -m`

SCRIPTTESTPATH=$0
pushd `dirname $0` > /dev/null
SCRIPTTESTABSPATH=`pwd -P`
SCRIPTNAME=`basename $SCRIPTTESTABSPATH`
popd > /dev/null
export LC_ALL="en_US.UTF-8"
CHAOS_OVERALL_OPT="--event-disable 1"
if [ -n "$CHAOS_INTERFACE" ];then
    CHAOS_OVERALL_OPT="$CHAOS_OVERALL_OPT --publishing-interface $CHAOS_INTERFACE"
fi

if [ -n "$CHAOS_TOOLS" ];then
    tools=$CHAOS_TOOLS
else
    if [ -n "$CHAOS_FRAMEWORK" ];then
	tools=$CHAOS_FRAMEWORK/tools
    else
	if [ -e $SCRIPTTESTABSPATH/common_util.sh ];then
	    tools=$SCRIPTTESTABSPATH
	else
	    if [ -n "$CHAOS_PREFIX" ];then
		tools=$CHAOS_PREFIX/tools
	    else
		echo "## cannot find tools directory, please set CHAOS_TOOLS or CHAOS_FRAMEWORK"
	    
		exit 1
	    fi
	fi
    fi
fi
if [ -n "$CHAOS_PREFIX" ];then
    export LD_LIBRARY_PATH=$CHAOS_PREFIX/lib
    export DYLD_LIBRARY_PATH=$CHAOS_PREFIX/lib
fi
KERNEL_VER=$(uname -r)
KERNEL_SHORT_VER=$(uname -r|cut -d\- -f1|tr -d '.'| tr -d '[A-Z][a-z]')
PID=$$
CHAOSTMP="/tmp/""$USER"
TEST_INFO_NAME=$CHAOSTMP"/__chaos_test_info__"
mkdir -p /tmp/$USER >& /dev/null
if [ -z "$NPROC" ];then
    NPROC=$(getconf _NPROCESSORS_ONLN)
fi

if [ $OS == "Darwin" ];then
    if gsed --v >& /dev/null;then
	SED=gsed
    else 
	echo "%% warning  gsed is required, scripts may misbehave, please install brew install gnu-sed"
	SED=sed
    fi
else
    SED=sed
fi
declare -a __testinfo__
for ((cnt=0;cnt<4;cnt++));do
    if [ -z "${__testinfo__[$cnt]}" ];then
	__testinfo__[$cnt]=0
	echo "${__testinfo__[@]}" > $TEST_INFO_NAME
    fi
done


kill_monitor_process(){
    if [ -f $CHAOSTMP/monitor_process ];then
	l=`cat  $CHAOSTMP/monitor_process`
	for i in $l;do
	    info_mesg "killing process " "$i"
	    kill -9 $i
	done
    else
	info_mesg "no process to kill"
    fi
	
}
monitor_processes(){
    procid=$1
    cuid=$2
    echo > $CHAOSTMP/monitor_process
    for i in ${procid[@]};do
	echo $i >> $CHAOSTMP/monitor_process
    done
    while true ;do
	cnt=0
	for i in ${procid[@]};do
	    info_mesg "monitoring " "${cuid[$cnt]}"
	    
	    if ! ps -fe |grep $i > /dev/null; then
		nok_mesg "process $i [ ${cuid[$cnt]} ]"
		info_mesg "exiting..."
		return 1
	    fi
	    ((cnt++))
	done
	sleep 60
    done
    
}

info_mesg(){
    if [ -z "$2" ]; then
	echo -e "* \x1B[1m$1\x1B[22m"
    else
	echo -e "* \x1B[1m$1\x1B[32m$2\x1B[39m\x1B[22m"
    fi
}
error_mesg(){
    if [ -z "$2" ]; then
	echo -e "# \x1B[31m\x1B[1m$1\x1B[22m\x1B[39m"
    else
	echo -e "# \x1B[1m$1\x1B[31m$2\x1B[39m\x1B[22m"
    fi
}

warn_mesg(){
    if [ -z "$2" ]; then
	echo -e "% \x1B[33m\x1B[1m$1\x1B[22m\x1B[39m"
    else
	echo -e "% \x1B[1m$1\x1B[33m$2\x1B[39m\x1B[22m"
    fi
}

ok_mesg(){
    echo -e "* $1 \x1B[32m\x1B[1mOK\x1B[22m\x1B[39m"
}
nok_mesg(){
    echo -e "* $1 \x1B[31m\x1B[1mNOK\x1B[22m\x1B[39m"
}

function unSetEnv(){
#    unset CC
#    unset CXX
    unset CHAOS_STATIC
    unset CHAOS_TARGET
    unset CHAOS_DEVELOPMENT

}

# type target build
function setEnv(){
    local type=$1
    local target=$2
    local build=$3
    local prefix=$4
    if [ -z "$1" ]; then
	## fetch from configuration file
	if [ -e "$CHAOS_BUNDLE/.chaos_config" ]; then
	    source $CHAOS_BUNDLE/.chaos_config
	fi
    else

	if [ "$type" == "static" ]; then
	    export CHAOS_STATIC=true
	fi
	if [ "$target" != "$ARCH" ]; then
	    export CHAOS_TARGET=$target
	fi

	if [ "$build" == "debug" ]; then
	    export CHAOS_DEVELOPMENT="debug"
	fi
	if [ "$build" == "profile" ]; then
	    export CHAOS_DEVELOPMENT="profile"
	fi

	if [ -d "$prefix" ]; then
	    export CHAOS_PREFIX=$prefix
	    PREFIX=$prefix
	else
	    echo "## directory $prefix is invalid"
	    exit 1
	fi
	if [ -e "$CHAOS_BUNDLE/.chaos_config" ]; then
	    cat $CHAOS_BUNDLE/.chaos_config | $SED 's/CHAOS_BUNDLE=.*//g'|$SED 's/CHAOS_PREFIX=.*//g'|$SED 's/CHAOS_BUNDLE=.*//g'|$SED 's/CHAOS_STATIC=.*//g'|$SED 's/CHAOS_TARGET=.*//g'|$SED 's/CHAOS_DEVELOPMENT=.*//g' > /tmp/.chaos_config
	    mv /tmp/.chaos_config $CHAOS_BUNDLE/.chaos_config
	fi
	echo "CHAOS_BUNDLE=$CHAOS_BUNDLE" >> $CHAOS_BUNDLE/.chaos_config
	echo "CHAOS_PREFIX=$CHAOS_PREFIX" >> $CHAOS_BUNDLE/.chaos_config
	echo "CHAOS_STATIC=$CHAOS_STATIC" >> $CHAOS_BUNDLE/.chaos_config
	echo "CHAOS_TARGET=$CHAOS_TARGET" >> $CHAOS_BUNDLE/.chaos_config
	echo "CHAOS_DEVELOPMENT=$CHAOS_DEVELOPMENT" >> $CHAOS_BUNDLE/.chaos_config
    fi
    info_mesg "CHAOS_BUNDLE  :" "$CHAOS_BUNDLE"
    info_mesg "Host Arch     :" "$ARCH"
    info_mesg "Target        :" "$target"
    info_mesg "Type          :" "$type"
    info_mesg "Configuration :" "$build"
    info_mesg "Prefix        :" "$prefix"
    info_mesg "OS            :" "$OS"
    if [ -n "$CHAOS_EXCLUDE_DIR" ]; then
	info_mesg "Excluding :" "$CHAOS_EXCLUDE_DIR"
    fi

}

function saveEnv(){
  echo "SOURCE=\"\${BASH_SOURCE[0]}\"" > $PREFIX/chaos_env.sh
  echo 'pushd `dirname $SOURCE` > /dev/null' >> $PREFIX/chaos_env.sh
  echo 'SCRIPTPATH=`pwd -P`' >> $PREFIX/chaos_env.sh
  echo "popd > /dev/null" >> $PREFIX/chaos_env.sh

  echo "echo \"* Environment $tgt\"" >> $PREFIX/chaos_env.sh
    if [ -n "$CHAOS_DEVELOPMENT" ];then
	     echo "export CHAOS_DEVELOPMENT=true" >> $PREFIX/chaos_env.sh
    fi
    if [ -n "$CHAOS_TARGET" ];then
	     echo "export CHAOS_TARGET=$CHAOS_TARGET" >> $PREFIX/chaos_env.sh
    fi
    echo "export CHAOS_PREFIX=\$SCRIPTPATH" >> $PREFIX/chaos_env.sh
    echo "export CHAOS_TOOLS=\$CHAOS_PREFIX/tools" >> $PREFIX/chaos_env.sh
#    echo "if [ -z \"\$CHAOS_BUNDLE\" ];then" >> $PREFIX/chaos_env.sh
#    echo -e "\texport CHAOS_BUNDLE=\$CHAOS_PREFIX" >> $PREFIX/chaos_env.sh
#    echo "fi" >> $PREFIX/chaos_env.sh
    if [ -n "$CHAOS_STATIC" ];then
	echo "export CHAOS_STATIC=true" >> $PREFIX/chaos_env.sh
    else
	if [[ "$CHAOS_TARGET" =~ Linux ]];then
	    echo "export LD_LIBRARY_PATH=\$CHAOS_PREFIX/lib" >> $PREFIX/chaos_env.sh
	else
	    echo "export LD_LIBRARY_PATH=\$CHAOS_PREFIX/lib" >> $PREFIX/chaos_env.sh
	    echo "export DYLD_LIBRARY_PATH=\$CHAOS_PREFIX/lib" >> $PREFIX/chaos_env.sh
	fi

    fi

    echo "export PATH=\$PATH:\$CHAOS_PREFIX/bin:\$CHAOS_PREFIX/tools" >> $PREFIX/chaos_env.sh


}


function chaos_configure(){
    info_mesg "configuring"
    if [ -z "$CHAOS_BUNDLE" ] || [ -z "$CHAOS_PREFIX" ]; then
	error_mesg "CHAOS_BUNDLE (sources) and CHAOS_PREFIX (install dir) environments must be set"
	exit 1
    fi

    saveEnv

#    cp -a $CHAOS_BUNDLE/tools $PREFIX
    mkdir -p $PREFIX/etc
    mkdir -p $PREFIX/vfs
    mkdir -p $PREFIX/log
    mkdir -p $PREFIX/chaosframework
    mkdir -p $PREFIX/doc

    mkdir -p $PREFIX/www/html
    
    path=`echo $PREFIX/vfs|$SED 's/\//\\\\\//g'`
    logpath=`echo $PREFIX/log/cds.log|$SED 's/\//\\\\\//g'`
    echo -e "metadata-server=localhost:5000\nlog-level=debug\nevent-disable=1\n" > $PREFIX/etc/cu-localhost.cfg
    echo -e "metadata-server=localhost:5000\nlog-level=debug\nserver_port=8081\nevent-disable=1\n" > $PREFIX/etc/cuiserver-localhost.cfg

    cp -r $CHAOS_BUNDLE/chaosframework/Documentation/html $PREFIX/doc/ >& /dev/null
    cp -r $CHAOS_BUNDLE/service/webgui/w3chaos/public_html/* $PREFIX/www/html
    
    cat $CHAOS_BUNDLE/chaosframework/ChaosDataService/__template__cds.conf | $SED s/_CACHESERVER_/localhost/|$SED s/_DOMAIN_/$tgt/|$SED s/_VFSPATH_/$path/g |$SED s/_CDSLOG_/$logpath/g > $PREFIX/etc/cds-localhost.cfg
    pushd $PREFIX/etc > /dev/null
    ln -sf cds-localhost.cfg cds.cfg
    ln -sf cuiserver-localhost.cfg webui.cfg
    ln -sf cu-localhost.cfg cu.cfg
    
    popd > /dev/null
    pushd $PREFIX/bin > /dev/null
    ln -sf CUIserver webui
    ln -sf ChaosDataService cds
    ln -sf ChaosMetadataService mds
    popd > /dev/null
    $SED 's/run_mode=.*/run_mode=1/' $PREFIX/etc/cds-localhost.cfg | $SED 's/vfs_storage_driver_kvp=.*/vfs_storage_driver_kvp=posix_root_path:\/dev\/null/g' > $PREFIX/etc/cds_noidx.cfg
    if [ -e $CHAOS_BUNDLE/chaosframework/ChaosMDSLite ]; then
	ln -sf $CHAOS_BUNDLE/chaosframework/ChaosMDSLite $PREFIX/chaosframework
    fi
    if [ -e  "$PREFIX/chaosframework/ChaosMDSLite/src/main/webapp/META-INF/context_template.xml" ];then
	cp $PREFIX/chaosframework/ChaosMDSLite/src/main/webapp/META-INF/context_template.xml $PREFIX/chaosframework/ChaosMDSLite/src/main/webapp/META-INF/context.xml
    fi

    if [ -e $CHAOS_BUNDLE/chaosframework/ChaosMetadataService/__template_mds.cfg ]; then
	

	logpath=`echo $PREFIX/log/mds.log|$SED 's/\//\\\\\//g'`

	cat $CHAOS_BUNDLE/chaosframework/ChaosMetadataService/__template_mds.cfg | $SED s/_MDSSERVER_/localhost/|$SED s/_MDSLOG_/mds.log/g > $PREFIX/etc/mds.cfg

    fi
    find $PREFIX -name ".git" -exec rm -rf \{\} \; >& /dev/null
}

get_pid(){
    local execname=`echo $1 | $SED 's/\(.\)/[\1]/'`
    ps -fe |grep -v "$SCRIPTNAME" |grep "$execname" | $SED 's/\ \+/\ /g'| $SED 's/^ //'  | cut -d ' ' -f 2|tr '\n' ' '

}
time_format="+%s.%N"
if [ "$OS" == "Darwin" ]; then
    time_format="+%s"
fi

stop_proc(){
    pid=`get_pid "$1"`
    for p in $pid;do
	if [ -n "$p" ]; then
	    if ! kill -9 $p ; then
		error_mesg "cannot kill process $p"
		exit 1
	    else
		ok_mesg "process $1 ($p) killed"
	    fi

	else
	    warn_mesg "process $1 ($p) " "not running"
	fi
    done
}


get_cpu_stat(){
    local cpu=0
    info=`ps -o pcpu $1| tail -1`
    if [[ "$info" =~ ([0-9\.]+) ]]; then
	cpu=${BASH_REMATCH[1]}
	read -r -a __testinfo__ <$TEST_INFO_NAME
	__testinfo__[0]=`echo "(${__testinfo__[0]} + $cpu)"|bc`
	((__testinfo__[1]++))
	echo "${__testinfo__[@]}" >$TEST_INFO_NAME
	echo "$cpu"
	return 0
    else
	echo "0"
	return 1
    fi
}
get_mem_stat(){
    local mem=0
    info=`ps -o pmem $1| tail -1`
    if [[ "$info" =~ ([0-9\.]+) ]]; then
	mem=${BASH_REMATCH[1]}
	read -r -a __testinfo__ <$TEST_INFO_NAME
	__testinfo__[2]=`echo "(${__testinfo__[2]} + $mem)"|bc`
	((__testinfo__[3]++))

	echo "${__testinfo__[@]}" >$TEST_INFO_NAME
	echo "$mem"
	return 0
    else
	echo "0"
	return 1
    fi
}
check_proc(){
    local xstatus=0
    proc_list=()
    pid=`get_pid "$1"`
    if [ -z "$pid" ]; then
	nok_mesg "process \x1B[1m$1\x1B[22m is not running"
	((xstatus++))
    fi
    for p in $pid;do
	if [ -n "$p" ]; then
	    cpu=$(get_cpu_stat $p)
	    mem=$(get_mem_stat $p)
	    if [ $(echo "($cpu - 50)>0" | bc) -gt 0 ]; then
		cpu="\x1B[31m$cpu%\x1B[39m"
	    else
		cpu="\x1B[1m$cpu%\x1B[22m"
	    fi
	    if [ $(echo "($mem - 50)>0" |bc) -gt 0 ]; then
		mem="\x1B[31m$mem%\x1B[39m"
	    else
		mem="\x1B[1m$mem%\x1B[22m"
	    fi

	    ok_mesg "process \x1B[1m$1\x1B[22m is running with pid \x1B[1m$p\x1B[22m cpu $cpu, mem $mem"

	    proc_list+=($p)
	else
	    nok_mesg "process \x1B[1m$1\x1B[22m is not running"
	    ((xstatus++))
	fi
    done
    return $xstatus
}

check_proc_then_kill(){
    pid=`get_pid "$1"`
    if [ -n "$pid" ]; then
	warn_mesg "process $1 is running with pid \"$pid\" " "killing"
	stop_proc $1;
    fi
}
run_proc(){
    command_line="$1"
    process_name="$2"
    proc_pid=0
    local run_prefix="$CHAOS_RUNPREFIX"
    local oldpid=`get_pid $process_name`
    local oldpidl=()
    if [ $? -eq 0 ] && [ -n "$oldpid" ]; then
	oldpidl=($oldpid)
    fi

    if [ -z "$run_prefix" ];then
	eval $command_line
    else
	if [ -n "$CHAOS_RUNOUTPREFIX" ];then

	    run_prefix="$run_prefix $CHAOS_RUNOUTPREFIX$CHAOS_PREFIX/log/data_$process_name.log"
	fi
	eval "$run_prefix $command_line"
	
    fi
   
    if [ $? -eq 0 ]; then
	pid=$!
	sleep 1
	local pidl=()
#	pid=`get_pid $process_name`
	if [ $? -eq 0 ] && [ -n "$pid" ]; then
	    pidl=($pid)
	fi

	if [ -n $pid ];then
#	    local p=${pidl[$((${#pidl[@]} -1))]}
	    p=$pid
	    ok_mesg "process \x1B[32m\x1B[1m$process_name\x1B[21m\x1B[39m with pid \"$p\", started"
	    proc_pid=$p

	    return 0
	else
	    nok_mesg "process $process_name ($command_line) quitted unexpectly "
	    exit 1
	fi

    else
	error_mesg "error lunching $process_name"
	exit 1
    fi
    return 0
}

test_services(){

    if  $tools/chaos_services.sh status ; then
	ok_mesg "chaos services"
	return 0
    else
	nok_mesg "chaos services"
	return 1
    fi
}
start_services(){

    if $tools/chaos_services.sh start mds; then
	ok_mesg "chaos start MDS"

    else
	nok_mesg "chaos start MDS"
	return 1
    fi

    if $tools/chaos_services.sh start cds; then
	ok_mesg "chaos start CDS"

    else
	nok_mesg "chaos start CDS"
	return 1
    fi

    if $tools/chaos_services.sh start uis; then
	ok_mesg "chaos start UIS"

    else
	nok_mesg "chaos start UIS"
	return 1
    fi
}
start_mds(){
    if $tools/chaos_services.sh start mds; then
	ok_mesg "chaos start mds"
	return 0
    else
	nok_mesg "chaos start mds"
	return 1
    fi
}

test_prefix(){
    if [ -z "$CHAOS_PREFIX" ];then
	error_mesg "environment CHAOS_PREFIX not set, please set it"
	exit 1
    else
	info_mesg "Prefix $CHAOS_PREFIX"
    fi
}

get_abs_filename() {

  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}

get_abs_dir() {

  echo "$(cd "$1" && pwd)"
}
get_abs_parent_dir() {

  echo "$(cd "$(dirname "$1")" && pwd)"
}

find_cu_conf(){
    find $CHAOS_PREFIX/etc -name "*.conf"
}

build_mds_conf(){
    local ncu=$1
    local nus=$2
    local out=$3
    local dataserver="$4"
    local lista_conf=""
    local ncutype=0
    local lista_file=$(find_cu_conf)
    local cuname=$5
    local include="$6"
    for l in $lista_file;do
	if [ -z "$include" ];then
	    ((ncutype++))
	else
	    for ll in $include;do
		if [[ $l =~ $ll ]];then
		    ((ncutype++))
		fi
	    done
	fi
    done
    if [ $ncutype -eq 0 ];then
	error_mesg "at least one configuration file must be provided"
	return 1
    fi

    for l in $lista_file;do
	local n=$((ncu/ncutype))
	if [ -z "$include" ];then
	    lista_conf="$lista_conf -i $l -n $n"
	    info_mesg "generating $n CU from " "$l"
	else
	    for ll in $include;do
		if [[ $l =~ $ll ]];then
		    lista_conf="$lista_conf -i $l -n $n"
		    info_mesg "generating $n CU from " "$l"
		fi
	    done


	fi
    done

    local param="$lista_conf -j $nus -o $out"
    if [ -n "$dataserver" ];then
	param="$param -d $dataserver"
    fi

    if [ -n "$cuname" ];then
	param="$param -c $cuname"
    fi


    if [ -n "$lista_conf" ] && $CHAOS_TOOLS/chaos_build_conf.sh $param >& /dev/null;then
	ok_mesg "MDS configuration generated"
	return 0
    else
	nok_mesg "MDS configuration generated"
	return 1
    fi
}

_start_profile_time=0
_end_profile_time=0
function start_profile_time(){
    _start_profile_time=`date $time_format`
}

function end_profile_time(){
    _end_profile_time=`date $time_format`
    echo "$_end_profile_time - $_start_profile_time"|bc
}

execute_command_until_ok(){
    local command="$1"
    local _cnt_=$2
    local _ok_=0

    while (((_cnt_ > 0) && (_ok_==0) ));do
	execute_command=`eval $command`
	if [ $? -eq 0 ] ;then
	    _ok_=1
	else
	    echo -n "."
	    ((_cnt_--))
	    sleep 1
	fi

    done
   if [ $_cnt_ -lt $2 ]; then
       echo
   fi
   if [ $_ok_ -eq 0 ]; then
       return 1

   fi

   return 0

}

chaos_cli_cmd(){
    local meta="$1"
    local cuname="$2"
    local param="$3"
    local timeout=10000
    cli_cmd=""
    if [ "$CHAOS_RUNTYPE" == "callgrind" ]; then
	timeout=$((timeout * 10))
    fi
    cli_cmd=`$CHAOS_PREFIX/bin/ChaosCLI --log-on-file $CHAOS_TEST_DEBUG --log-file $CHAOS_PREFIX/log/ChaosCLI.log --metadata-server $meta --device-id $cuname --timeout $timeout $param 2>&1`

    if [ $? -eq 0 ]; then
	return 0
    fi
    error_mesg "Error \"$CHAOS_PREFIX/bin/ChaosCLI --metadata-server $meta --device-id $cuname --timeout $timeout $param \" returned:$cli_cmd"
    return 1

}
# meta cuname
init_cu(){
    chaos_cli_cmd $1 $2 "--op 1"
}
start_cu(){
    chaos_cli_cmd $1 $2 "--op 2"
}
deinit_cu(){
    chaos_cli_cmd $1 $2 "--op 4"
}
stop_cu(){
    chaos_cli_cmd $1 $2 "--op 3"
}

get_timestamp_cu(){
    local meta="$1"
    local cuname="$2"

    timestamp_cu=0
    if ! chaos_cli_cmd $meta $cuname "--print-dataset 0";then
	return 1
    fi

    if [[ "$cli_cmd" =~ \"dpck_ats\"\ \:\ \{\ \"\$[a-zA-Z]+\"\ \:\ \"([0-9]+)\" ]];then
	timestamp_cu=${BASH_REMATCH[1]}
	return 0
    else
	error_mesg "cannot get timestamp from: \"$cli_cmd\""
    fi

    return 1
}

get_dataset_cu(){
    local meta="$1"
    local cuname="$2"
    local type="$3"
    if [ -z "$type" ];then
	type=0
    fi

    if ! chaos_cli_cmd $meta $cuname "--print-dataset $type";then
	return 1
    fi

    if [[ "$cli_cmd" =~ \"dpck_ats\"\ \:\ \{\ \"\$[a-zA-Z]+\"\ \:\ \"([0-9]+)\" ]];then
	timestamp_cu=${BASH_REMATCH[1]}
    else
	return 1
    fi

    dataset_cu="$cli_cmd"
    return 0
}

get_hdataset_cu(){
    local meta="$1"
    local cuname="$2"
    local st_time="$3"
    local end_time="$4"
    local filename="$5"
    info_mesg "dumping historical data for cu $cuname interval " "$end_time-$st_time"
    if ! $CHAOS_PREFIX/bin/ChaosDataExport --metadata-server $meta --device-id $cuname --start-time $st_time --end-time $end_time --dest-file $filename --dest-type 1 > $CHAOS_PREFIX/log/ChaosDataExport.log 2>&1 ;then
	return 1
    fi
    return 0
}
loop_cu_test(){
    local meta="$1"
    local cuname="$2"
    local max="$3"
    local pid="$4"
    local cnt=0
    local t1=0
    local cpu=0
    local mem=0
    local oldcpu=0
    local oldmem=0

    for ((cnt=0;cnt<$max;cnt++));do

	oldcpu=$cpu
	oldmem=$mem
	cpu=$(get_cpu_stat $pid)
	mem=$(get_mem_stat $pid)
	info_mesg "loop cu test $cnt on CU $cuname, cpu:$cpu% mem:$mem%"
	if [ $cnt -eq 0 ];then
	    oldcpu=$cpu
	    oldmem=$mem
	fi
	cpudiff=$(echo "($cpu - $oldcpu)" |bc)
	memdiff=$(echo "($mem - $oldmem)" |bc)
	if [ $(echo "($cpudiff > 1)"|bc) -gt 0 ];then
	    warn_mesg "cpu occupation \x1B[1m$cpu%\x1B[22m increased respect previous cycle \x1B[1m$oldcpu%\x1B[22m by " "$cpudiff%"
	fi
	if [ $(echo "($memdiff > 0)"|bc) -gt 0 ];then
	    warn_mesg "mem occupation \x1B[1m$mem%\x1B[22m increased respect previous cycle \x1B[1m$oldmem\x1B[22m by " "$memdiff%"
	fi
	if init_cu $meta $cuname;then
	    ok_mesg "- $cnt init $cuname cpu $cpu% mem $mem%"
	else
	    nok_mesg "- $cnt init $cuname"
	    return 1
	fi
	cpu=$(get_cpu_stat $pid)
	mem=$(get_cpu_stat $pid)
	if start_cu $meta $cuname;then
	    ok_mesg "- $cnt start $cuname $cpu% mem $mem%"
	else
	    nok_mesg "- $cnt start $cuname"
	    return 1
	fi
	sleep 1
	if get_timestamp_cu $meta $cuname;then
	    ok_mesg "- $cnt get timestamp $timestamp_cu"

	    if [ $t1 -gt 0 ];then
		res=$((timestamp_cu -t1))
		if [ $res -gt 0 ]; then
		    info_mesg "cu $cuname is living loop time" " $res ms"
		else
		    warn_mesg "cu $cuname " "not progressing"
		fi
	    fi
	    t1=$timestamp_cu
	else
	    nok_mesg "- $cnt get timestamp $cuname"
	    return 1
	fi
	cpu=$(get_cpu_stat $pid)
	mem=$(get_cpu_stat $pid)
	if stop_cu $meta $cuname;then
	    ok_mesg "- $cnt stop $cuname $cpu% mem $mem%"
	else
	    nok_mesg "- $cnt stop $cuname"
	    return 1
	fi
	cpu=$(get_cpu_stat $pid)
	mem=$(get_cpu_stat $pid)
	if deinit_cu $meta $cuname;then
	    ok_mesg "- $cnt deinit $cuname $cpu% mem $mem%"
	else
	    nok_mesg "- $cnt deinit $cuname"
	    return 1
	fi
    done
    return 0
}

launch_us_cu(){
    local USNAME=UnitServer
    local NUS=2
    local NCU=5
    local META="localhost:5000"
    local ALIAS="TEST_UNIT"
    if [ -n "$1" ];then
	NUS=$1
    fi
    if [ -n "$2" ];then
	NCU=$2
    fi
    if [ -n "$3" ];then
	META="$3"
    fi
    if [ -n "$4" ];then
	USNAME="$4"
    fi
    if [ -n "$5" ];then
	ALIAS="$5"
    fi

    if [ -n "$6" ];then
	CHECK_REGISTRATION="$6"
    fi

#    check_proc_then_kill "$USNAME"
    if [ ! -x $CHAOS_PREFIX/bin/$USNAME ]; then
	nok_mesg "Unit Server $USNAME not found, in $CHAOS_PREFIX/bin/$USNAME"
	exit 1
    fi

    us_proc=()

    info_mesg "launching " "$NUS $USNAME with $NCU CU"

    for ((us=0;us<$NUS;us++));do
	if [ $NUS -eq 1 ] && [ "$ALIAS" != "TEST_UNIT" ]; then
	    REAL_ALIAS=$ALIAS
	else
	    REAL_ALIAS="$ALIAS"_$us
	fi
	info_mesg "launching US[$us] alias " "$REAL_ALIAS"
	rm $CHAOS_PREFIX/log/$USNAME-$us.log >& /dev/null

	FILE_NAME=`echo $REAL_ALIAS|$SED 's/\//_/g'`
	echo "$CHAOS_PREFIX/bin/$USNAME $CHAOS_OVERALL_OPT --log-on-file $CHAOS_TEST_DEBUG --log-file $CHAOS_PREFIX/log/$USNAME-$FILE_NAME.log --unit-server-alias $REAL_ALIAS $META"  > $CHAOS_PREFIX/log/$USNAME-$FILE_NAME-$us.stdout
	if run_proc "$CHAOS_PREFIX/bin/$USNAME --log-on-file $CHAOS_TEST_DEBUG $CHAOS_OVERALL_OPT --log-file $CHAOS_PREFIX/log/$USNAME-$FILE_NAME.log --unit-server-alias $REAL_ALIAS $META >> $CHAOS_PREFIX/log/$USNAME-$FILE_NAME-$us.stdout 2>&1 &" "$USNAME"; then
	    ok_mesg "$USNAME \"$REAL_ALIAS\" ($proc_pid) started"
	    us_proc+=($proc_pid)
	else
	    nok_mesg "$USNAME \"$REAL_ALIAS\" started"
            return 1
	fi
	start_profile_time;
	# for ((cu=0;cu<$NCU;cu++));do
	#     if [ $NUS -eq 1 ]; then
	# 	REAL_ALIAS=$ALIAS
	#     else
	# 	REAL_ALIAS=$ALIAS_$us
	#     fi

	#     info_mesg "checking for $REAL_ALIAS of $NCU registrations"
	#     if execute_command_until_ok "grep -o \".\+ successfully registered\" $CHAOS_PREFIX/log/$USNAME-$REAL_ALIAS.log >& /dev/null" 180; then
	# 	t=$(end_profile_time)
	# 	ok_mesg "CU \"TEST_UNIT_$us/TEST_CU_$cu\" registered in $t"
	#     else
	# 	nok_mesg "CU \"TEST_UNIT_$us/TEST_CU_$cu\" registered $t"
	# 	return 1
	#     fi
	# done
	if [ -n "$CHECK_REGISTRATION" ];then
	    var1="((\`grep \"successfully registered\" $CHAOS_PREFIX/log/$USNAME-$FILE_NAME.log |wc -l\` >= $NCU))"
	    if execute_command_until_ok "$var1" 180; then
		t=$(end_profile_time)
		ok_mesg "$NCU  registered in $t"
	    else
		var=$((`grep "successfully registered" $CHAOS_PREFIX/log/$USNAME-$FILE_NAME.log |wc -l`))
		nok_mesg "$var CU registered in $t"
		return 1
	    fi
	fi
    done
}
start_test(){
    data=`date`
    echo "======================================================================================================="
    info_mesg "[ $data ] starting test " "$0"
    for ((cnt=0;cnt<4;cnt++));do
	if [ -z "$__testinfo__[$cnt]" ];then
	    __testinfo__[$cnt]=0
	fi
    done
    echo "${__testinfo__[@]}" >$TEST_INFO_NAME
    __start_test_time__=`date $time_format`

}
end_test(){
    local status=$1
    local desc="--"
    local pcpu="--"
    local pmem="--"
    local __end_test_time__=`date $time_format`
    local __start_test_name__=$SCRIPTNAME
    local __start_test_group__=$(get_abs_parent_dir $SCRIPTTESTPATH)
    local __start_test_group__=`basename $__start_test_group__`
    local exec_time=`echo "scale=3;($__end_test_time__ - $__start_test_time__ )" |bc`
    read -r -a __testinfo__ <$TEST_INFO_NAME

    if [ ${__testinfo__[1]} -gt 0 ];then
	pcpu=`echo "scale=2;${__testinfo__[0]} / ${__testinfo__[1]}" |bc -l`

    fi
    if [ ${__testinfo__[3]} -gt 0 ];then
	pmem=`echo "scale=2;${__testinfo__[2]} / ${__testinfo__[3]}" |bc -l`

    fi

    if [ -n "$2" ];then
	desc=$2
    fi

    if [ -f "$CHAOS_TEST_REPORT" ];then
	local stat="FAILED"
	if [ $status -eq 0 ];then
	    stat="OK"
	fi
	echo "$__start_test_group__;$SCRIPTNAME;$stat;$exec_time;$pcpu;$pmem;$desc" >> $CHAOS_TEST_REPORT
    fi
    exit $status
}

