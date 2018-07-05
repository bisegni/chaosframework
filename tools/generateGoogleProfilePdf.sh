#!/bin/bash
echo "* looking .heap in $1 using prefix $2"
heapls=`find $1 -name "*.heap"`
for heapf in $heapls;do
    echo "* processing $heapf"
    if [[ $heapf =~ \/([[:alpha:]]+)\.(.+)\.(.+)\.heap$ ]];then
	pname=${BASH_REMATCH[1]}
	ppid=${BASH_REMATCH[2]}
#	echo "program name:$pname"
#	echo "program pid:$ppid"
	if [ ! -x $2/$pname ];then
	    echo "## cannot find executable $2/$pname"
	    exit 1
	fi
	if google-pprof $2/$pname "$heapf" --inuse_objects --lines --heapcheck  --edgefraction=1e-10 --nodefraction=1e-10 --pdf > $pname.$ppid.pdf;then
	    echo "* generated $pname.$ppid.pdf"
	else
	    echo "## error processing $heapf"
	fi
    fi
done
