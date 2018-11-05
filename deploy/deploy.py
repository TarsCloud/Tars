#!/usr/bin/python
# encoding: utf-8
import sys
import comm.tarsBuild as tarsBuild
import comm.tarsCheck as tarsCheck
import comm.tarsDeploy as tarsDeploy
import comm.tarsTest as tarsTest
from comm.tarsUtil import *
def do():
    if len(sys.argv)==1:
        help()
        return
    param =  sys.argv[1]
    if "all" == param:
        check()
        build()
        deploy()
        test()
    elif "check"== param:
        check()
    elif "build" == param:
        build()
    elif "deploy" ==  param:
        deploy()
    elif "test" == param:
        test()
    else:
        paramError()
        return
    return

def check():
    tarsCheck.do()

def build():
    tarsBuild.do()

def deploy():
    tarsDeploy.do()

def test():
    tarsTest.do()


def help():
    helpMsg = '''This script is used to deploy tars frame
Usage: $0 Receiver Message [other]
Parameter:
    all : check the environment,build the code ,deploy and test
    check : check the environment
    build : build the code to get binary file
    deploy : deploy the frame ,you must run the command build to get  binary file
    test :  test if all the server are ok
Attention:
    1.support with python 2.7
    2.network unobstructed
    3.you had installed :gcc,gcc-c++,cmake,yasm,glibc-devel,flex,bison,ncurses-devel,zlib-devel,autoconf ,if not ,we will install it ,but It's possible to fail.
    4.Tars uses /usr/local/mysql/ as default path. If yours is not this, please modify the file CMakeLists.txt(framework/tarscpp/CMakeLists.txt, framework/CMakeLists.txt) before compile.
    '''
    print helpMsg
    return

def paramError():
    print "param error! only supprot: all,check,build,deploy or test ."
    help()
    return


def unitTest():
    print isUbuntu()
    return

if __name__ == '__main__':
    do()
    pass