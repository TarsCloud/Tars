#!/bin/bash

if (( $# < 1 ))
then
    echo $#
    echo "$0 push(false/true)"
    exit 1
fi

push=$1

if [ "$push" == "true" ]; then
	push="true"
else
	push="false"
fi

tarsTag=$(git describe --tags $(git rev-list --tags --max-count=1) --abbrev=0 --always)
cd framework && frameworkTag=$(git describe --tags $(git rev-list --tags --max-count=1) --abbrev=0 --always) && cd ..
cd web && webTag=$(git describe --tags $(git rev-list --tags --max-count=1) --abbrev=0 --always) && cd ..
echo "starting tars:${tarsTag} framework:$frameworkTag web:$webTag deploy"

cd docker/framework
./build-docker.sh $frameworkTag $webTag ${tarsTag} ${push}

cd ../../

cd docker/tars
./build-docker.sh $frameworkTag $webTag ${tarsTag} ${push}
