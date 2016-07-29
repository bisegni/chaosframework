#!/bin/bash
dir=`dirname $0`
if [ -n "$CHAOS_BUNDLE" ];then
    chaos_bundle="$CHAOS_BUNDLE"
else
    chaos_bundle=$dir/..
fi

function cleandir(){
    echo "* cleaning $1"
    rm -rf $1
}

cleandir $chaos_bundle/external
cleandir $chaos_bundle/chaosframwork/usr
cleandir $chaos_bundle/chaosframework/bin
cleandir $chaos_bundle/chaosframework/build
if [ -n "$CHAOS_PREFIX" ]; then
    cleandir $CHAOS_PREFIX
fi
find $chaos_bundle -name "CMakeFiles" -exec rm -rf \{\} \; >& /dev/null
find $chaos_bundle -name "CMakeCache.txt" -exec rm -rf \{\} \; >& /dev/null
