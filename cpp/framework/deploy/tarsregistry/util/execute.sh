#!/bin/sh
ulimit -a
export PATH=${PATH}:/usr/local/tars/bin;
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/app/gperftool/lib;
#export HEAPCHECK=strict
export HEAP_CHECK_DUMP_DIRECTORY=.
HEAP_PROFILE_MMAP=true
HEAP_PROFILE_MMAP_LOG=true
#export HEAPPROFILE=registry
bin="/usr/local/app/tars/tarsregistry/bin/tarsregistry"

PID=`ps -eopid,cmd | grep "$bin"| grep "tarsregistry" |  grep -v "grep" |awk '{print $1}'`

echo $PID

if [ "$PID" != "" ]; then
        kill -9 $PID
        echo "kill -9 $PID"
fi
ulimit -c unlimited
$bin  --config=/usr/local/app/tars/tarsregistry/conf/tarsregistry.conf &
