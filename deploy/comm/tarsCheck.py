#!/usr/bin/python
# encoding: utf-8
import sys
import os
import tarsLog
from tarsUtil import *
log = tarsLog.getLogger()
checkServer =["git","gcc","gcc-c++","yasm","glibc-devel","flex","bison","cmake","ncurses-devel","zlib-devel","autoconf"]
def do():
    check()
    return

def check():
    for server in checkServer:
        log.infoPrint(" check and install server {}".format(server))
        installIfNotExistsByYum(server)
    return

def hasInstallServer(server):
    result = doCmdIgnoreException("which {}".format(server))
    if result["status"] == 0:
        return True
    else:
        return False

def installIfNotExistsByYum(server):
    if isCentos():
        os.system("yum install  -y {}".format(server))
    elif isSuse():
        os.system("zypper install  -y {}".format(server))
    elif isUbuntu():
        os.system("apt-get install  -y {}".format(server))
    else:
        raise Exception("ERROR ,NO SUPPORT THIS PLATFORM ,ONLY SUPPORT CENTOS ,SUSE ,UBUNTU .")
    return

if __name__ == '__main__':
    pass