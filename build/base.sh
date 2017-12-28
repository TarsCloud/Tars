#!/bin/bash

PWD_DIR=`pwd`
MachineIp="ip addr | grep inet | grep eth0 | awk '{print $2;}' | sed 's|/.*$||'"
MachineName='hostname'

##安装lrzsz git

apt install -y git

apt install -y lrzsz

##clone code
git clone https://github.com/tencent/tars.git

##clone 
mkdir wang70937
cd wang70937

git clone https://github.com/wang70937/tars.git

##copy files
cd build
cp install-ubuntu2.sh ../tars/build/
cp settings.xml ../tars/build/
cp -r conf ../tars/build/

##
cd ../../tars/build

rz