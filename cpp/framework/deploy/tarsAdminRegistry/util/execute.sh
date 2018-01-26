#!/bin/sh
ulimit -a
bin="/usr/local/app/tars/tarsAdminRegistry/bin/tarsAdminRegistry"

PID=`ps -eopid,cmd | grep "$bin"| grep "tarsAdminRegistry" |  grep -v "grep" |awk '{print $1}'`

echo $PID

if [ "$PID" != "" ]; then
        kill -9 $PID
        echo "kill -9 $PID"
fi
ulimit -c unlimited

CONFIG=/usr/local/app/tars/tarsnode/data/tars.tarsAdminRegistry/conf/tars.tarsAdminRegistry.conf

if [ ! -f $CONFIG ]; then
	CONFIG=/usr/local/app/tars/tarsAdminRegistry/conf/adminregistry.conf	
fi

$bin  --config=$CONFIG &

