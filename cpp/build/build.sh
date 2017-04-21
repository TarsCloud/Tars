#!/bin/sh

ARGS=$1

if [ $# -lt 1 ]; then
    ARGS="help"
fi

BASEPATH=$(cd `dirname $0`; pwd)

case $ARGS in
    all)
	cd $BASEPATH; cp CMakeLists.txt ../; cmake ..;  make
        ;;
    cleanall)
        cd $BASEPATH; make clean; rm -rf CMakeFiles/ CMakeCache.txt Makefile util/ tools/ servant/ framework/ test/ cmake_install.cmake *.tgz install_manifest.txt
        ;;
    install)
        cd $BASEPATH; make install
        ;;
    help|*)
        echo "Usage:"
        echo "$0 help:     view help info."
        echo "$0 all:      build all target"
        echo "$0 install:  install framework"
        echo "$0 cleanall: remove all temp file"
        ;;
esac


