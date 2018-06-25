#!/bin/bash
#
# axel@cern.ch, 2014-02-07

# which is not ideal, see http://stackoverflow.com/a/677212/1392758
python=`which python`
if type python2 > /dev/null 2>&1; then
    python=`which python2`
fi

source=$1
cores=`$python <(cat <<EOF
import multiprocessing
print (multiprocessing.cpu_count())
EOF
)`
echo Using $cores cores.
echo using $source as source direcotries

function update {
    cd $source || exit 1
    echo '++ Updating llvm...'
    git pull || exit 1
    cd tools/clang || exit 1
    echo '++ Updating clang...'
    git pull || exit 1
    echo '++ Updating cling...'
    cd ../cling || exit 1
    git pull || exit 1
    echo '++ Update done.'
    cd ../../..
}

function clone {
    # clone what branch where
    where=$3
    if [ "$where" = "" ]; then
        where=$1
    fi
    echo '>> Cloning '$1'...'
    git clone http://root.cern.ch/git/${1}.git $where > /dev/null || exit 1
    ( cd $where && git checkout $2 )
}

function initial {
    clone llvm cling-patches $source
    cd $source/tools || exit 1
    clone clang cling-patches
    clone cling master
    cd ../..
}

function configure {
    mkdir -p obj || exit 1
    INSTDIR=`pwd`/inst
    cd obj || exit 1
    echo '>> Configuring...'
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DPYTHON_EXECUTABLE=$python ../$source > /dev/null || exit 1
    cd ..
}

function build {
    cd obj
    echo ':: Building...'
    make -j$cores || exit 1
    rm -rf ../inst
    echo ':: Installing...'
    make -j$cores install || exit 1
    echo ':: SUCCESS.'
    cd ..
}

if [ -d "$source/.config" ]; then
    # update mode
    update
else
    initial
fi
