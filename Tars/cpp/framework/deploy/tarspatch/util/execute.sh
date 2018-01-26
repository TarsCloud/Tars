#!/bin/sh
ulimit -a
bin="/usr/local/app/tars/tarspatch/bin/tarspatch"

PID=`ps -eopid,cmd | grep "$bin"| grep "tarspatch" |  grep -v "grep" |awk '{print $1}'`

echo $PID

if [ "$PID" != "" ]; then
        kill -9 $PID
        echo "kill -9 $PID"
fi
ulimit -c unlimited
$bin  --config=/usr/local/app/tars/tarspatch/conf/tarspatch.conf &
