#!/bin/sh

bin="/usr/local/app/tars/tarsnode/bin/tarsnode"

PID=`ps -eopid,cmd | grep "$bin"| grep "tarsnode" |  grep -v "grep" |awk '{print $1}'`

echo $PID

if [ "$PID" != "" ]; then
    kill -9 $PID
    echo "kill -9 $PID"
fi


