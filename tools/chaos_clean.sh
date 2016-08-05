#!/bin/bash
dir=`dirname $0`
if [ -n "$1" ];then
    chaos_bundle=$1
else
    if [ -n "$CHAOS_BUNDLE" ];then
	chaos_bundle="$CHAOS_BUNDLE"
    else
	chaos_bundle=$dir/..
    fi
fi
function cleandir(){
    echo "* cleaning $1"
    rm -rf $1
}
sys=`uname -s`
mac=`uname -m`
cleandir $chaos_bundle/usr

if [ -n "$CHAOS_TARGET" ];then
    cleandir $chaos_bundle/chaosframwork/external-${CHAOS_TARGET}
    cleandir $chaos_bundle/build-${CHAOS_TARGET}
else
    cleandir $chaos_bundle/chaosframwork/external-$sys-$mac
        cleandir $chaos_bundle/build-$sys-$mac
fi
cleandir $chaos_bundle/chaosframwork/usr
cleandir $chaos_bundle/chaosframework/bin
cleandir $chaos_bundle/chaosframework/build
cleandir $chaos_bundle/build
if [ -n "$CHAOS_PREFIX" ]; then
    echo "* cleaning CHAOS_PREFIX=$CHAOS_PREFIX" 
    cleandir $CHAOS_PREFIX
fi
find $chaos_bundle -name "CMakeFiles" -exec rm -rf \{\} \; >& /dev/null
find $chaos_bundle -name "CMakeCache.txt" -exec rm -rf \{\} \; >& /dev/null
