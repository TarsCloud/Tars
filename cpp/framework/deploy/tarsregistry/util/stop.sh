#!/bin/sh

bin="/usr/local/app/tars/tarsregistry/bin/tarsregistry"

PID=`ps -eopid,cmd | grep "$bin"| grep "tarsregistry" |  grep -v "grep"|grep -v "sh" |awk '{print $1}'`

echo $PID

if [ "$PID" != "" ]; then
        kill -9 $PID
            echo "kill -9 $PID"
        fi

