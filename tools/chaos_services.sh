#!/bin/bash
cmd=$1
pushd `dirname $0` > /dev/null
scriptdir=`pwd -P`
popd > /dev/null

if [ -z "$CHAOS_TOOLS" ];then
    export CHAOS_TOOLS=$scriptdir
fi
source $scriptdir/common_util.sh

# CDS_EXEC=ChaosMetadataService
#CDS_CONF=cds.cfg
MDS_EXEC=mds
UI_EXEC=webui
US_EXEC=cu
AGENT_EXEC=agent
if [ -z "$CHAOS_PREFIX" ]; then
    error_mesg "CHAOS_PREFIX environment variables not set"
    exit 1
fi



backend_checks(){
    if [ -z "$CHAOS_DB_SERVERS" ];then
        if ! ps -fe |grep [m]ongod >/dev/null ;then
            error_mesg "mongod not running" ; exit 1
        else
            ok_mesg "mongod check"
        fi
    fi
    if [ -z "$CHAOS_LIVE_SERVERS" ]; then
        if ! ps -fe |grep [e]pmd >/dev/null ;then
            if ! ps -fe |grep [m]emcached >/dev/null;then
                error_mesg "epmd (couchbase) nor memcached  running" ; exit 1
            fi
        else
            ok_mesg "couchbase check"
        fi
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
    info_mesg "Usage :$0 {start|stop|status| start agent| start mds | start webui|start devel | stop webui|stop mds}"
}
start_mds(){
    if [ -n "$CHAOS_MDS" ];then
        if ! [[ "$CHAOS_MDS" =~ localhost ]];then
            echo "* Using $CHAOS_MDS"
            return 1
        fi
    fi
    backend_checks;
    mds_checks;
    if check_proc "$CHAOS_PREFIX/bin/$MDS_EXEC";then
        info_mesg "already running " "MDS.."
    else
        info_mesg "starting MDS..." "($CHAOS_LIVE_SERVERS)($CHAOS_DB_SERVERS)"
        
        run_proc "$CHAOS_SERVICE_ENV $CHAOS_PREFIX/bin/$MDS_EXEC --conf-file $CHAOS_PREFIX/etc/mds.cfg $CHAOS_OVERALL_OPT $CHAOS_MDS_OPT --log-file $CHAOS_PREFIX/log/$MDS_EXEC.$MYPID.log > $CHAOS_PREFIX/log/$MDS_EXEC.$MYPID.std.out 2>&1 &" "$MDS_EXEC"
        #    echo "$CHAOS_PREFIX/bin/$MDS_EXEC --conf-file $CHAOS_PREFIX/etc/mds.cfg $CHAOS_OVERALL_OPT $CHAOS_MDS_OPT --log-file $CHAOS_PREFIX/log/$MDS_EXEC.log" >> $CHAOS_PREFIX/log/$MDS_EXEC.std.out
        
        if execute_command_until_ok "grep \"Data Service published\" $CHAOS_PREFIX/log/$MDS_EXEC.$MYPID.log" 120;then
            info_mesg "MDS:" " `grep \"Data Service published\" $CHAOS_PREFIX/log/$MDS_EXEC.$MYPID.log`"
            sleep 1
            ok_mesg "checking publishing"
            
        else
            nok_mesg "checking publishing"
            return 1
        fi
        info_mesg "waiting " " 5 seconds"
        sleep 5
    fi
}

# start_cds(){
#     cds_checks
#     info_mesg "starting CDS..."
#     check_proc_then_kill "$CDS_EXEC"
#     echo "$CDS_BIN --conf-file $CHAOS_PREFIX/etc/$CDS_CONF --log-file $CHAOS_PREFIX/log/cds.log" > $CHAOS_PREFIX/log/$CDS_EXEC.std.out
#     run_proc "$CDS_BIN --conf-file $CHAOS_PREFIX/etc/$CDS_CONF $CHAOS_OVERALL_OPT --log-file $CHAOS_PREFIX/log/$CDS_EXEC.log >> $CHAOS_PREFIX/log/$CDS_EXEC.std.out 2>&1 &" "$CDS_EXEC"
# }
start_ui(){
    if [ -n "$CHAOS_WEBUI" ];then
        if ! [[ "$CHAOS_WBUI" =~ localhost ]];then
            echo "* Using $CHAOS_WEBUI"
            return 1
        fi
    fi

    if check_proc "$CHAOS_PREFIX/bin/$UI_EXEC";then
        info_mesg "already running " "webui.."
        
    else
        info_mesg "starting " "webui.."
        # check_proc_then_kill "$CHAOS_PREFIX/bin/$UI_EXEC"
        
        run_proc "$CHAOS_SERVICE_ENV $CHAOS_PREFIX/bin/$UI_EXEC --conf-file $CHAOS_PREFIX/etc/webui.cfg $port $CHAOS_OVERALL_OPT --log-file $CHAOS_PREFIX/log/webui.$MYPID.log > $CHAOS_PREFIX/log/$UI_EXEC.$MYPID.std.out 2>&1 &" "$UI_EXEC"
    fi
    #   echo "$CHAOS_PREFIX/bin/$UI_EXEC --conf-file $CHAOS_PREFIX/etc/webui.cfg $port $CHAOS_OVERALL_OPT --log-file $CHAOS_PREFIX/log/webui.log" >> $CHAOS_PREFIX/log/$UI_EXEC.std.out
}

start_agent(){
    if [ -n "$CHAOS_AGENT" ];then
        if ! [[ "$CHAOS_AGENT" =~ localhost ]];then
            echo "* Using $CHAOS_AGENT"
            return 1
        fi
    fi

    if check_proc "$CHAOS_PREFIX/bin/$AGENT_EXEC";then
        info_mesg "already running " "agent.."
        
    else
        info_mesg "starting " "agent"
        run_proc "$CHAOS_SERVICE_ENV $CHAOS_PREFIX/bin/$AGENT_EXEC --conf-file  $CHAOS_PREFIX/etc/agent.cfg $CHAOS_OVERALL_OPT --log-on-file --log-file $CHAOS_PREFIX/log/agent.$MYPID.log > $CHAOS_PREFIX/log/$AGENT_EXEC.$MYPID.std.out 2>&1 &" "$AGENT_EXEC"
    fi
}


start_us(){
    if check_proc  "$CHAOS_PREFIX/bin/$US_EXEC";then
        info_mesg "already running " "UnitServer.."
        
    else
        if [ ! -e "$CHAOS_PREFIX/etc/cu.cfg" ]; then
            warn_mesg "UnitServer configuration file not found in \"$CHAOS_PREFIX/etc/cu.cfg\" " "start skipped"
            return
        fi
        if [ ! -e "$CHAOS_PREFIX/etc/localhost/MDSConfig.json" ]; then
            warn_mesg "localhost configuration file not found in \"$CHAOS_PREFIX/etc/localhost/MDSConfig.json\" " "start skipped"
            return
        fi
        info_mesg "transferring configuration to MDS " "$CHAOS_PREFIX/etc/localhost/MDSConfig.json"
        if ! run_proc "$CHAOS_PREFIX/bin/ChaosMDSCmd --mds-conf $CHAOS_PREFIX/etc/localhost/MDSConfig.json $CHAOS_OVERALL_OPT -r 1 --log-on-file --log-file $CHAOS_PREFIX/log/ChaosMDSCmd.$MYPID.log > $CHAOS_PREFIX/log/ChaosMDSCmd.$MYPID.std.out 2>&1 " "ChaosMDSCmd";then
            error_mesg "failed initialization of " "MDS"
            exit 1
        fi
        info_mesg "wait 5s ..."
        sleep 5
        info_mesg "starting " "$US_EXEC"
        run_proc "$CHAOS_PREFIX/bin/$US_EXEC --conf-file $CHAOS_PREFIX/etc/cu.cfg $CHAOS_OVERALL_OPT --log-on-file 1 --log-file $CHAOS_PREFIX/log/$US_EXEC.$MYPID.log > $CHAOS_PREFIX/log/$US_EXEC.$MYPID.std.out 2>&1 &" "$US_EXEC"
    fi
}

ui_stop()
{
    info_mesg "stopping... " "$UI_EXEC"
    stop_proc "$CHAOS_PREFIX/bin/$UI_EXEC"
}

agent_stop()
{
    info_mesg "stopping... " "$AGENT_EXEC"
    stop_proc "$CHAOS_PREFIX/bin/$AGENT_EXEC"
}


mds_stop()
{
    info_mesg "stopping... " "$MDS_EXEC"
    stop_proc "$CHAOS_PREFIX/bin/$MDS_EXEC"
}

# cds_stop(){
#     info_mesg "stopping CDS..."
#     stop_proc "$CDS_EXEC"
# }

start_all(){
    local status=0
    info_mesg "start all chaos services..."
    #    start_cds
    #    status=$((status + $?))
    start_mds
    status=$((status + $?))
    
    start_ui
    status=$((status + $?))
    
    
}
stop_all(){
    local status=0
    info_mesg "stopping all chaos services..."
    
    ui_stop
    status=$((status + $?))
    mds_stop
    status=$((status + $?))
    #    cds_stop
    #    status=$((status + $?))
    agent_stop
    status=$((status + $?))
    if [ -n "$(get_pid $CHAOS_PREFIX/bin/$US_EXEC)" ];then
        stop_proc "$CHAOS_PREFIX/bin/$US_EXEC"
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
    check_proc "$CHAOS_PREFIX/bin/$MDS_EXEC"
    status=$((status + $?))
    #    check_proc "$CDS_EXEC"
    #    status=$((status + $?))
    check_proc "$CHAOS_PREFIX/bin/$UI_EXEC"
    status=$((status + $?))
    
    check_proc "$CHAOS_PREFIX/bin/$AGENT_EXEC"
    status=$((status + $?))
    
    
    
    
    if [ -n "$(get_pid $CHAOS_PREFIX/bin/$US_EXEC)" ];then
        check_proc "$CHAOS_PREFIX/bin/$US_EXEC"
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
            start_agent
            
            
        else
            case "$2" in
                mds)
                    start_mds
                    exit 0
                ;;
                webui)
                    start_ui
                    exit 0
                ;;
                agent)
                    start_agent
                    exit 0
                ;;
                
                devel)
                    start_all
                    start_us
                    start_agent
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
                # cds)
                #     cds_stop
                #     exit 0
                #     ;;
                agent)
                    agent_stop
                    exit 0
                ;;
                webui)
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
