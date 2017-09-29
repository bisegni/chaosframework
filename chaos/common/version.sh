#!/bin/sh

BASEDIR=$(dirname $0)
echo "#define CSLIB_VERSION_MAJOR \"1\"" > $BASEDIR/curr_version.h
echo "#define CSLIB_VERSION_MINOR \"1\"" >> $BASEDIR/curr_version.h
git log -n 1 --pretty="format:#define CSLIB_VERSION_HEADER \"!CHAOS Library Developed By Claudio Bisegni\"%n#define CSLIB_VERSION_NUMBER \"%h\"%n" >> $BASEDIR/curr_version.h

if diff $BASEDIR/curr_version.h $BASEDIR/version.h> /dev/null 2>&1; then
    echo "* no version changes detected";
    exit 0;
else
    cp $BASEDIR/curr_version.h $BASEDIR/version.h;
fi

#check if we are using the script into xcode or no (in xcode the ssty give an error)
if [ ! -n $XCODE_PRODUCT_BUILD_VERSION ]; then
    git log -n 1 --pretty="format:#define CSLIB_VERSION_HEADER \"!CHAOS Library Developed By Claudio Bisegni\"%n#define CSLIB_VERSION_NUMBER \"Version:%h\"%n" > /tmp/version.h
    stty -echo

    if ! diff /tmp/version.h $BASEDIR/version.h>/dev/null; then
	stty echo
	echo "Writing version header file" $BASEDIR
	cp /tmp/version.h $BASEDIR/version.h
    fi
    rm /tmp/version.h
    stty echo
else
    git log -n 1 --pretty="format:#define CSLIB_VERSION_HEADER \"!CHAOS Library Developed By Claudio Bisegni\"%n#define CSLIB_VERSION_NUMBER \"Version:%h\"%n" > $BASEDIR/version.h
fi
