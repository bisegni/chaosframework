#!/bin/sh

BASEDIR=$(dirname $0)


export PATH=$PATH:/opt/local/bin:/usr/local/bin

git log -n 1 --pretty="format:#define CSLIB_VERSION_HEADER \"!CHAOS Library Developed By Claudio Bisegni\"%n#define CSLIB_VERSION_NUMBER \"Version:%h\"%n#define CSLIB_VERSION_LAST_COMMITTER \"committed by %an\"%n" > /tmp/version.h 
if ! diff /tmp/version.h $BASEDIR/version.h; then
echo "Writing version header file" $BASEDIR
cp /tmp/version.h $BASEDIR/version.h
fi
rm /tmp/version.h
