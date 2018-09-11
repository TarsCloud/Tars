#!/usr/bin/python
# encoding: utf-8
import comm.tarsLog as tarsLog
from comm.tarsUtil import *
import shutil
def do():
    doCmd("ps -aux|grep resin|grep -v grep|awk '{print \" kill -9\" ,$2}'|bash")
    doCmd("ps -aux|grep tars|grep -v grep|awk '{print \" kill -9\" ,$2}'|bash")
    doCmd("mysql -utars -ptars2015 -e 'drop database if exists db_tars'")
    doCmd("mysql -utars -ptars2015 -e 'drop database if exists tars_stat'")
    doCmd("mysql -utars -ptars2015 -e 'drop database if exists tars_property'")
    doCmd("mysql -utars -ptars2015 -e 'drop database if exists db_tars_web'")
    if os.path.exists("/usr/local/app/web"):
        shutil.rmtree("/usr/local/app/web")
    if os.path.exists("/usr/local/app/tars"):
        shutil.rmtree("/usr/local/app/tars")
    return
if __name__ == '__main__':
    do()
    pass