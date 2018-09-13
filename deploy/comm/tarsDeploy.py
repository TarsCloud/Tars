#!/usr/bin/python
# encoding: utf-8
import tarsLog
import shutil
import os
import sys
import stat
from tarsUtil import *
log = tarsLog.getLogger()
tarsDeploy = "/usr/local/app/tars"
tarsDeployFrameBasicServerList = ["tarsregistry", "tarsnode", "tarsAdminRegistry", "tarspatch","tarsconfig"]
tarsDeployFrameCommServerList = ["tarsnotify", "tarsstat", "tarsproperty", "tarsquerystat", "tarsqueryproperty", "tarslog", "tarsauth"]
baseDir = getBaseDir()
def do():
    log.infoPrint("initDB start ...")
    initDB()
    log.infoPrint("initDB success ")
    log.infoPrint("deploy frameServer start ...")
    deployFrameServer()
    log.infoPrint("deploy frameServer success ")
    log.infoPrint("deploy web start ... ")
    deployWeb()
    log.infoPrint("deploy web success")
    return

def getDBDir():
    dbDir = baseDir+"/framework/sql/"
    return dbDir

def deployFrameServer():
    for server in tarsDeployFrameBasicServerList:
        srcDir = "{}/framework/build/deploy/{}".format(baseDir,server)
        confDir = "{}/framework/deploy/{}".format(baseDir,server)
        dstDir = "/usr/local/app/tars/{}".format(server)
        log.infoPrint(" deploy {} start srcDir is {} , confDir is {} , dstDir is {}  ".format(server,srcDir,confDir,dstDir))
        copytree(srcDir,dstDir)
        copytree(confDir,dstDir)
        updateConf(server)
        os.chmod(dstDir+"/util/start.sh",stat.S_IXGRP)
        doCmd(dstDir+"/util/start.sh".format(server))
        log.infoPrint(" deploy {}  sucess".format(server))

    for server in tarsDeployFrameCommServerList:
        srcDir = "{}/framework/build/deploy/{}".format(baseDir,server)
        confDir = "{}/framework/deploy/{}".format(baseDir, server)
        dstDir = "/usr/local/app/tars/{}".format(server)
        dstBinDir = "/usr/local/app/tars/{}/bin/".format(server)
        log.infoPrint(" deploy {} start srcDir is {} , confDir is {} , dstDir is {}  ".format(server, srcDir, confDir, dstDir))
        if not os.path.exists(dstBinDir):
            os.makedirs(dstBinDir)
        shutil.copy(srcDir+"/"+server,dstBinDir+"/"+server)
        copytree(confDir,dstDir)
        updateConf(server)
        os.chmod(dstDir+"/util/start.sh",stat.S_IXGRP)
        doCmd(dstDir+"/util/start.sh".format(server))
    return

def updateConf(server):
    mysqlHost = getCommProperties("mysql.host")
    localIp = getLocalIp()
    replaceConf("/usr/local/app/tars/{}/conf/tars.{}.config.conf".format(server,server),"localip.tars.com",localIp)
    replaceConf("/usr/local/app/tars/{}/conf/tars.{}.config.conf".format(server, server), "192.168.2.131", localIp)
    replaceConf("/usr/local/app/tars/{}/conf/tars.{}.config.conf".format(server, server), "db.tars.com", mysqlHost)
    replaceConf("/usr/local/app/tars/{}/conf/tars.{}.config.conf".format(server, server), "registry.tars.com", localIp)
    replaceConf("/usr/local/app/tars/{}/conf/tars.{}.config.conf".format(server, server), "10.120.129.226", localIp)
    if "tarsnode" == server:
        replaceConf("/usr/local/app/tars/{}/util/execute.sh".format(server, server), "registry.tars.com", localIp)
    return

def deployWeb():
    mysqlHost = getCommProperties("mysql.host")
    localIp = getLocalIp()
    result = doCmdIgnoreException("nvm --version")
    if result["status"] != 0:
        log.infoPrint("install nvm start...")
        doCmd("wget -qO- https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh | bash ")
        log.infoPrint("install nvm success")
    else:
        log.infoPrint("nvm version  is {}".format(result["output"]))
    result = doCmdIgnoreException("node --version")
    if result["status"] != 0:
        log.infoPrint("install node start...")
        doCmd(" export NVM_DIR=\"$HOME/.nvm\";[ -s \"$NVM_DIR/nvm.sh\" ] && \. \"$NVM_DIR/nvm.sh\";source /etc/profile && nvm install v8.11.3")
        log.infoPrint("install node success")
    else:
        log.infoPrint("node version  is {}".format(result["output"]))
    result = doCmdIgnoreException("pm2 --version")
    if result["status"] != 0:
        doCmd("export NVM_DIR=\"$HOME/.nvm\";[ -s \"$NVM_DIR/nvm.sh\" ] && \. \"$NVM_DIR/nvm.sh\";source /etc/profile &&  npm install -g pm2 --registry=https://registry.npm.taobao.org")
    else:
        log.infoPrint(" pm2 version  is {}".format(result["output"]))

    replaceConf("/usr/local/app/web/config/tars.conf", "registry.tars.com", localIp)
    replaceConf("/usr/local/app/web/config/webConf.js", "db.tars.com", mysqlHost)

    doCmd("cd /usr/local/app/web;export NVM_DIR=\"$HOME/.nvm\";[ -s \"$NVM_DIR/nvm.sh\" ] && \. \"$NVM_DIR/nvm.sh\";source /etc/profile && npm install --registry=https://registry.npm.taobao.org;npm run prd")
    return

def initDB():
    dbDir=getDBDir()
    mysqlHost = getCommProperties("mysql.host")
    mysqlPort = getCommProperties("mysql.port")
    mysqlRootPassWord = getCommProperties("mysql.root.password")
    localIp = getLocalIp()
    log.info(" dbDir is{} , mysqlHost is {} , mysqlPort is {} mysqlRootPassWord is {} ,localIp is {} ".format(dbDir,mysqlHost,mysqlPort,mysqlRootPassWord,localIp))
    doCmd("mysql -uroot -p{}  -e \"grant all on *.* to 'tars'@'%' identified by 'tars2015' with grant option;flush privileges;\"".format(mysqlRootPassWord))
    log.info(" the mysqlHost is {} , mysqlPort is {},  mysqlRootPassWord is {}".format(mysqlHost,mysqlPort,mysqlRootPassWord))

    replaceConfDir(dbDir, "192.168.2.131", localIp)
    replaceConfDir(dbDir, "db.tars.com", mysqlHost)
    replaceConfDir(dbDir, "10.120.129.226", localIp)


    doCmd("mysql -utars -ptars2015 -e 'drop database if exists db_tars;create database db_tars'")
    doCmd("mysql -utars -ptars2015 -e 'drop database if exists tars_stat;create database tars_stat'")
    doCmd("mysql -utars -ptars2015 -e 'drop database if exists tars_property;create database tars_property'")
    doCmd("mysql -utars -ptars2015 -e 'drop database if exists db_tars_web;create database db_tars_web'")

    doCmd("mysql -utars -ptars2015 db_tars < {}/db_tars.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars -e 'truncate table t_server_conf;truncate table t_adapter_conf;truncate table t_node_info;truncate table t_registry_info '")
    doCmd("mysql -utars -ptars2015 db_tars < {}/tarsconfig.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars < {}/tarslog.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars < {}/tarspatch.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars < {}/tarsproperty.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars < {}/tarsqueryproperty.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars < {}/tarsquerystat.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars < {}/tarsstat.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars < {}/tarsnotify.sql".format(dbDir))
    doCmd("mysql -utars -ptars2015 db_tars_web < {}/t_tars_files.sql".format(dbDir))
    return

if __name__ == '__main__':
    pass