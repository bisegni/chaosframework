#!/bin/bash -e

separator='-'
pushd `dirname $0` > /dev/null
dir=`pwd -P`
popd > /dev/null

source $dir/common_util.sh
TMPDIR="/tmp/$USER/chaos_deploy"
dest_prefix=chaos-distrib
DEPLOY_FILE=$TMPDIR/deployTargets

Usage(){
    echo "$0 <deploy configuration>"
}

if [ ! -e "$1" ]; then
    nok_mesg "You must specify a valid file for deployment"
    exit 1
fi
if [ -z "$CHAOS_PREFIX" ];then
    echo "## NO environment CHAOS_PREFIX defined"
    exit 1
fi
source $1

rm -rf $TMPDIR
mkdir -p $TMPDIR
cudir=`dirname $1`
cuconfig=`basename $cudir`


deployServer(){
    serv=$1
    if ! grep "$serv" $DEPLOY_FILE;then
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
    deployServer $MDS_SERVER

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

    pushd $CHAOS_PREFIX > /dev/null
    cp -r $CHAOS_PREFIX/html $CHAOS_PREFIX/www-$webui
    find $CHAOS_PREFIX/www-$webui -name "*" -exec  sed -i s/__template__webuiulr__/$webui/g \{\} >& /dev/null \; 

    popd > /dev/null
    deployServer $WAN_SERVER

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


    deployServer $WAN_SERVER

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

    for i in $CDS_SERVERS;do
	deployServer $i	
    done

fi



for i in $CU_SERVERS;do
    deployServer $i	
done


name=`basename $CHAOS_PREFIX`
info_mesg "generating tarball " "$name.tgz"
if tar -c -C $CHAOS_PREFIX/.. $name | gzip -n > $TMPDIR/$name.tgz;then
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

if [ -n "$DEPLOY_SERVERS" ];then
    for i in $DEPLOY_SERVERS;do
	deployServer $i	
    done
fi

servers=`cat $DEPLOY_FILE`
info_mesg "copy on the destination servers: " "$servers"


for host in `cat $DEPLOY_FILE`;do
    info_mesg "removing chaos-* " "$host"
    if ssh chaos@$host "rm -rf chaos-*;rm $name.tgz;echo \"$ver\" > README.install"; then
	ok_mesg "removing chaos-* in $host"
    else
	nok_mesg "removing chaos-* in $host"
    fi

done  

if $dir/chaos_remote_copy.sh -u chaos -s $TMPDIR/$name.tgz $DEPLOY_FILE;then
    ok_mesg "copy done"
else
    nok_mesg "copy done"
    exit 1
fi


md5=`md5sum $TMPDIR/$name.tgz | cut -d ' ' -f 1`

start_stop_service(){
    host=$1
    type=$2
    op=$3
## new 
	if ssh chaos@$host "sudo service chaos-$type $op" ;then
	    ok_mesg "[$host] chaos-$type $op"
	else
	    nok_mesg "[$host] chaos-$type $op"
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

extract(){
    host=$1

    if ssh chaos@$host "test -f $name.tgz"; then
	ver="installed "`date`" MD5:$md5"
	if ssh chaos@$host "tar xfz $name.tgz;ln -sf $name $dest_prefix;rm $name.tgz;echo \"$ver\" > README.install"; then
	    ok_mesg "extracting $name in $host"
	else
	    nok_mesg "extracting $name in $host"
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

    extract $host
    if ssh chaos@$host "cd $dest_prefix;ln -sf \$PWD/tools/config/lnf/$cuconfig/$type.cfg etc/";then
	ok_mesg "$type configuration $dest_prefix/tools/config/lnf/$cuconfig/$type.cfg"
    else
	ok_mesg "$type configuration $dest_prefix/tools/config/lnf/$cuconfig/$type.cfg"
    fi    
    

    if [ "$type" == "cu" ];then
	if ssh chaos@$host "cd $dest_prefix/;ln -sf \$PWD/tools/config/lnf/$cuconfig/cu$subtype.sh bin/cu";then
	    ok_mesg "linking $dest_prefix/tools/config/lnf/$cuconfig/cu$subtchype.sh in $dest_prefix/bin/cu"
	else
	    nok_mesg "linking $dest_prefix/tools/config/lnf/$cuconfig/cu$subtype.sh in $dest_prefix/bin/cu"
	    
	fi
    fi

    
    start_stop_service $host $type start


}

if [ -n "$MDS_SERVER" ]; then
    deploy_install "$MDS_SERVER" mds
fi

for i in $CDS_SERVERS;do
    deploy_install "$i" cds
done

if [ -n "$WEBUI_SERVER" ]; then
    deploy_install "$WEBUI_SERVER" webui
fi

if [ -n "$WAN_SERVER" ]; then
    deploy_install "$WAN_SERVER" wan
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

for i in $CU_SERVERS;do
    deploy_install "$i" cu$k
    ((k++))
done

for i in $DEPLOY_SERVERS;do
    extract "$i"
done





