#!/bin/sh

BASEDIR=$(dirname $0)
echo "Writing version header file" $BASEDIR

export PATH=$PATH:/opt/local/bin:/usr/local/bin

git log -n 1 --pretty="format:#define CSLIB_VERSION_HEADER \"!CHAOS Library Developed By Claudio Bisegni\"%n#define CSLIB_VERSION_NUMBER \"Version:%h\"%n#define CSLIB_VERSION_LAST_COMMITTER \"committed by %an\"%n" > $BASEDIR/version.h
