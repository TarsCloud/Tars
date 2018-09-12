#!/usr/bin/python
# encoding: utf-8
import subprocess
import tarsLog
import os
from tarsUtil import *
log = tarsLog.getLogger()
def do():
    log.infoPrint("pullFramework start ...")
    pullFramework()
    log.infoPrint("pullFramework success")
    log.infoPrint("pullWeb start ...")
    pullWeb()
    log.infoPrint("pullWeb success")
    log.infoPrint("build start ...")
    build()
    log.infoPrint("build sucess")
    return

def pullFramework():
    baseDir = getBaseDir()
    if not os.path.exists("{}/framework/RegistryServer/".format(baseDir)):
        log.infoPrint(" git clone TarsFramework")
        os.system("git clone https://github.com/TarsCloud/TarsFramework.git --recursive {}/framework ".format(baseDir))
    return



def pullWeb():
    baseDir = getBaseDir()
    if not os.path.exists("/usr/local/app/web".format(baseDir)):
        log.infoPrint(" git clone TarsWeb")
        os.system("git clone https://github.com/TarsCloud/TarsWeb.git /usr/local/app/web")
    return

def build():
    baseDir = getBaseDir()
    doCmd("{}/framework/build/build.sh cleanall".format(baseDir))
    rCode = buildCmd("{}/framework/build/build.sh all".format(baseDir))
    if rCode == 0:
        log.info(" build success !!!")
    else:
        log.info(" build error !!!")
    return

def buildCmd(cmd):
    rCode=0
    p = subprocess.Popen(cmd, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    while p.poll() is None:
        line = p.stdout.readline()
        line = line.strip()
        if line:
           log.infoPrint(line)
    if p.returncode == 0:
        rCode =0
    else:
        rCode=-1
    p.stdin.close()
    p.stdout.close()
    return rCode

if __name__ == '__main__':
    pass