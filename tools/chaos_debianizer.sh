#!/bin/bash
## must package name, source dir,  version 
pushd `dirname $0` > /dev/null
dir=`pwd -P`
popd > /dev/null

export CHAOS_TOOL=$dir

source $dir/common_util.sh

APT_DEST="chaosweb@opensource.lnf.infn.it:/var/www/html/apt/dists/unstable/main/binary-"

SOURCE_DIR=""


copy(){
    if ! cp -af $1 $2 >&/dev/null;then
	error_mesg "cannot copy from $1 to $2"
	exit 1
    fi

}
VERSION="0.1"
PACKAGE_NAME="chaos"

CLIENT="true"

Usage(){
    echo -e "Usage is $0 -i <source dir> [-p <package name > ] [-v <version> ] [-c] [-s] [-d] [-a] [-l]\n-i <source dir>: a valid source chaos distribution [$SOURCE_DIR]\n-p <package name>: is the package name prefix [$PACKAGE_NAME]\n-v <version>:a version of the package distribution [$VERSION]\n-c: client distribution (i.e US) [$CLIENT]\n-s: server distribution (CDS,MDS..)\n-a: development with all (client, server and includes)\n-r:copy to remote apt server\n-d: docker distribution\n-l:local host configuration"
    exit 1
}
DEPENDS="bash (>= 3), bc"
while getopts lp:i:v:dt:sac,d,r opt; do
    case $opt in
	p) PACKAGE_NAME=$OPTARG
	    ;;
	i) SOURCE_DIR=$OPTARG
	    ;;
	v) VERSION=$OPTARG
	    ;;
	l) LOCALHOST="true"
	    desc="localhost configuration"
	    DEPENDS="$DEPENDS,mongodb"
	    ;;
	c) CLIENT="true"
	    SERVER=""
	    EXT="client"
	    desc="Client !CHAOS package include libraries and binaries"
	    ;;
	s) SERVER="true"
	    CLIENT=""
	    EXT="server"
	    desc="Server !CHAOS package include libraries and binaries"
	    
	    ;;
	a) ALL="true"
	    EXT="devel"
	    desc="Development package include libraries, includes and tools"
	    ;;
	r) COPY_REMOTE="true"
	    ;;
        d) DOCKER="true"
	    EXT="docker"
	    desc="package include libraries, binaries, includes and tools"
	    CLIENT=""
	    ;;
#        t) ARCH=$OPTARG
#	    ;;
	*)
	    Usage
    esac
	
done
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

if [ -n "$CHAOS_STATIC" ];then
    EXT="static-$EXT"
else
    DYNAMIC="true"
fi
    


NAME=$PACKAGE_NAME-$EXT-$VERSION\_$ARCH
PACKAGE_DIR="/tmp/$USER/$NAME"

PACKAGE_INSTALL_PREFIX="/usr/local/chaos"
PACKAGE_INSTALL_DIR="$PACKAGE_INSTALL_PREFIX/$NAME"
PACKAGE_INSTALL_ALIAS_DIR="$PACKAGE_INSTALL_PREFIX/chaos-distrib"

PACKAGE_DEST="$PACKAGE_DIR/$PACKAGE_INSTALL_DIR"
DEBIAN_DIR="$PACKAGE_DIR/DEBIAN"
rm -rf $PACKAGE_DIR >& /dev/null
info_mesg "taking distribution $SOURCE_DIR, building " "$NAME"
if !(mkdir -p "$DEBIAN_DIR"); then
    echo "## cannot create $DEBIAN_DIR"
    exit 1
fi;


if [ -n "$CLIENT" ];then
   if mkdir -p $PACKAGE_DEST/bin;then
       copy $SOURCE_DIR/bin/UnitServer $PACKAGE_DEST/bin

   else
       error_mesg "cannot create directory " "$PACKAGE_DEST/bin"
       exit 1
   fi
fi


if [ -n "$SERVER" ] || [ -n "$DOCKER" ];then
   if mkdir -p $PACKAGE_DEST/bin;then
       copy $SOURCE_DIR/html $PACKAGE_DEST
       copy $SOURCE_DIR/bin $PACKAGE_DEST
       copy $SOURCE_DIR/etc $PACKAGE_DEST
       DEPENDS="$DEPENDS, apache2(>=2.4), libapache2-mod-php5"
   else
       error_mesg "cannot create directory " "$PACKAGE_DEST/bin"
       exit 1
   fi
fi

if [ -n "$DOCKER" ];then
    pushd $PACKAGE_DEST/etc
    ln -sf ../tools/docker/cds.cfg cds.cfg
    ln -sf ../tools/docker/mds.cfg mds.cfg
    ln -sf ../tools/docker/webui.cfg webui.cfg
    ln -sf ../tools/docker/wan.cfg wan.cfg
fi

if [ -n "$ALL" ];then
    copy $SOURCE_DIR $PACKAGE_DEST
    rm -rf $PACKAGE_DEST/etc
    DEPENDS="$DEPENDS, gcc(>=4.8), g++(>=4.8), cmake(>=2.6), apache2(>=2.4), libapache2-mod-php5"
    SERVER="true"
fi


copy $SOURCE_DIR/tools $PACKAGE_DEST
copy $SOURCE_DIR/chaos_env.sh $PACKAGE_DEST
copy $SOURCE_DIR/tools/package_template/etc $PACKAGE_DIR/etc

 if [ -n "$DYNAMIC" ]; then
     copy $SOURCE_DIR/lib $PACKAGE_DEST/
#     mkdir -p $PACKAGE_DIR/etc/ld.so.conf.d/

#     echo "$PACKAGE_INSTALL_ALIAS_DIR/lib" > $PACKAGE_DIR/etc/ld.so.conf.d/chaos-distrib.conf
 fi

PACKAGE_NAME=$PACKAGE_NAME-$EXT
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

cd $PACKAGE_DIR
echo "#!/bin/bash" > DEBIAN/postrm
echo "#!/bin/bash" > DEBIAN/prerm

echo "#!/bin/bash" > DEBIAN/postinst
# echo "set -e" >> DEBIAN/postinst
echo "INSTDIR=$PACKAGE_INSTALL_DIR" >> DEBIAN/postrm
echo "INSTDIR_ALIAS=$PACKAGE_INSTALL_ALIAS_DIR" >> DEBIAN/postrm
echo "PACKAGE_NAME=$PACKAGE_NAME" >> DEBIAN/postrm
echo "PACKAGE_NAME=$PACKAGE_NAME" >> DEBIAN/prerm
echo "PACKAGE_NAME=$PACKAGE_NAME" >> DEBIAN/postinst
echo "ln -sf $PACKAGE_INSTALL_DIR $PACKAGE_INSTALL_ALIAS_DIR" >> DEBIAN/postinst
echo "INSTDIR=$PACKAGE_INSTALL_ALIAS_DIR" >> DEBIAN/postinst

if [ -n "$LOCALHOST" ] && [ -n "$SERVER" ];then
    echo "LOCALHOST=$SERVER" >> DEBIAN/config
else

    if [ -z "$DOCKER" ];then
	echo "#!/bin/bash" > DEBIAN/config
	#echo "set -e" >> DEBIAN/config
    echo ". /usr/share/debconf/confmodule" >> DEBIAN/config
    
    echo "PACKAGE_NAME=$PACKAGE_NAME" >> DEBIAN/config
    echo "echo \"* configuring \$PACKAGE_NAME\"" >> DEBIAN/config
    if [ "$SERVER" == "true" ] || [ "$ALL" == "true" ]; then
	#    echo "SET_SERVERS=true" >> DEBIAN/postinst
	#    echo "SET_SERVERS=true" >> DEBIAN/postrm
	echo "db_input high \$PACKAGE_NAME/ask_mds || true" >> DEBIAN/config
	echo "db_input high \$PACKAGE_NAME/ask_cds || true" >> DEBIAN/config
	echo "db_input high \$PACKAGE_NAME/ask_webui || true" >> DEBIAN/config
	echo "db_input high \$PACKAGE_NAME/ask_us || true" >> DEBIAN/config
    else
	echo "db_input high \$PACKAGE_NAME/ask_us || true" >> DEBIAN/config
    fi
    
    echo "db_go || true" >> DEBIAN/config
    cat "$SOURCE_DIR/tools/package_template/DEBIAN/config" >> DEBIAN/config
    chmod 0555 DEBIAN/config
fi
fi


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

cd - > /dev/null
info_mesg "packaging $NAME for architecture " "$ARCH .."
dpkg-deb -b $PACKAGE_DIR > /dev/null
mv $PACKAGE_DIR.deb .
info_mesg "created " "$NAME.deb"
if [ -n "$COPY_REMOTE" ];then
    info_mesg "copying to APT server " "$APT_DEST"
    scp $NAME.deb $APT_DEST
fi
#rm -rf $PACKAGE_DIR
