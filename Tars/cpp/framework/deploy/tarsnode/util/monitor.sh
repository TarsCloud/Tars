#!/bin/sh
cd /usr/local/app/tars/tarsnode/util

bin="/usr/local/app/tars/tarsnode/bin/tarsnode"

$bin --monitor --config=/usr/local/app/tars/tarsnode/conf/tarsnode.conf

ex=$?

if [ $ex -ne 0 ]; then
#    echo "monitor:"$ex", restart tarsnode"
    sh /usr/local/app/tars/tarsnode/util/start.sh
fi

