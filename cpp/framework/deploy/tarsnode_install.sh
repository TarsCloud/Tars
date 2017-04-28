
if [ ! -d /usr/local/app/tars/app_log ]; then
    mkdir -p /data/log/tars
    mkdir -p /usr/local/app/tars
    mkdir -p /data/tars/app_log
    ln -s /data/tars/app_log /usr/local/app/tars/app_log
fi

if [ ! -d /usr/local/app/tars/remote_app_log ]; then
    mkdir -p /data/tars/remote_app_log
    ln -s /data/tars/remote_app_log /usr/local/app/tars/remote_app_log
fi

cd /usr/local/app/tars/


chmod +x tarsnode/util/*.sh

./tarsnode/util/start.sh


