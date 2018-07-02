#!/bin/bash
cd $(dirname $0)
if [ -f ../src/conf/env.conf ]; then 
    echo "find   src/conf/env.conf,  please delete it   "
    exit;
fi 
rm -rf ../log/*/*/*
cd ../src
composer run deploy
mv *.tar.gz ../
