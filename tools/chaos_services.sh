#!/bin/bash
cmd=$1
pushd `dirname $0` > /dev/null
scriptdir=`pwd -P`
popd > /dev/null

if [ -z "$CHAOS_TOOLS" ];then
    export CHAOS_TOOLS=$scriptdir
fi
source $scriptdir/common_util.sh

CDS_EXEC=ChaosDataService
CDS_CONF=cds.cfg
MDS_EXEC=ChaosMetadataService
UI_EXEC=CUIserver
US_EXEC=UnitServer
WAN_EXEC=ChaosWANProxy
if [ -z "$CHAOS_PREFIX" ]; then
    error_mesg "CHAOS_PREFIX environment variables not set"
    exit 1
fi



cds_checks(){

    if [ -x "$CHAOS_PREFIX/bin/$CDS_EXEC" ]; then
	CDS_BIN=$CHAOS_PREFIX/bin/$CDS_EXEC
    else
	error_mesg "$CDS_EXEC binary not found in $CHAOS_PREFIX/bin"
	exit 1
    fi

     if [ ! -e "$CHAOS_PREFIX/etc/$CDS_CONF" ]; then
     	error_mesg "# CDS configuration file \"$CDS_CONF\" not found in $CHAOS_PREFIX/etc/$CDS_CONF"
     	exit 1
     fi

    if ! ps -fe |grep [m]ongod >/dev/null ;then
	error_mesg "mongod not running" ; exit 1
    else
	ok_mesg "mongod check"
    fi
    if ! ps -fe |grep [e]pmd >/dev/null ;then
	if ! ps -fe |grep [m]emcached >/dev/null;then
	    error_mesg "epmd (couchbase) nor memcached  running" ; exit 1
	fi
    else
	ok_mesg "couchbase check"
    fi

}

mds_checks(){
    MDS_LOG=$CHAOS_PREFIX/log/ChaosMetadataService.log
    mkdir -p $CHAOS_PREFIX/log
    if [ -x "$CHAOS_PREFIX/bin/$MDS_EXEC" ]; then
	     MDS_BIN=$CHAOS_PREFIX/bin/$MDS_EXEC
    else
	     error_mesg "$MDS_EXEC binary not found in $CHAOS_PREFIX/bin"
	      exit 1
    fi

}


usage(){
    info_mesg "Usage :$0 {start|stop|status|start mds | start uis| start cds | start wan| |start devel | stop uis|stop mds | stop cds |stop wan}"
}
start_mds(){
    mds_checks;
    info_mesg "starting MDS..."
    check_proc_then_kill "$MDS_EXEC"
    cd "$MDS_DIR"
    run_proc "$MDS_BIN --conf-file $CHAOS_PREFIX/etc/mds.cfg $CHAOS_OVERALL_OPT --log-file $CHAOS_PREFIX/log/$MDS_EXEC.log > $CHAOS_PREFIX/log/$MDS_EXEC.std.out 2>&1 &" "$MDS_EXEC"
    cd - > /dev/null
}

start_cds(){
    cds_checks
    info_mesg "starting CDS..."
    check_proc_then_kill "$CDS_EXEC"
    echo "$CDS_BIN --conf-file $CHAOS_PREFIX/etc/$CDS_CONF --log-file $CHAOS_PREFIX/log/cds.log" > $CHAOS_PREFIX/log/$CDS_EXEC.std.out
    run_proc "$CDS_BIN --conf-file $CHAOS_PREFIX/etc/$CDS_CONF $CHAOS_OVERALL_OPT --log-file $CHAOS_PREFIX/log/$CDS_EXEC.log >> $CHAOS_PREFIX/log/$CDS_EXEC.std.out 2>&1 &" "$CDS_EXEC"
}
start_ui(){
    port=8081
    info_mesg "starting UI Server on port " "$port"
    check_proc_then_kill "$UI_EXEC"
    run_proc "$CHAOS_PREFIX/bin/$UI_EXEC --direct-io-priority-server-port 1690 --direct-io-service-server-port 30200 --server_port $port --log-on-file --log-file $CHAOS_PREFIX/log/$UI_EXEC.log $CHAOS_OVERALL_OPT --log-level debug > $CHAOS_PREFIX/log/$UI_EXEC.std.out 2>&1 &" "$UI_EXEC"
}
start_wan(){
    port=8082
    info_mesg "starting WAN Server on port " "$port"
    check_proc_then_kill "$WAN_EXEC"
    if [ ! -e "$CHAOS_PREFIX/etc/wan.cfg" ]; then
	     warn_mesg "Wan proxy configuration file not found in \"$CHAOS_PREFIX/etc/WanProxy.conf\" " "start skipped"
	      return
    fi
    run_proc "$CHAOS_PREFIX/bin/$WAN_EXEC --conf-file $CHAOS_PREFIX/etc/wan.cfg $CHAOS_OVERALL_OPT --log-on-file $CHAOS_PREFIX/log/$WAN_EXEC.log > $CHAOS_PREFIX/log/$WAN_EXEC.std.out 2>&1 &" "$WAN_EXEC"
}

start_us(){
    info_mesg "starting Unit Server " "$US_EXEC"
    check_proc_then_kill "$US_EXEC"
    if [ ! -e "$CHAOS_PREFIX/etc/cu.cfg" ]; then
	     warn_mesg "UnitServer configuration file not found in \"$CHAOS_PREFIX/etc/cu.cfg\" " "start skipped"
	      return
    fi
    if [ ! -e "$CHAOS_PREFIX/etc/localhost/MDSConfig.txt" ]; then
	     warn_mesg "localhost configuration file not found in \"$CHAOS_PREFIX/etc/localhost/MDSConfig.txt\" " "start skipped"
	      return
    fi
    info_mesg "transferring configuration to MDS " "$CHAOS_PREFIX/etc/localhost/MDSConfig.cfg"
    if ! run_proc "$CHAOS_PREFIX/bin/ChaosMDSCmd --mds-conf $CHAOS_PREFIX/etc/localhost/MDSConfig.txt $CHAOS_OVERALL_OPT -r 1 --log-on-file $CHAOS_PREFIX/log/ChaosMDSCmd.log > $CHAOS_PREFIX/log/ChaosMDSCmd.std.out 2>&1 " "ChaosMDSCmd";then
	error_mesg "failed initialization of " "MDS"
	exit 1
    fi
    
    run_proc "$CHAOS_PREFIX/bin/$US_EXEC --conf-file $CHAOS_PREFIX/etc/cu.cfg $CHAOS_OVERALL_OPT --log-on-file $CHAOS_PREFIX/log/$US_EXEC.log > $CHAOS_PREFIX/log/$US_EXEC.std.out 2>&1 &" "$US_EXEC"
}

ui_stop()
{
    info_mesg "stopping UI Server..."
    stop_proc "$UI_EXEC"
}
wan_stop()
{
    info_mesg "stopping WAN Server..."
    stop_proc "$WAN_EXEC"
}

mds_stop()
{
    info_mesg "stopping MDS..."
    stop_proc "$MDS_EXEC"
}

cds_stop(){
    info_mesg "stopping CDS..."
    stop_proc "$CDS_EXEC"
}
start_all(){
    local status=0
    info_mesg "start all chaos services..."
    start_cds
    status=$((status + $?))
    start_mds
    status=$((status + $?))
    start_ui
    status=$((status + $?))
    start_wan
    status=$((status + $?))

    
}
stop_all(){
    local status=0
    info_mesg "stopping all chaos services..."
    wan_stop
    status=$((status + $?))
    ui_stop
    status=$((status + $?))
    mds_stop
    status=$((status + $?))
    cds_stop
    status=$((status + $?))
    if [ -n "$(get_pid $US_EXEC)" ];then
	     stop_proc "$US_EXEC"
	      status=$((status + $?))
    fi
    exit $status
}

status(){
    local status=0
    check_proc "mongod"
    status=$((status + $?))
    check_proc "epmd"
    status=$((status + $?))
    check_proc "$MDS_EXEC"
    status=$((status + $?))
    check_proc "$CDS_EXEC"
    status=$((status + $?))
    check_proc "$UI_EXEC"
    status=$((status + $?))


    if [ -n "$(get_pid $WAN_EXEC)" ];then
	check_proc "$WAN_EXEC"
    fi


    if [ -n "$(get_pid $US_EXEC)" ];then
	check_proc "$US_EXEC"
    fi

    exit $status
}
case "$cmd" in
    status)
	status
	exit 0
	;;
    start)
	if [ -z "$2" ]; then
	    start_all
	else
	    case "$2" in
		mds)
		    start_mds
		    exit 0
		    ;;
		cds)
		    start_cds
		    exit 0
		    ;;
	       uis)
		    start_ui
		    exit 0
		    ;;
	       wan)
		    start_wan
		    exit 0
		    ;;
	       devel)
		   start_all
		   start_us
		    exit 0
		    ;;
		*)
		    error_mesg "\"$2\" no such service"
		    usage
		    ;;
	    esac

	fi

	;;
    stop)
	if [ -z "$2" ]; then
	    stop_all
	else
	    case "$2" in
		mds)
		    mds_stop
		    exit 0
		    ;;
		cds)
		    cds_stop
		    exit 0
		    ;;
		uis)
		    ui_stop
		    exit 0
		    ;;
		wan)
		    ui_stop
		    exit 0
		    ;;
		*)
		    error_mesg "\"$2\" no such service"
		    usage
		    ;;
	    esac

	fi
	;;
    *)
	usage
	exit 1
	;;
esac
