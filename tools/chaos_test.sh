#!/bin/bash
cmd=$1

scriptdir=$(dirname $0)
if [ -z "$CHAOS_TOOLS" ];then
    export CHAOS_TOOLS=`cd $scriptdir && pwd`
fi

source $scriptdir/common_util.sh

testdir=$scriptdir/test
testlists=()
test_found=0
report_file="/dev/null"
stop_on_error=""
CALLGRIND_OPT="valgrind --tool=callgrind"

info_mesg "CHAOS_TOOLS=" "$CHAOS_TOOLS"
if [ -z "$CHAOS_PREFIX" ];then
    error_mesg "CHAOS_PREFIX " "must be defined"
    exit 1
fi

export CHAOS_RUNPREFIX=""
export CHAOS_RUNOUTPREFIX=""
export CHAOS_RUNTYPE=""
export CHAOS_TEST_REPORT=""
export CHAOS_TEST_DEBUG=""
export GOOGLE_PROFILE=""
usage(){
    echo -e "Usage :$0 [-t <testlist0> .. -t <testlistN>] [-m <mdsserver> ] [-d <directory of testlists> [$testdir]] [-r csv report_file] [-v] [-k]\n-t <testlist>: choose a particular testlist\n-d <dir>: execute all the testlist in a given directory\n-r <report>:create a CSV file with test summary\n-k:stop on error\n-v:enable callgrind\n-g:activate debug log\n-p <normal|strict|draconian>:activate google heap check\n"
}
while getopts m:t:d:r:kvgp: opt; do
    case $opt in
	p)
	    if [ -z "$LD_PRELOAD" ];then
		error_mesg "To enable GOOGLE Profiling you have to define LD_PRELOAD pointing to a tcmalloc.so location"
		exit 1
	    fi
	    info_mesg "google heap check enabled " "$OPTARG"
	    GOOGLE_PROFILE="HEAPCHECK=$OPTARG LD_PRELOAD=$LD_PRELOAD"
	    ;;
	t) 
	if [ ! -f "$OPTARG" ]; then
	    error_mesg "test list $OPTARG not found"
	    exit 1
	fi
	testlists+=($OPTARG)
	;;
	k)
	    stop_on_error="true"
	    ;;
	v)
	    export CHAOS_RUNPREFIX="$CALLGRIND_OPT"
	    export CHAOS_RUNOUTPREFIX="--callgrind-out-file="
	    export CHAOS_RUNTYPE="callgrind"
	    ;;
	r)
	    report_file="$PWD/$OPTARG"
	    export CHAOS_TEST_REPORT=$report_file
	    echo "test group;test name; status; exec time (s); %cpu ; %mem; desc" > $report_file
	    info_mesg "enable report in " "$CHAOS_TEST_REPORT"
	    ;;
	m)
	    export CHAOS_MDS=$OPTARG
	    info_mesg "using MDS " "$CHAOS_MDS"
	    ;;
	d)
	    if [ -d "$OPTARG" ]; then
		testdir=$OPTARG
	    else
		error_mesg "invalid directory $OPTARG"
		exit 1
	    fi
	    ;;
	g)
	    export CHAOS_TEST_DEBUG="--log-level debug "
	    info_mesg "activating debug logging"
	    ;;

	*)
	    usage
	    exit 1
	    ;;
    esac
done

if [ ${#testlist[@]} -eq 0 ]; then
    if [ ! -d "$testdir" ]; then
	error_mesg "invalid test directory $testdir" " (please specify -d <testdir>)"
	exit 1
    fi
    lists=`ls $testdir/test_list_*.txt |sort -n`
    for test in $lists; do
	testlist+=($test)
    done
fi


test_prefix;

final_test_list=()
for test in ${testlist[@]}; do
   basedir=`dirname $test`
   tests=`sed s/#.*// $test | cut -d ' ' -f 1`

   for test_test in $tests; do
       if [[ "$test_test" =~ ^/.+ ]];then
	   test_full_dir=$test_test
       else
	   test_full_dir=$basedir/$test_test
       fi
       
       if [ ! -f "$test_full_dir" ]; then
	   error_mesg "test dir \"$test_full_dir\" specified in \"$test\" not found"
	   exit 1
       else
	   if [ -x $test_full_dir ];then
	       final_test_list+=($test_full_dir)
	       ((test_found++))
	   else
	       warn_mesg "not executable found " "$test_full_dir"
	   fi
       fi
   done
done

info_mesg "found $test_found test(s).."
error_list=()
ok_list=()
status=0

for test in ${final_test_list[@]};do
    info_mesg "starting test $test ..."
    group_test=`dirname $test`
    group_test=`basename $group_test`
    test_name=`basename $test`
    pushd `dirname $test` >/dev/null
    start_profile_time
    echo "----------------------------------------------------------"
    ./$test_name
    res=$?
    status=$(($status + $res))
    echo "----------------------------------------------------------"
    exec_time=$(end_profile_time)
    popd >/dev/null
    if [ $res -eq 0 ]; then
	ok_list+=("$test")
	
	info_mesg "\e[32mTEST \"$test\" ($exec_time s) OK\e[39m"

    else

	error_list+=("$test")
	info_mesg "\e[31mTEST \"$test\" ($exec_time s) FAILED\e[39m"

	if [ -n "$stop_on_error" ];then
	    exit 1
	fi
    fi
done
if [ "$CHAOS_RUNTYPE" == "callgrind" ]; then
    if callgrind_control --dump >& /dev/null; then
	info_mesg "look callgrind dump in $CHAOS_PREFIX/log"

    else
	warn_mesg "cannot dump callgrind"
    fi

fi

exit $status


