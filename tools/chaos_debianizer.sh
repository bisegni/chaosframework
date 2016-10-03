#!/bin/bash
## must package name, source dir,  version 
pushd `dirname $0` >& /dev/null
dir=`pwd -P`
popd >& /dev/null

export CHAOS_TOOL=$dir

source $dir/common_util.sh

APT_DEST="chaosweb@opensource.lnf.infn.it:/var/www/html/apt/dists/unstable/main/binary-"

SOURCE_DIR=""


copy(){
    if ! cp -af $1 $2 >&/dev/null;then
	error_mesg "cannot copy from $1 to $2"
	exit 1
    else
	info_mesg "transferring " "$1"
    fi

}
VERSION="0.1"
PACKAGE_NAME="chaos"

CLIENT="true"

Usage(){
    echo -e "Usage is $0 -i <source dir> [-p <package name > ] [-v <version> ] [-c] [-r]\n-i <source dir>: a valid source chaos distribution [$SOURCE_DIR]\n-p <package name>: is the package name prefix [$PACKAGE_NAME]\n-v <version>:a version of the package distribution [$VERSION]\n-r:copy to remote apt server\n-t <all,docker,localhost,server,client>"
    exit 1
}
DEPENDS="bash (>= 3), bc"
TEMPLATE="localhost"

while getopts p:i:v:t:,r opt; do
    case $opt in
	p) PACKAGE_NAME=$OPTARG
	    ;;
	i) SOURCE_DIR=$OPTARG
	    ;;
	v) VERSION=$OPTARG
	    ;;
	t) TEMPLATE=$OPTARG;
	   ;;

	r) COPY_REMOTE="true"
	   ;;

#        t) ARCH=$OPTARG
#	    ;;
	*)
	    Usage
    esac
	
done

COPY_SRC_FILES=""

case $TEMPLATE in
    docker)
	desc="docker package, similar to localhost but different backend address"
	DEPENDS="$DEPENDS,apache2(>=2.4)"
	;;
    client)
	desc="Client !CHAOS package include libraries and binaries"
	COPY_SRC_FILES="$SOURCE_DIR/bin/UnitServer"
	;;
    server)
	desc="Server !CHAOS package include libraries and binaries"
	DEPENDS="$DEPENDS, apache2(>=2.4)"
	;;
    dev)
	desc="Development package include libraries, includes and tools"
	DEPENDS="$DEPENDS, gcc(>=4.8), g++(>=4.8), cmake(>=2.6), apache2(>=2.4)"
	;;
    *)
	TEMPLATE="localhost"
	desc="localhost configuration"
	DEPENDS="$DEPENDS,apache2(>=2.4),mongodb-org,couchbase-server"
	;;
esac

if [ -z "$SOURCE_DIR" ] && [ -n "$CHAOS_PREFIX" ];then
    SOURCE_DIR=$CHAOS_PREFIX
fi
    
if [  ! -f "$SOURCE_DIR/chaos_env.sh" ];then
    error_mesg "please specify a valid CHAOS INSTALL DIRECTORY missing " "$SOURCE_DIR/chaos_env.sh"
    exit 1
fi

SOURCE_DIR=$(get_abs_dir $SOURCE_DIR)


if [ ! -d "$SOURCE_DIR" ];then
    error_mesg "missing a valid source directory"
    Usage
fi
source $SOURCE_DIR/chaos_env.sh

if [ -z "$CHAOS_TARGET" ];then
    os=`uname -s`
    arch=`uname -m`
    CHAOS_TARGET=$os-$arch
fi

if [ "$CHAOS_TARGET" != "Linux-i386" ] && [ "$CHAOS_TARGET" != "Linux-x86_64" ] && [ "$CHAOS_TARGET" != "Linux-armhf" ];then
    
    error_mesg "bad target " "$CHAOS_TARGET"
    Usage;
fi

if [[ "$CHAOS_TARGET" =~ (.+)-(.+) ]] ;then
    ARCH=`echo ${BASH_REMATCH[2]} | sed 's/_/-/g'`
fi

if [ "$ARCH" == "x86-64" ]; then
    ARCH="amd64"
fi

if [ -z "$CHAOS_STATIC" ];then
    DYNAMIC="true"
fi
    


NAME=$PACKAGE_NAME-$TEMPLATE-$VERSION\_$ARCH
PACKAGE_DIR="/tmp/$USER/$NAME"

PACKAGE_INSTALL_PREFIX="/usr/local/chaos"
PACKAGE_INSTALL_DIR="$PACKAGE_INSTALL_PREFIX/$NAME"
PACKAGE_INSTALL_ALIAS_DIR="$PACKAGE_INSTALL_PREFIX/chaos-distrib"

PACKAGE_DEST="$PACKAGE_DIR/$PACKAGE_INSTALL_DIR"
DEBIAN_DIR="$PACKAGE_DIR/DEBIAN"
rm -rf $PACKAGE_DIR >& /dev/null
info_mesg "using template " "$TEMPLATE ($desc)"
info_mesg "taking distribution $SOURCE_DIR, building " "$NAME"
if !(mkdir -p "$DEBIAN_DIR"); then
    echo "## cannot create $DEBIAN_DIR"
    exit 1
fi;

if !(mkdir -p "$PACKAGE_DEST"); then
    echo "## cannot create $PACKAGE_DEST"
    exit 1
fi;

###
copy $SOURCE_DIR/bin $PACKAGE_DEST

copy $SOURCE_DIR/html $PACKAGE_DEST
copy $SOURCE_DIR/bin $PACKAGE_DEST
copy $SOURCE_DIR/etc $PACKAGE_DEST
copy $SOURCE_DIR/tools $PACKAGE_DEST
copy $SOURCE_DIR/chaos_env.sh $PACKAGE_DEST
copy $SOURCE_DIR/tools/package_template/etc $PACKAGE_DIR/etc
rm -f $PACKAGE_DEST/etc/*.cfg >& /dev/null



if [ "$TEMPLATE" == "dev" ];then
    copy $SOURCE_DIR/include $PACKAGE_DEST
    copy $SOURCE_DIR/lib $PACKAGE_DEST
else
    if [ -n "$DYNAMIC" ]; then
	copy $SOURCE_DIR/lib $PACKAGE_DEST
    fi    
fi

if [ "$TEMPLATE" == "docker" ];then
    pushd $PACKAGE_DEST/etc >& /dev/null
    ln -sf ../tools/config/lnf/docker/cds.cfg cds.cfg
    ln -sf ../tools/config/lnf/docker/mds.cfg mds.cfg
    ln -sf ../tools/config/lnf/docker/webui.cfg webui.cfg
    ln -sf ../tools/config/lnf/docker/wan.cfg wan.cfg
    popd >& /dev/null
fi

if [ "$TEMPLATE" == "dev" ];then
    pushd $PACKAGE_DEST/etc >& /dev/null
    ln -sf ../tools/config/lnf/development/cds.cfg cds.cfg
    ln -sf ../tools/config/lnf/development/mds.cfg mds.cfg
    ln -sf ../tools/config/lnf/development/webui.cfg webui.cfg
    ln -sf ../tools/config/lnf/development/wan.cfg wan.cfg
    popd >& /dev/null
fi

if [ "$TEMPLATE" == "localhost" ];then
    pushd $PACKAGE_DEST/etc >& /dev/null
    ln -sf ../tools/config/lnf/localhost/cds.cfg cds.cfg
    ln -sf ../tools/config/lnf/localhost/mds.cfg mds.cfg
    ln -sf ../tools/config/lnf/localhost/webui.cfg webui.cfg
    ln -sf ../tools/config/lnf/localhost/wan.cfg wan.cfg
    popd >& /dev/null
fi


PACKAGE_NAME=$PACKAGE_NAME-$TEMPLATE
TS=`date +%s`
echo "Package: $PACKAGE_NAME" > $DEBIAN_DIR/control
echo "Filename: $NAME.deb" >> $DEBIAN_DIR/control
echo "Version: $TS:$VERSION" >> $DEBIAN_DIR/control
# echo "Date: " `date -R ` >> $DEBIAN_DIR/control
echo "Section: base" >> $DEBIAN_DIR/control
echo "Priority: optional" >> $DEBIAN_DIR/control
echo "Architecture: $ARCH" >> $DEBIAN_DIR/control
echo "Depends: $DEPENDS" >> $DEBIAN_DIR/control
echo "Maintainer: Claudio Bisegni claudio.bisegni@lnf.infn.it, Andrea Michelotti andrea.michelotti@lnf.infn.it" >> $DEBIAN_DIR/control
echo "Description: $desc" >> $DEBIAN_DIR/control
APT_DEST="$APT_DEST$ARCH"

pushd $PACKAGE_DIR >& /dev/null
echo "#!/bin/bash" > DEBIAN/postrm
echo "#!/bin/bash" > DEBIAN/prerm
echo "#!/bin/bash" > DEBIAN/postinst
echo "#!/bin/bash" > DEBIAN/config
echo "TEMPLATE=$TEMPLATE" >> DEBIAN/postrm
echo "TEMPLATE=$TEMPLATE" >> DEBIAN/postinst
echo "TEMPLATE=$TEMPLATE" >> DEBIAN/prerm
echo "TEMPLATE=$TEMPLATE" >> DEBIAN/config
echo ". /usr/share/debconf/confmodule" >> DEBIAN/config    
echo "PACKAGE_NAME=$PACKAGE_NAME" >> DEBIAN/config
echo "PACKAGE_NAME=$PACKAGE_NAME" >> DEBIAN/postrm
echo "PACKAGE_NAME=$PACKAGE_NAME" >> DEBIAN/prerm
echo "PACKAGE_NAME=$PACKAGE_NAME" >> DEBIAN/postinst

# echo "set -e" >> DEBIAN/postinst
echo "INSTDIR=$PACKAGE_INSTALL_DIR" >> DEBIAN/postrm
echo "INSTDIR_ALIAS=$PACKAGE_INSTALL_ALIAS_DIR" >> DEBIAN/postrm

echo "ln -sf $PACKAGE_INSTALL_DIR $PACKAGE_INSTALL_ALIAS_DIR" >> DEBIAN/postinst
echo "INSTDIR=$PACKAGE_INSTALL_ALIAS_DIR" >> DEBIAN/postinst

if [ "$TEMPLATE"=="server" ] || [ "$TEMPLATE" == "dev" ];then
    echo "db_input high \$PACKAGE_NAME/ask_mds || true" >> DEBIAN/config
    echo "db_input high \$PACKAGE_NAME/ask_cds || true" >> DEBIAN/config
    echo "db_input high \$PACKAGE_NAME/ask_webui || true" >> DEBIAN/config
    echo "db_input high \$PACKAGE_NAME/ask_us || true" >> DEBIAN/config
fi
if [ "$TEMPLATE"=="localhost" ] || [ "$TEMPLATE" == "docker" ];then
    echo "db_set  \$PACKAGE_NAME/ask_mds true" >> DEBIAN/config
    echo "db_set  \$PACKAGE_NAME/ask_cds true" >> DEBIAN/config

       echo "db_set  \$PACKAGE_NAME/ask_webui true" >> DEBIAN/config
    echo "db_set  \$PACKAGE_NAME/ask_wan true" >> DEBIAN/config
    if [ "$TEMPLATE"=="localhost" ];then
	echo "db_set  \$PACKAGE_NAME/ask_us true" >> DEBIAN/config
    else
	echo "db_set  \$PACKAGE_NAME/ask_us false" >> DEBIAN/config
    fi
    echo "db_set \$PACKAGE_NAME/ask_mongo \"localhost\"" >> DEBIAN/config 
    echo "db_set \$PACKAGE_NAME/ask_dbuser \"chaos\" " >> DEBIAN/config
    echo "db_set \$PACKAGE_NAME/ask_dbpass \"chaos\" " >> DEBIAN/config
    echo "db_set \$PACKAGE_NAME/ask_couchbase \"localhost\"" >> DEBIAN/config
    echo "db_set \$PACKAGE_NAME/ask_mdsurl \"localhost:5000\"" >> DEBIAN/config

fi

if [ "$TEMPLATE" != "docker" ];then
    echo "db_input high \$PACKAGE_NAME/ask_us || true" >> DEBIAN/config
    echo "db_go || true" >> DEBIAN/config
fi

    
cat "$SOURCE_DIR/tools/package_template/DEBIAN/config" >> DEBIAN/config
chmod 0555 DEBIAN/config
cp $SOURCE_DIR/tools/package_template/DEBIAN/templates DEBIAN/
echo "ln -sf $PACKAGE_INSTALL_DIR/bin/UnitServer $PACKAGE_INSTALL_DIR/bin/cu" >> DEBIAN/postinst


cp "$SOURCE_DIR/tools/package_template/DEBIAN/preinst" DEBIAN/
cat "$SOURCE_DIR/tools/package_template/DEBIAN/prerm" >> DEBIAN/prerm
cat "$SOURCE_DIR/tools/package_template/DEBIAN/postrm" >> DEBIAN/postrm
cat "$SOURCE_DIR/tools/package_template/DEBIAN/postinst" >> DEBIAN/postinst
echo "ln -sf $PACKAGE_INSTALL_DIR/bin/ccs.sh /usr/bin/ccs" >> DEBIAN/postinst
echo "rm -f /usr/bin/ccs" >> DEBIAN/postrm

cmd="sed -i 's/chaos\//$PACKAGE_NAME\//g' DEBIAN/templates"

eval $cmd 


# listabin=`ls $PACKAGE_DEST/bin`
#for i in $listabin;do
#    name=`basename $i`
#    echo "ln -sf $PACKAGE_INSTALL_DIR/bin/$name /usr/bin" >> DEBIAN/postinst

#    echo "rm -rf /usr/bin/$name" >> DEBIAN/postrm 
#done


# if [ -n "$DYNAMIC" ]; then
#     echo "rm -f /etc/ld.so.conf.d/chaos_distrib.conf" >> DEBIAN/postrm
#     echo "ldconfig" >> DEBIAN/postinst
# fi
echo "echo \"* install done \"" >> DEBIAN/postinst
echo "echo \"* remove done \"" >> DEBIAN/postrm
echo "exit 0" >> DEBIAN/postrm
chmod 0555 DEBIAN/postrm
chmod 0555 DEBIAN/preinst
chmod 0555 DEBIAN/prerm
chmod 0555 DEBIAN/postinst

popd >& /dev/null
info_mesg "packaging $NAME for architecture " "$ARCH .."

dpkg-deb -b $PACKAGE_DIR >& /dev/null
mv $PACKAGE_DIR.deb .
info_mesg "created in $PWD" " $NAME.deb"
if [ -n "$COPY_REMOTE" ];then
    info_mesg "copying to APT server " "$APT_DEST"
    scp $NAME.deb $APT_DEST
fi
#rm -rf $PACKAGE_DIR
