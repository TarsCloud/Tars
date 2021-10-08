#!/bin/bash

if (( $# < 2 ))
then
    echo $#
    echo "$0 tag push(false/true)"
    exit 1
fi

tag=$1
push=$2

if [ "$push" == "true" ]; then
	push="true"
else
	push="false"
fi

# tarsTag=$(git describe --tags $(git rev-list --tags --max-count=1) --abbrev=0 --always)
# cd framework && frameworkTag=$(git describe --tags $(git rev-list --tags --max-count=1) --abbrev=0 --always) && cd ..
# cd web && webTag=$(git describe --tags $(git rev-list --tags --max-count=1) --abbrev=0 --always) && cd ..

tarsTag=$tag
cd framework && frameworkTag=$(git reflog HEAD | grep 'checkout:' | head -1 | awk '{print $NF}') && cd ..
cd web && webTag=$(git reflog HEAD | grep 'checkout:' | head -1 | awk '{print $NF}') && cd ..

echo "starting tars:${tarsTag} framework:$frameworkTag web:$webTag deploy"

cd docker/framework
./build-docker.sh $frameworkTag $webTag ${tarsTag} ${push}

cd ../../

cd docker/tars
./build-docker.sh $frameworkTag $webTag ${tarsTag} ${push}
