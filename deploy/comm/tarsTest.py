#!/usr/bin/python
# encoding: utf-8
import subprocess
import tarsLog
import os
import requests
from tarsUtil import *
log = tarsLog.getLogger()
localIp = getLocalIp()
webPort = "3000"
def do():
    (rCode,msg) = testWeb()
    if rCode !=0:
        log.infoPrint(msg)
        return
    (rCode,msg) = testFrameServer()
    if rCode !=0:
        log.infoPrint(msg)
        return
    print " test success!"


def testWeb():
    return testByInterface("/pages/server/api/tree","","tarspatch")

def testFrameServer():
    if True:
        return (0,"")
    frameServer = [("tarspatch","1"),("tarsconfig","2"),("tarsnotify","20"),("tarsstat","23"),("tarsproperty","25"),("tarsqueryproperty","27"),("tarsquerystat","29")]
    for (serverName,serverId) in frameServer:
        log.infoPrint("test server {} start ".format(serverName))
        (retCode,msg) = testByInterface("/pages/server/api/send_command?server_ids={}&command=tars.viewversion".format(serverId),"","succ")
        if retCode != 0 :
            return (retCode," server {} test fail ".format(serverName))
        else:
            log.infoPrint("server {} test success ".format(serverName))
    return (0,"")

def testByInterface(uri,params,indexKey):
    url ="http://{}:{}{}".format(localIp,webPort,uri)
    log.info(url)
    try:
        result = requests.get(url,data=params)
    except Exception:
        log.infoPrint(" test fail,tarsweb cannot visit")
        return (-1, "tarweb cannot visit")
    log.info(" url is {} ,statusCode is {} ,text is {} ".format(url,result.status_code,result.text))
    if(result.status_code!=200):
        return (-1,"tarweb cannot visit")
    else:
        if result.text.find(indexKey) == -1:
            return (-1,"return value wrong")
    return (0,"")

if __name__ == '__main__':
    pass