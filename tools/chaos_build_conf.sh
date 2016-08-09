#!/bin/bash

usname="TEST_UNIT"
cuname=""
dataservers=()
instances=()
nus=1
outfile="test_conf.mds"
infile=()
device_classes=()
setcuclass="true"
cucounter=0
while getopts i:o:u:c:n:hd:j: opt; do
    case $opt in
	i) infile+=($OPTARG)
	    ;;
	n) instances+=($OPTARG)
	    ;;
	o) outfile="$OPTARG"
	    if ! echo > $outfile;then
		echo "## cannot open file $outfile"
		exit 1
	    fi
	    ;;
	u) usname="$OPTARG"
	    ;;
	c) cuname="$OPTARG"
	    setcuclass=""
	    ;;
	d) dataservers+=("$OPTARG")
	    ;;
	j) nus=$OPTARG
	    ;;
	h)
	    echo -e "Usage:$0 -i <configuration CU template0 file> [-i <configuration CU template1 file] [-n <#instance0> [1]] [-o <outputconfiguration> ] [-u <unit server name>] [-c <unit cuname prefix>] [-d <dataserver url>] [-j <number of us>]\n-i <file>: CU configuration template\n-n <#instances>: number of instances of configuration template\n-u <name>: name of the Unit Server container [$usname]\n-c <cu name prefix> [$cuname]\n-o <outfile> [$outfile]\n-d <url>:dataserver url [localhost:11211]\n-j <# number of US>: number of US to create [$nus]"
	    exit 0
	    ;;
    esac
done;

if [ ${#dataservers} -eq 0 ];then
    echo "* no dataserver given assuming localhost"
    dataservers+=("localhost:1700:32000|0")
fi

for c in ${infile[@]}; do
    if [ ! -f "$c" ]; then
	echo "## configuration file $c not found"
	exit 1
    else
	classes=`cat $c | grep -o '"cu_type"\ *:\ * "[[:alpha:]:]*"'`
	if [[ $classes =~ :\ *\"(.+)\" ]]; then
	    device_classes+=(${BASH_REMATCH[1]})
	fi
    fi
done



function generate_cu(){
    confile="$1"
    ninstances=$2
    usown="$3"
    echo "* generating $ninstances instances of $confile"
    if [ ! -f "$confile" ]; then
	echo "## file $confile not found"
	exit 1
    fi
    

    variables=`cat $confile | grep -o '\$r[0-9]*'|sort -n |uniq`
    expressions=`cat $confile | grep -o '\$r[0-9]*([0-9\.,]*)'`
    if [ -n "$setcuclass" ];then
	local name=`basename $confile`
	cuname=`echo $name|sed s/\.conf//|tr [:lower:] [:upper:]`

    fi
    
    while ((ninstances--)); do
	randoms=()
	for r in $variables;do
	    rand=$RANDOM
	    if [[ "$r" =~ r([0-9]+) ]]; then
		randoms[${BASH_REMATCH[1]}]=$rand;
	    fi
	done
	expression_value=()
	replace=""
	for r in $expressions;do
	    val=`echo $r|tr ',' ' '`
	    if [[ "$val" =~ r([0-9]+)\((.+)\) ]]; then
		curr_arr=(${BASH_REMATCH[2]})
	    sel=$((${randoms[${BASH_REMATCH[1]}]}%${#curr_arr[@]}));
	    expression_value+=(${curr_arr[$sel]})
	    r_escape=`echo $r | sed 's/(/\\(/g' |sed 's/)/\\)/g'`
	    #	echo "-$sel $r =${curr_arr[$sel]}"
	    if [ -z "$replace" ];then
		replace="cat $confile| sed 's/$r_escape/${curr_arr[$sel]}/g'";
	    else
		replace="$replace | sed 's/$r_escape/${curr_arr[$sel]}/g'";
	    fi
	    fi
	done

	if [ -n "$setcuclass" ];then
	    cucounter=$ninstances
	fi
	if [ -n "$replace" ];then
	    eval $replace | sed s/\$US/$usown/g|sed s/\$CUID/$usown\\/$cuname\_$cucounter/g>> $outfile
	else
	    cat $confile | sed s/\$US/$usown/g|sed s/\$CUID/$usown\\/$cuname\_$cucounter/g>> $outfile
	fi
	if (( $ninstances > 0 ));then
	    echo ",">> $outfile
	fi
	if [ -z "$setcuclass" ];then
	    ((cucounter++))
	fi

    done
}

function add_array(){
    var="$1"
    max=$2
    if [ $cnt -eq 0 ]; then
	echo "[" >> $outfile
    fi
    echo -n "$var" >> $outfile
    ((cnt++))
    if [ $cnt -lt $max ]; then
	echo "," >> $outfile
    fi
    if [ $cnt -eq $max ]; then
	echo -n "]" >> $outfile
    fi


}
function generate_array(){
    echo "\"$1\":" >> $outfile
    cnt=0
}
function generate_dataservers(){
    generate_array "data_servers";
    for d in ${dataservers[@]}; do
	add_array "{\"hostname\":\"$d\" , \"id_server\":$cnt,\"is_live\" : true}" ${#dataservers[@]}
    done;

}



function generate_classes(){
    generate_array "device_classes";
    for d in ${device_classes[@]}; do
	if [[ "$d" =~ (.+)::(.+)$ ]]; then
	    alias=${BASH_REMATCH[2]}
	    interface=${BASH_REMATCH[1]}
	else
	    alias=$d
	    interface=""
	fi
	add_array "{\"device_class_alias\" : \"$alias\" , \"device_class\" : \"$d\" , \"device_class_interface\" : \"$interface\" , \"device_source\" : \"TEST\"}" ${#device_classes[@]}

    done
}

function generate_us(){


    echo -ne "\"us\":\n[" >> $outfile;

    for (( d=0;d<$nus;++d)); do
	name="$usname""_$d"
	cucounter=0
	echo -ne "{ \"unit_server_alias\" : \"$name\", \"private_key\" :  null  , \"public_key\" :  null  , \n\"cu_desc\" :[" >> $outfile
	ninstp=0
	for c in ${infile[@]}; do
	    if (( $ninstp >= ${#instances[@]} )); then
		ninst=1
#		echo "number of instances ${#instances[@]}"
	    else
		ninst=${instances[$ninstp]}
#		echo "- $ninstp ${instances[$ninstp]} number of instances ${#instances[@]}"
	    fi
	    generate_cu $c $ninst $name
	    ((ninstp++))
	    if [ $ninstp -lt ${#infile[@]} ];then
		echo "," >> $outfile
	    fi
	done
	echo "]}" >> $outfile ## close cu array close us
	if (( $d < $((nus -1)) )); then
	    echo "," >> $outfile
	fi
    done
    echo "]" >> $outfile ## close us array
}
function generate_conf(){
    echo "{" > $outfile
    generate_dataservers;
    echo "," >> $outfile
    generate_classes;
    echo "," >> $outfile
    generate_us;
    echo "}" >> $outfile
}

generate_conf
exit 0

