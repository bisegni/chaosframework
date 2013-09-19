#!/bin/sh

BASEDIR=$(dirname $0)


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