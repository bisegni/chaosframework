#!/bin/bash
curr=`pwd -P`
outdir=$curr
startdir=$1
pname="UnitServer"

exename=`basename $0`
lista_cu=();

lista_driver="";
lista_driver_ns=();
lista_h=();
lista_hd=();
lista_lib=""
skipdir=();
TEMP_DIR="/tmp/$USER/"
TEMP_CMAKE="$TEMP_DIR/cmakelist.txt"
mkdir -p $TEMP_DIR

while getopts i:o:n:hs: opt; do
    case $opt in
	i) startdir=$OPTARG
	    ;;
	o) outdir=$OPTARG
	    ;;
	n) pname=$OPTARG
	    ;;
	s) skipdir+=($OPTARG)
	   ;;
	h) echo -e "Usage : $exename [-i <input directory>] [-o <output directory>] [-n <project name>] [-s <directory to skip>]\n-i <input direcory>: directory that contains CUs\n-o <output directory>: Unit Server project dir\n-n <project name>: name of the unit server project\n-s <directory>: skip the specified directory"
	    exit 0
	    ;;
    esac
   
done
OS=`uname -s`
if [ $OS == "Darwin" ];then
    if gsed --v >& /dev/null;then
	echo "using gsed"
	SED=gsed
    else 
	echo "%% warning  gsed is required, scripts may misbehave, please install brew install gnu-sed"
	SED=sed
    fi
else
    SED=sed
fi
if ! [[ "$outdir" =~ ^/ ]] ; then
    outdir=$curr/$outdir
    
fi
if [ ! -d "$startdir" ]; then
    echo "## directory not found $startdir"
    exit 1
fi

pushd $startdir > /dev/null
curr=`pwd -P`
findopt=""
echo "startdir $curr"
lista_ignore=`find . -name UnitServerIgnore`

for m in $lista_ignore;do
    dir=$(dirname $m)
    lista_ignore_dir="$lista_ignore_dir $dir"
    skipdir+=($lista_ignore_dir)
    p=`echo "$dir"|sed 's/\./\*/g'`
    findopt="$findopt -not -path \"$p*\""
 done

cmd="find . -name \"CMakeLists.txt\" $findopt"
listcmake=$(eval $cmd);

for m in $listcmake;do
    dir=`dirname $m`
    if [ ! -d $dir/CMakeFiles ] && [ "$dir" != "." ];then
	p=`echo "$dir"|sed 's/\./\*/g'`
	findopt="$findopt -not -path \"$p*\""
	echo "* skipping directory not configured $dir" 
    fi
done



cmd="find . -name \"*.h\" $findopt"
listah=$(eval $cmd)
cmd="find . -name \"*.cpp\" $findopt"
listacpp=$(eval $cmd)

project_dir=$outdir/$pname
echo "* creating $project_dir"
mkdir -p $project_dir
sourcedir_base=`basename $startdir`
echo "* linking $curr into $project_dir/" 
ln -sf $curr $project_dir


parent=`dirname $startdir`
prefix=`basename $startdir`

incdir_list=""

CHAOS_EXCLUDE_DIR="$CHAOS_EXCLUDE_DIR"
function to_skip(){

    for s in ${skipdir[@]} $CHAOS_EXCLUDE_DIR; do
	if [[ "$1" =~ $s ]]; then
	    return 0
	fi
    done
    return 1
}
for c in $listah; do
    if to_skip $c; then
	echo "* skipping $c"
	continue;
    fi
    
    header=`echo $c | $SED 's/src\///' | $SED 's/source\///' | $SED "s/\.\//$prefix\//g"`;
    filenamespace=""
#    namespace=`grep -o 'namespace\s\+\w\+\s*{' $c |$SED 's/namespace\s\+\(\w\+\)\s*{/\1/g'`;
    namespace=`grep -o 'namespace\s\+\w\+\s*{' $c | $SED 's/namespace\ *//g' |$SED 's/[\ {]//g'`
  #  namespace=`grep -o 'namespace\s\+\w\+' $c | $SED 's/^namespace //g'`
#`grep -o 'namespace\s\+\w\+' $c | $SED 's/namespace\ *//g' | tr '\n' ' '`;
    oldbb=$bb
    bb=$(dirname $c)
   # if [ "$bb" != "." ] && [ "$bb" != "$oldbb" ]; then

   # 	incdir_list+=" $startdir/$bb"
   # 	incdir_list+=" $bb"
   # fi
    
    for n in $namespace; do
	if [ -z "$filenamespace" ];then
	    filenamespace="$n";
	else
	    filenamespace="$filenamespace::$n";
	fi
    done;
    if [ -n "$filenamespace" ];then
	filenamespace="::$filenamespace"
    fi
    # plugin=`grep ADD_CU_DRIVER_PLUGIN_SUPERCLASS $c -s`;
    # if [ -n "$plugin" ]; then
    # 	if [[ "$plugin" =~ class\ +(.+)\: ]]; then
	    
    # 	    lista_driver+=("${BASH_REMATCH[1]}");
    # 	    lista_driver_ns+=("$filenamespace");
    # 	    lista_hd+=("$header");
    # 	fi
    # fi
    if [ -n "$filenamespace" ];then
	filenamespace="$filenamespace::"
    fi
    cu=`grep -s -o "PUBLISHABLE_CONTROL_UNIT_INTERFACE(.\+)" $c | $SED 's/[:alpha:_]\+:://g'|$SED "s/PUBLISHABLE_CONTROL_UNIT_INTERFACE(\(\w\+\))$/REGISTER_CU($filenamespace\1)/g"`;
    if [ -n "$cu" ]; then
	echo "* got CU in $header"
	lista_cu+=("$cu");
	lista_h+=("$header");
    fi
done
 for c in $listacpp; do
     

     plugin=`grep -s -o "OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(.\+)" $c| $SED "s/OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(\(\w\+\),.\+,\(.*\)::\(.\+\))/REGISTER_DRIVER(\2,\3)/g"| $SED "s/OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(\(\w\+\),.\+,\(.\+\))/REGISTER_DRIVER(,\2)/g"| $SED  s/\ //g`
#     echo "===> plugin: $plugin"
     if [[ "$plugin" =~ REGISTER_DRIVER\(.*,.+\) ]];then
	 lista_driver="$lista_driver $plugin"

	 nameh=`echo $c | $SED s/\.cpp/\.h/|$SED 's/src\///' | $SED 's/source\///' | $SED "s/\.\//$prefix\//g"`;
	 lista_hd+=($nameh)
     fi
#     drv=`grep -s REGISTER_PLUGIN\( $c | $SED s/REGISTER_PLUGIN\(/REGISTER_DRIVER\(/`;
#     rr=`basename $startdir`


#     if [ -n "$drv" ]; then
# 	lista_driver+=("$drv");
# 	lista_h+=("$header");
#     fi
 done;
cmd="find . -name \"CMakeLists.txt\" $findopt"
listcmake=$(eval $cmd);
listadep=""
cmake_things=""

for c in $listcmake;do
    if to_skip $c; then
	echo "* skipping $c"
	continue;
    fi
    testmake=`dirname $c`
    if [ ! -d "$testmake/CMakeFiles" ];then
	echo "* skipping $c not compiled"
	continue
    fi
    
    CL=$c
    dir=`dirname $c`
  # if ! make -C $dir >& /dev/null;then
  # 	echo "skipping $dir because does not compile"
  # 	continue;
  # fi
    project_name_tmp=`grep -i project $c`
    project_name=""
    cmake_things+="##### from $startdir/$c ###### \n"
    
    if [[ "$project_name_tmp" =~ \(([a-zA-Z0-9_]+)\) ]];then
	project_name=${BASH_REMATCH[1]}
       
	cat $c| $SED "s/\${PROJECT_NAME}/$project_name/g" > $TEMP_CMAKE
	CL="$TEMP_CMAKE"
	echo "replacing $project_name"
    fi
    




    cmake_include="$(grep INCLUDE_DIRECTORIES $c)"
    if [ -n "$cmake_include" ]; then
	if [[ "$cmake_include" =~ .+\((.+)\) ]];then
	    myinclude=${BASH_REMATCH[1]}
	    cmake_things+="INCLUDE_DIRECTORIES("
	    for d in $myinclude;do
		echo "=== adding $startdir/$dir/$d"
		cmake_things+=" $startdir/$dir/$d"
	    done
	    cmake_things+=")\n"
	fi
    fi
    
    # if [ -n "$cmake_include" ]; then
    # 	cmake_things+="$cmake_include\n"
    # fi

    cmake_include="$(grep ADD_DEFINITIONS $c)"
    if [ -n "$cmake_include" ]; then
	cmake_things+="$cmake_include\n"
    fi
    cmake_things+="#########################\n"
    varl=`grep -i add_library $CL` # | grep SHARED`;
    incdir=`dirname $startdir/$c | sed 's/\.\///g'`

    parent=`dirname $incdir`

    path=`basename $parent`/`basename $incdir`
    incdir_list="$incdir_list \${CMAKE_INSTALL_PREFIX}/include/$path "
#    incdir_list="$incdir_list \${CMAKE_INSTALL_PREFIX}/include/$c"
#    echo "start dir $startdir"
#    for h in `find $startdir -name "*.h"`; do
#    done
    for var in $varl; do
	if [ -n "$var" ]; then
	    if [[ "$var" =~ .+\((.+) ]];then
		if [ -n "${BASH_REMATCH[1]}" ];then
		    mylib=${BASH_REMATCH[1]}
		 #   echo "-->$mylib"
		    lista_lib="$lista_lib $mylib";
		    varlink=`grep -i "target_link_libraries\ *(\ *$mylib" $CL | tail -1`;
		    
		    patt="\($mylib\ +(.+)\ *\)"
		    if [[ "$varlink" =~ $patt ]]; then
			echo "---link-->${BASH_REMATCH[1]}"
			if [ -z "$listadep" ]; then
			    listadep="${BASH_REMATCH[1]}";
		#	   
			else
			    listadep="$listadep ${BASH_REMATCH[1]}";
			fi
			echo "---dep-->$listadep"

		    fi
		fi
	    fi
	fi;
    done;
done;

echo -e "// Project $pname" > $project_dir/main.cpp
echo -e "// includes the available CU in \"$prefix\"" >> $project_dir/main.cpp
echo -e "// generated by \"$exename\"\n\n" >> $project_dir/main.cpp
echo -e "#include <chaos/common/chaos_constants.h>\n#include <chaos/cu_toolkit/ChaosCUToolkit.h>\n#include <chaos/common/exception/CException.h>\n" >> $project_dir/main.cpp

echo -e "/*** CU Types ****/\n">>$project_dir/main.cpp
for h in ${lista_h[@]};do
    
    echo "#include<$h>" >> $project_dir/main.cpp
done
echo -e "/*** Drivers ****/\n">>$project_dir/main.cpp
for h in ${lista_hd[@]};do
    
    echo "#include<$h>" >> $project_dir/main.cpp
done
lista_unica=`echo -e $listadep| $SED 's/ /\n/g'|tr '\n' ' '`

echo -e "\n\nint main(int argc,const char**argv){">>$project_dir/main.cpp
echo -e "\ttry{\n">>$project_dir/main.cpp
echo -e "\t\tchaos::cu::ChaosCUToolkit::getInstance()->init(argc, argv);">>$project_dir/main.cpp
arr=0
for c in ${lista_cu[@]}; do
    echo -e "\t\t$c; /* file: ${lista_h[$arr]} */" >> $project_dir/main.cpp
    ((arr++))
done
arr=0
for c in $lista_driver; do
    
#    echo -e "\t\tREGISTER_DRIVER(${lista_driver_ns[arr]},$c); /* file: ${lista_hd[$arr]} */" >> $project_dir/main.cpp
     echo -e "\t\t$c; /* file: ${lista_hd[$arr]} */" >> $project_dir/main.cpp
  ((arr++))
done
echo -e "\t\tchaos::cu::ChaosCUToolkit::getInstance()->start();" >> $project_dir/main.cpp

echo -e "\t} catch (CException& e) {\n\t\tstd::cerr<<\"Exception:\"<<std::endl;\n\t\tstd::cerr<< \"domain	:\"<<e.errorDomain << std::endl;\n\t\tstd::cerr<< \"cause	:\"<<e.errorMessage << std::endl;return -1;\n\t} catch (program_options::error &e){\n\t\tstd::cerr << "\"Unable to parse command line: \"" << e.what() << std::endl;return -2;\n\t} catch (...){\n\t\tstd::cerr << \"unexpected exception caught.. \" << std::endl;return -3;\n\t}return 0;\n}\n" >> $project_dir/main.cpp

echo "cmake_minimum_required(VERSION 2.8)" > $project_dir/CMakeLists.txt
echo "find_package(chaos MODULE REQUIRED)" >> $project_dir/CMakeLists.txt
echo "include_directories(../../ \${chaos_INCLUDE_DIRS})">> $project_dir/CMakeLists.txt

echo "SET(src main.cpp )" >>  $project_dir/CMakeLists.txt
echo -e "$cmake_things" >> $project_dir/CMakeLists.txt
if [ -n "$incdir_list" ]; then
    echo "INCLUDE_DIRECTORIES(\${CMAKE_INCLUDE_PATH} \${CMAKE_INSTALL_PREFIX}/include $incdir_list)" >> $project_dir/CMakeLists.txt
fi
echo -e "IF(BUILD_FORCE_STATIC)\nSET(CMAKE_EXE_LINKER_FLAGS \"-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive\")\nENDIF()\n" >>  $project_dir/CMakeLists.txt
echo "ADD_EXECUTABLE($pname \${src})" >>  $project_dir/CMakeLists.txt

echo "TARGET_LINK_LIBRARIES($pname $lista_lib $lista_unica $lista_lib common_debug \${FrameworkLib})" >>  $project_dir/CMakeLists.txt
echo "INSTALL_TARGETS(/bin $pname)" >>  $project_dir/CMakeLists.txt

popd > /dev/null
echo "* Generated $project_dir"
