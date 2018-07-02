#!/bin/bash
cd $(dirname $0)
cd src
php ./index.php  --config=../test.conf stop 
#php ./index.php  --config=../test.conf start


