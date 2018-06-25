#!/bin/bash -e

separator='-'
pushd `dirname $0` > /dev/null
dir=`pwd -P`
popd > /dev/null

source $dir/common_util.sh
TMPDIR="/tmp/$USER/chaos_deploy"

DEPLOY_FILE=$TMPDIR/deployTargets





rm -rf $TMPDIR
mkdir -p $TMPDIR
echo "" > $DEPLOY_FILE
Usage(){
	echo -e "$0 [-r <cu|mds|web|all>] [-l <"cu,mds,web,wan">] [-i <source dir>] [-u] -c <configuration>\n-r <cu|mds|cds|web|all>: restart\n-s <cu|mds|cds|web|all>: stop\n-u: update MDS configuration\n-c <configuration>:configuration\n-i <source dir>: distribution dir\n-l:<command separated of servers type to update i.e \"cu,mds\" for cus and mds>"
}
while getopts s:r:hc:i:ul: opt; do
 case $opt in
	r) 
	    restart=$OPTARG
	    ;;
	s) 
	    stop=$OPTARG
	    ;;

	i) 
	    sourcedir=$OPTARG
	    ;;
	u)
	    updateconfig="true"
	;;
	c) 
	    conf=$OPTARG
	    ;;
	l) 
	    listupdate=$OPTARG
	    ;;

	h) 
		Usage
	exit 0
	;;
	*)
	Usage
	exit 1
	;;
esac
	
done

if [ -z "$listupdate" ];then
    listupdate="all"
fi
 
if [ ! -e "$conf" ]; then
    nok_mesg "You must specify a valid configuration file"
    exit 1
fi

cudir=`dirname $conf`
cuconfig=`basename $cudir`

if [ -z "$restart" ];then
	if [ ! -d "$sourcedir" ];then
		if [ -z "$CHAOS_PREFIX" ];then
   			echo "## NO environment CHAOS_PREFIX nor '-i' defined"
	   	 	exit 1
		fi
	else
		export CHAOS_PREFIX=$sourcedir
	fi

	
fi

source $conf
if [ -z $DEPLOY_USER ];then
    DEPLOY_USER="chaos"
fi

if [ -z $DEPLOY_PREFIX_DIR ];then
    DEPLOY_PREFIX_DIR=/usr/local/chaos
fi
if [ -z $START_STOP_CMD ];then
    START_STOP_CMD="sudo service chaos-\$type \$op"
fi
dest_prefix=$DEPLOY_PREFIX_DIR/chaos-distrib
deployServer(){
    serv=$1
    if ! grep "$serv" $DEPLOY_FILE >& /dev/null;then
	echo "$serv" >> $DEPLOY_FILE
	echo "* including $serv"
    else
	echo "* skipping $serv"
    fi
}

## MDS ##
if [ -z "$MDS_SERVER" ]; then
    info_mesg "MDS_SERVER " "not specified"
else
    if [ ! -f $cudir/mds.cfg ];then
	error_mesg "missing configuration  " "$cudir/mds.cfg"
	exit 1
    else
	info_mesg "using configuration " "$cudir/mds.cfg"
    fi

    mds="$MDS_SERVER"
#    cat $CHAOS_PREFIX/etc/cu-localhost.cfg | sed "s/localhost/$mds/g" > $CHAOS_PREFIX/etc/cu-$mds.cfg
 #   cat $CHAOS_PREFIX/etc/cuiserver-localhost.cfg | sed "s/localhost/$mds/g" > $CHAOS_PREFIX/etc/cuiserver-$mds.cfg
  #  pushd $CHAOS_PREFIX/etc > /dev/null
  #  ln -sf cu-$mds.cfg cu.cfg
  #  ln -sf cuiserver-$mds.cfg cuiserver.cfg
    #  popd > /dev/null
    if [ $listupdate == "all" ] || [[ $listupdate =~ mds ]];then 
	deployServer $MDS_SERVER
    fi

fi

if [ -n "$updateconfig" ];then

	if [ -z "$CHAOS_PREFIX" ];then
		error_mesg "you must specify a CHAOS_PREFIX or '-i' to update MDS"
		exit 1	
	fi	
	info_mesg "updating configuration of $MDS_SERVER with " "$cudir/MDSConfig.txt"
	if [ ! -f  $cudir/MDSConfig.txt ];then
		error_mesg "cannot find configuration " "$cu_dir/MDSConfig.txt"
		exit 1
	fi	
	if $CHAOS_PREFIX/bin/ChaosMDSCmd --mds-conf $cudir/MDSConfig.txt --metadata-server $MDS_SERVER:5000 >& /dev/null;then
		ok_mesg "configuration set $cudir/MDSConfig.txt"
	else
		nok_mesg "configuration set $cudir/MDSConfig.txt"
		exit 1
	fi	
fi

## WEBUI ##
if [ -z "$WEBUI_SERVER" ]; then
    info_mesg "WEBUI_SERVER " "not specified"
else
    webui="$WEBUI_SERVER"
    if [ ! -f "$cudir/webui.cfg" ];then
	error_mesg "missing configuration " "$cudir/webui.cfg"
	exit 1
    else
	info_mesg "using configuration " "$cudir/webui.cfg"
    fi

#    pushd $CHAOS_PREFIX > /dev/null
#    cp -r $CHAOS_PREFIX/html $CHAOS_PREFIX/www-$webui
#    find $CHAOS_PREFIX/www-$webui -name "*" -exec  sed -i s/__template__webuiulr__/$webui/g \{\} >& /dev/null \; 

#    popd > /dev/null
    if [ $listupdate == "all" ] || [[ $listupdate =~ webui ]];then 
	for i in $WEBUI_SERVER;do
	    deployServer $i	
	done

    fi



fi


## WAN ##
if [ -z "$WAN_SERVER" ]; then
    info_mesg "WAN_SERVER " "not specified"
else
    wan="$WAN_SERVER"
    if [ ! -f "$cudir/wan.cfg" ];then
	error_mesg "missing configuration " "$cudir/wan.cfg"
	exit 1
    else
	info_mesg "using configuration " "$cudir/wan.cfg"
    fi

    if [ $listupdate == "all" ] || [[ $listupdate =~ wan ]];then 
	for i in $WAN_SERVER;do
	    deployServer $i	
	done


    fi


fi



info_mesg "working on " "$cuconfig"

## CDS ##
if [ -z "$CDS_SERVERS" ]; then
    info_mesg "CDS_SERVERS " "not specified"
else
    if [ ! -f $cudir/cds.cfg ];then
	error_mesg "missing configuration " "$cudir/cds.cfg"
	exit 1
    else
	info_mesg "using configuration " "$cudir/cds.cfg"
    fi

    if [ $listupdate == "all" ] || [[ $listupdate =~ cds ]];then
	for i in $CDS_SERVERS;do
	    deployServer $i	
	done

    fi


fi


if [ $listupdate == "all" ] || [[ $listupdate =~ cu ]];then
    for i in $CU_SERVERS;do
	deployServer $i	
    done
fi

start_stop_service(){
    host=$1
    type=$2
    op=$3

    # if [[ "$host" =~ (.+)@(.+) ]];then
    # 	user=${BASH_REMATCH[1]}
    # 	host=${BASH_REMATCH[2]}
    # fi

## new 
    ll=`eval echo $START_STOP_CMD`
	if eval ssh $DEPLOY_USER@$host $START_STOP_CMD ;then
	    ok_mesg "[$host] $ll"
	else
	    nok_mesg "[$host] $ll"
	fi  
	if [ "$type" == "cu" ];then
	    if ssh $DEPLOY_USER@$host "killall -9 UnitServer"; then
		ok_mesg "[$host] killed UnitServer"
	    else
		nok_mesg "[$host] killed UnitServer"
	    fi
	fi

    # if ssh chaos@$host "test -f /etc/init/chaos-service.conf";then
    # 	if ssh chaos@$host "sudo service chaos-service $op NODE=$type" ;then
    # 	    ok_mesg "[$host] chaos-service $op NODE=$type"
    # 	else
    # 	    nok_mesg "[$host] chaos-service $op NODE=$type"
    # 	fi
    # else
    # 	if ssh chaos@$host "sudo service chaos-$type $op" ;then
    # 	    ok_mesg "[$host] chaos-$type $op"
    # 	else
    # 	    nok_mesg "[$host] chaos-$type $op"
    # 	fi  
	
#    fi
}


net_start_stop(){
    type=$1
    op=$2
    if [ "$type" == "cu" ] || [ "$type" == "agent" ] ;then
	for c in $CU_SERVERS;do
    		start_stop_service "$c" agent $op
	done
    fi

	
	if [ "$type" == "webui" ];then
	for c in $WEBUI_SERVER;do
    		start_stop_service "$c" $type $op
   
	done
    fi

	if [ "$type" == "wan" ];then
	for c in $WAN_SERVER;do
    		start_stop_service "$c" $type $op
   
	done
    fi

	if [ "$type" == "mds" ] || [ "$type" == "cds" ] ;then
	for c in $MDS_SERVER;do
    		start_stop_service "$c" mds $op
	done
	for c in $CDS_SERVERS;do
    		start_stop_service "$c" cds $op
   
	done

    fi


}

if [ -n "$restart" ];then
	if [ "$restart" == "all" ];then
		restart="mds webui wan agent" 
	fi
	k=0
	for i in $restart;do
		info_mesg "stopping all " "$i"
		net_start_stop $i stop
	done
	for i in $restart;do
		info_mesg "starting all " "$i"
		net_start_stop $i start
		sleep 2
	done

	exit 0
	
fi
if [ -n "$stop" ];then
	if [ "$stop" == "all" ];then
		stop="mds webui wan agent" 
	fi
	k=0
	for i in $stop;do
		info_mesg "stopping all " "$i"
		net_start_stop $i stop
	done

	exit 0
	
fi

name=`basename $CHAOS_PREFIX`
info_mesg "generating tarball " "$name.tgz"
## clean log if exists


TMP_DEPLOY=$TMPDIR/$name
mkdir $TMP_DEPLOY
mkdir $TMP_DEPLOY/log
if [ -z "$DEPLOY_DIR" ];then
    DEPLOY_DIR="bin lib etc html tools"
fi


for i in $DEPLOY_DIR;do
    info_mesg "including " "$i"
    
    if [ "$i" != "bin" ];then
	cp -r $CHAOS_PREFIX/$i $TMP_DEPLOY
    fi
done


rm -f $TMP_DEPLOY/etc/*.cfg
cp $cudir/*.cfg $TMP_DEPLOY/etc/
# cp -r $CHAOS_PREFIX/html $TMP_DEPLOY/
cp -r $CHAOS_PREFIX/chaos_env.sh $TMP_DEPLOY/

mkdir $TMP_DEPLOY/bin
if [ -z "$DEPLOY_BINARIES" ];then
    DEPLOY_BINARIES="ChaosWANProxy:wan CUIserver:webui ChaosMetadataService:mds ChaosMetadataService:cds UnitServer:cu ChaosAgent:agent"
fi

for i in $DEPLOY_BINARIES;do

    if [[ "$i" =~ ([a-zA-Z0-9]+):([a-zA-Z0-9]+) ]];then
	bin=${BASH_REMATCH[1]}
	link=${BASH_REMATCH[2]}
	cp $CHAOS_PREFIX/bin/$bin $TMP_DEPLOY/bin
	pushd $TMP_DEPLOY/bin >& /dev/null
	ln -sf $bin $link
	popd >& /dev/null
    else
	cp $CHAOS_PREFIX/bin/$i $TMP_DEPLOY/bin
    fi


done
## find $TMP_DEPLOY -name ".git" -exec rm -rf \{\} \; >& /dev/null


if tar -c -C $TMP_DEPLOY/.. $name | gzip -n > $TMPDIR/$name.tgz;then
    ok_mesg "$name created"
else 
    nok_mesg "$name created"

    exit 1
fi


if [ -n "$CU_SERVERS" ]; then
    if [ ! -f $cudir/cu.cfg ];then
	error_mesg "missing configuration " "$cudir/cu.cfg"
	exit 1
    else
	info_mesg "using configuration " "$cudir/cu.cfg"
    fi


fi

if [ -n "$DEPLOY_REPO" ];then
    scp $TMPDIR/$name.tgz $DEPLOY_REPO
fi

if [ -n "$DEPLOY_SERVERS" ];then
    for i in $DEPLOY_SERVERS;do
	deployServer $i	
    done
fi

servers=`cat $DEPLOY_FILE`



for host in `cat $DEPLOY_FILE`;do

    info_mesg "stopping all services on" " $host"
    start_stop_service $host "cu" stop >& /dev/null
    start_stop_service $host "agent" stop >& /dev/null
    start_stop_service $host "mds" stop >& /dev/null
    start_stop_service $host "cds" stop >& /dev/null
    start_stop_service $host "webui" stop >& /dev/null
    start_stop_service $host "wan" stop >& /dev/null


    info_mesg "removing chaos-* " " $DEPLOY_USER@$host:$DEPLOY_PREFIX_DIR"

    if ssh $DEPLOY_USER@$host "rm -rf $DEPLOY_PREFIX_DIR/chaos-*;echo \"$ver\" > README.install"; then
	ok_mesg "removing chaos-* in $host"
    fi
    
done  
info_mesg "copy on the destination servers: " "$servers"
if $dir/chaos_remote_copy.sh -u $DEPLOY_USER -d $DEPLOY_PREFIX_DIR -s $TMPDIR/$name.tgz $DEPLOY_FILE;then
    ok_mesg "copy done"
else
    nok_mesg "copy done"
    exit 1
fi


md5=`md5sum $TMPDIR/$name.tgz | cut -d ' ' -f 1`

extract(){
    host=$1

    
    if ssh $DEPLOY_USER@$host "test -f $DEPLOY_PREFIX_DIR/$name.tgz"; then
	ver="installed "`date`" MD5:$md5"
	if ssh $DEPLOY_USER@$host "cd $DEPLOY_PREFIX_DIR;tar xfz $name.tgz;ln -sf $name $dest_prefix;echo \"$ver\" > README.install;rm $name.tgz"; then
	    ok_mesg "extracting $name in $host"
	fi
    fi

}
deploy_install(){
    host=$1
    type=$2
    subtype=""
    if [[ "$type" =~ ([a-zA-Z]+)([0-9]+) ]];then
	type=${BASH_REMATCH[1]}
	subtype=${BASH_REMATCH[2]}
	
    fi
    info_mesg "installing $type$subtype in " "$host"
    ## STOP
    start_stop_service $host $type stop
    if [ $type == "cu" ];then
	start_stop_service $host agent stop
    fi


    extract $host


    if ssh $DEPLOY_USER@$host "cd $dest_prefix;ln -sf \$PWD/tools/config/lnf/$cuconfig/$type.cfg etc/";then
	ok_mesg "$type configuration $dest_prefix/tools/config/lnf/$cuconfig/$type.cfg"
    else
	nok_mesg "$type configuration $dest_prefix/tools/config/lnf/$cuconfig/$type.cfg"
    fi    

    if ssh $DEPLOY_USER@$host "cd $dest_prefix;ln -sf \$PWD/tools/config/lnf/$cuconfig/agent.cfg etc/";then
	ok_mesg "agent.cfg configuration $dest_prefix/tools/config/lnf/$cuconfig/agent.cfg"
    else
	nok_mesg "agent configuration $dest_prefix/tools/config/lnf/$cuconfig/agent.cfg"
    fi    
    

    if [ "$type" == "cu" ];then
	if ssh $DEPLOY_USER@$host "cd $dest_prefix/;ln -sf \$PWD/tools/config/lnf/$cuconfig/cu$subtype.sh bin/cu";then
	    ok_mesg "linking $dest_prefix/tools/config/lnf/$cuconfig/cu$subtchype.sh in $dest_prefix/bin/cu"
	else
	    nok_mesg "linking $dest_prefix/tools/config/lnf/$cuconfig/cu$subtype.sh in $dest_prefix/bin/cu"
	    
	fi
    fi

    if [ $type == "cu" ];then
	start_stop_service $host agent start
    else
	start_stop_service $host $type start
    fi

}

if [ $listupdate == "all" ] || [[ $listupdate =~ mds ]];then 
    if [ -n "$MDS_SERVER" ]; then
	deploy_install "$MDS_SERVER" mds
    fi
fi

if [ $listupdate == "all" ] || [[ $listupdate =~ cds ]];then 
    for i in $CDS_SERVERS;do
	deploy_install "$i" cds
    done
fi

if [ $listupdate == "all" ] || [[ $listupdate =~ webui ]];then 
    if [ -n "$WEBUI_SERVER" ]; then
	for i in $WEBUI_SERVER;do
	    deploy_install "$i" webui
	done
    fi
fi

if [ $listupdate == "all" ] || [[ $listupdate =~ wan ]];then 
    if [ -n "$WAN_SERVER" ]; then
	deploy_install "$WAN_SERVER" wan
    fi
fi

if [ -f "$cudir/$MDS_CONFIG" ]; then
    info_mesg "applying MDS CONFIGURATION " "$cudir/$MDS_CONFIG"
    if LD_LIBRARY_PATH=$CHAOS_PREFIX/lib $CHAOS_PREFIX/bin/ChaosMDSCmd --mds-conf $cudir/$MDS_CONFIG --metadata-server $MDS_SERVER:5000 -r 1 >  /dev/null;then
	ok_mesg "configuration applied"
    else
	nok_mesg "configuration applied"
    fi
    
fi


k=1
if [ $listupdate == "all" ] || [[ $listupdate =~ cu ]];then 
    for i in $CU_SERVERS;do
	deploy_install "$i" cu$k
	((k++))
    done
fi

for i in $DEPLOY_SERVERS;do
    extract "$i"
done


