#!/bin/bash
realpath=`readlink -f $0`
dir=`dirname $realpath`
os=`uname -s`
if [ "$os" == "Darwin" ];then 
    DYLD_LIBRARY_PATH=$dir/../lib $dir/ccs
else
    LD_LIBRARY_PATH=$dir/../lib $dir/ccs
fi

