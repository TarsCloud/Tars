#!/bin/sh
export PATH=${PATH}:/usr/local/app/tars/bin:/usr/local/jdk/bin;
bin="/usr/local/app/tars/tarsnode/bin/tarsnode"

PID=`ps -eopid,cmd | grep "$bin"| grep "tarsnode" |  grep -v "grep"|grep -v "sh" |awk '{print $1}'`

echo $PID

if [ "$PID" != "" ]; then
    kill -9 $PID
    echo "kill -9 $PID"
fi

ulimit -c 409600
ulimit -a

$bin --locator="tars.tarsregistry.QueryObj@tcp -h registry.tars.com -p 17890" --config=/usr/local/app/tars/tarsnode/conf/tarsnode.conf &
