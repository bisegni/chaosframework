#!/bin/bash
#script for initialize the bundle create with google repo utility

#boostrap !CHAOS Framework in development mode
export CHAOS_DEVELOPMENT="YES"

chaosframework/bootstrap.sh
ln -s chaosframework/usr usr

#make the documentation
cd chaosframework
doxygen Documentation/chaosdocs
cd ..
ln -s chaosframework/Documentation/html Documentation
