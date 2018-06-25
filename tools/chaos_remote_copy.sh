#!/bin/bash


user="root"
source=""
dest=""


usage(){

    echo "Usage is $0  [-u <ssh user ($user)> ] -s <local source> [-d <remote destination>] <server lists file >"
    
}

dest=""
while getopts u:d:s:h opt;do 
    case $opt in
	u) 
	    user=$OPTARG
	    ;;
	d)
	    dest=$OPTARG
	    ;;
	h)
	    usage
	    exit 0
	    ;;
	s)
	    source=$OPTARG
	    ;;
    esac
    
done


shift $((OPTIND -1))

if [ ! -e "$1" ];then
    echo "## you must specify a valid list of servers file"
    exit 1
fi

if [ ! -e "$source" ];then
    echo "## you must specify an existing binary, \"$source\" doesn't exists"
    exit 1
fi

 
list=`cat $1`
md5=`md5sum $source | cut -d ' ' -f 1`
base_source=`basename $source`
echo "* md5: $md5"
for s in $list;do
     if [ -n "$dest" ];then
     	rmd5=`ssh $user@$s "md5sum $dest/$base_source|cut -d ' ' -f 1"`
     else
     	 rmd5=`ssh $user@$s "md5sum $base_source|cut -d ' ' -f 1"`

    fi
    if ! [ "$rmd5" == "$md5" ];then
	scp $source $user@$s:$dest >& /dev/null &
	echo "* copying $source in $user@$s:$dest id $! ..."
    else
	echo "* skipping copy an existing copy esists with the same MD5 $rmd5"
    fi

done
error=0
for job in `jobs -p`;do
    echo "* waiting finishing id $job"
    wait $job || let "error+=1"
    if [ $error != "0" ] ;then
	echo "## error copying (job $job) $error"
	
    fi
done

if [ $error == "0" ] ;then
    echo "* successfully copied"
else
    exit 1
fi

