
if [ ! -d /usr/local/app/patchs ]; then
    mkdir -p /usr/local/app/patchs
    mkdir -p /data/tars/patchs/tars
    mkdir -p /data/tars/patchs/TmpPatch	
    mkdir -p /data/tars/patchs/tars.upload
    ln -s /data/tars/patchs/tars /usr/local/app/patchs/tars
    ln -s /data/tars/patchs/TmpPatch  /usr/local/app/patchs/TmpPatch
    ln -s /data/tars/patchs/tars.upload /usr/local/app/patchs/tars.upload
fi

bin="rsync"

PID=`ps -eopid,cmd | grep "$bin" |  grep -v "grep" |awk '{print $1}'`

echo $PID

if [ "$PID" != "" ]; then
        kill -9 $PID
        echo "kill -9 $PID"
fi

rsync --address=web.tars.com --daemon --config=/usr/local/app/tars/tarspatch/conf/rsync.conf &

#mkdir -p /usr/local/app/patchs
#cd /usr/local/app/patchs
#mkdir -p /data/tars/patchs/tars
#mkdir -p /data/tars/patchs/TmpPatch	
#mkdir -p /data/tars/patchs/tars.upload
#ln -s /data/tars/patchs/tars /usr/local/app/patchs/tars
#ln -s /data/tars/patchs/TmpPatch  /usr/local/app/patchs/TmpPatch
#ln -s /data/tars/patchs/tars.upload /usr/local/app/patchs/tars.upload

